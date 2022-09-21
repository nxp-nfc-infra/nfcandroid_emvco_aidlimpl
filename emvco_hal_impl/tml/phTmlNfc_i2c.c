/******************************************************************************
 *
 *  Copyright 2022 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "phNxpNciHal_utils.h"
#include "phOsalNfc_Thread.h"
#include "phOsalNfc_Tml.h"
#include "phTmlNfc.h"
#include "phTmlNfc_i2c.h"
#include <phNfcStatus.h>
#include <phNxpLog.h>
#include <string.h>

#define CRC_LEN 2
#define NORMAL_MODE_HEADER_LEN 3
#define FW_DNLD_HEADER_LEN 2
#define FW_DNLD_LEN_OFFSET 1
#define NORMAL_MODE_LEN_OFFSET 2
#define FRAGMENTSIZE_MAX PHNFC_I2C_FRAGMENT_SIZE

static struct phOsalNfc_Thread_sOsalSemaphore mTxRxSemaphore;

extern phTmlNfc_Context_t *gpphTmlNfc_Context;

/*******************************************************************************
**
** Function         phTmlNfc_i2c_close
**
** Description      Closes NFCC device
**
** Parameters       pDevHandle - device handle
**
** Returns          None
**
*******************************************************************************/
void phTmlNfc_i2c_close(void *pDevHandle) {
  if (NULL != pDevHandle) {
    phOsalNfc_Tml_close((intptr_t)pDevHandle);
  }
  phOsalNfc_Thread_sem_destroy(&mTxRxSemaphore.ObjectHandle);
  return;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_open_and_configure
**
** Description      Open and configure NFCC device
**
** Parameters       pConfig     - hardware information
**                  pLinkHandle - device handle
**
** Returns          NFC status:
**                  NFCSTATUS_SUCCESS - open_and_configure operation success
**                  NFCSTATUS_INVALID_DEVICE - device open operation failure
**
*******************************************************************************/
NFCSTATUS phTmlNfc_i2c_open_and_configure(pphTmlNfc_Config_t pConfig,
                                          void **pLinkHandle) {
  int nHandle;

  NXPLOG_TML_D("%s Opening port=%s\n", __func__, pConfig->pDevName);
  /* open port */
  nHandle = phOsalNfc_Tml_open((const char *)pConfig->pDevName, O_RDWR);
  if (nHandle < 0) {
    NXPLOG_TML_E("_i2c_open() Failed: retval %x", nHandle);
    *pLinkHandle = NULL;
    return NFCSTATUS_INVALID_DEVICE;
  }

  *pLinkHandle = (void *)((intptr_t)nHandle);
  if (0 != phOsalNfc_Thread_sem_init(&mTxRxSemaphore.ObjectHandle, 0, 1)) {
    NXPLOG_TML_E("%s Failed: reason phOsalNfc_Thread_sem_init : retval %x",
                 __func__, nHandle);
  }
  /*Reset PN54X*/
  phTmlNfc_i2c_nfcc_reset((void *)((intptr_t)nHandle), MODE_POWER_OFF);
  usleep(10 * 1000);
  phTmlNfc_i2c_nfcc_reset((void *)((intptr_t)nHandle), MODE_POWER_ON);

  return NFCSTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         Flushdata
**
** Description      Reads payload of FW rsp from NFCC device into given buffer
**
** Parameters       pDevHandle - valid device handle
**                  pBuffer    - buffer for read data
**                  numRead    - number of bytes read by calling function
**
** Returns          always returns -1
**
*******************************************************************************/
int phTmlNfc_i2c_flush_data(void *pDevHandle, uint8_t *pBuffer, int numRead) {
  int retRead = 0;
  uint16_t totalBtyesToRead =
      pBuffer[FW_DNLD_LEN_OFFSET] + FW_DNLD_HEADER_LEN + CRC_LEN;
  /* we shall read totalBtyesToRead-1 as one byte is already read by calling
   * function*/
  retRead = phOsalNfc_Tml_read((intptr_t)pDevHandle, pBuffer + numRead,
                               totalBtyesToRead - 1);
  if (retRead > 0) {
    numRead += retRead;
    phNxpNciHal_print_packet("RECV", pBuffer, numRead);
  } else if (retRead == 0) {
    NXPLOG_TML_E("%s _i2c_read() [pyld] EOF", __func__);
  } else {
    NXPLOG_TML_D("%s _i2c_read() [hdr] received", __func__);
    phNxpNciHal_print_packet("RECV", pBuffer - numRead, NORMAL_MODE_HEADER_LEN);
    NXPLOG_TML_E("%s _i2c_read() [pyld] errno : %x", __func__, errno);
  }
  phTmlNfc_i2c_sem_post();
  return -1;
}

/*******************************************************************************
**
** Function         phOsal_Read
**
** Description      Reads requested number of bytes from NFCC device into given
**                  buffer
**
** Parameters       pDevHandle       - valid device handle
**                  pBuffer          - buffer for read data
**                  nNbBytesToRead   - number of bytes requested to be read
**
** Returns          numRead   - number of successfully read bytes
**                  -1        - read operation failure
**
*******************************************************************************/
int phTmlNfc_i2c_read(void *pDevHandle, uint8_t *pBuffer, int nNbBytesToRead) {
  NXPLOG_TML_D("%s phTmlNfc_i2c_read", __func__);
  int ret_Read;
  int ret_Select;
  int numRead = 0;
  struct timeval tv;
  fd_set rfds;
  uint16_t totalBtyesToRead = 0;

  UNUSED(nNbBytesToRead);
  if (NULL == pDevHandle) {
    return -1;
  }

  totalBtyesToRead = NORMAL_MODE_HEADER_LEN;

  /* Read with 2 second timeout, so that the read thread can be aborted
     when the NFCC does not respond and we need to switch to FW download
     mode. This should be done via a control socket instead. */
  FD_ZERO(&rfds);
  FD_SET((intptr_t)pDevHandle, &rfds);
  tv.tv_sec = 2;
  tv.tv_usec = 1;

  ret_Select =
      select((int)((intptr_t)pDevHandle + (int)1), &rfds, NULL, NULL, &tv);
  if (ret_Select < 0) {
    NXPLOG_TML_D("%s errno : %x", __func__, errno);
    return -1;
  } else if (ret_Select == 0) {
    NXPLOG_TML_D("%s Timeout", __func__);
    return -1;
  } else {
    ret_Read = phOsalNfc_Tml_read((intptr_t)pDevHandle, pBuffer,
                                  totalBtyesToRead - numRead);
    if (ret_Read > 0 && !(pBuffer[0] == 0xFF && pBuffer[1] == 0xFF)) {
      phTmlNfc_i2c_sem_timed_wait();
      numRead += ret_Read;
    } else if (ret_Read == 0) {
      NXPLOG_TML_E("%s [hdr]EOF", __func__);
      return -1;
    } else {
      NXPLOG_TML_E("%s [hdr] errno : %x", __func__, errno);
      NXPLOG_TML_E(" %s pBuffer[0] = %x pBuffer[1]= %x", __func__, pBuffer[0],
                   pBuffer[1]);
      return -1;
    }

    totalBtyesToRead = NORMAL_MODE_HEADER_LEN;
#if (NXP_EXTNS == TRUE)
    if (gpphTmlNfc_Context->tReadInfo.pContext != NULL &&
        !memcmp(gpphTmlNfc_Context->tReadInfo.pContext, "MinOpen", 0x07) &&
        !pBuffer[0] && pBuffer[1]) {
      return phTmlNfc_i2c_flush_data(pDevHandle, pBuffer, numRead);
    }
#endif

    if (numRead < totalBtyesToRead) {
      ret_Read = phOsalNfc_Tml_read((intptr_t)pDevHandle, (pBuffer + numRead),
                                    totalBtyesToRead - numRead);

      if (ret_Read != totalBtyesToRead - numRead) {
        phTmlNfc_i2c_sem_post();
        NXPLOG_TML_E("%s [hdr] errno : %x", __func__, errno);
        return -1;
      } else {
        numRead += ret_Read;
      }
    }
    totalBtyesToRead = pBuffer[NORMAL_MODE_LEN_OFFSET] + NORMAL_MODE_HEADER_LEN;
    if ((totalBtyesToRead - numRead) != 0) {
      ret_Read = phOsalNfc_Tml_read((intptr_t)pDevHandle, (pBuffer + numRead),
                                    totalBtyesToRead - numRead);
      if (ret_Read > 0) {
        numRead += ret_Read;
      } else if (ret_Read == 0) {
        phTmlNfc_i2c_sem_post();
        NXPLOG_TML_E("%s [pyld] EOF", __func__);
        return -1;
      } else {
        NXPLOG_TML_D("_i2c_read() [hdr] received");
        phNxpNciHal_print_packet("RECV", pBuffer, NORMAL_MODE_HEADER_LEN);
        phTmlNfc_i2c_sem_post();
        NXPLOG_TML_E("%s [pyld] errno : %x", __func__, errno);
        return -1;
      }
    } else {
      NXPLOG_TML_E("%s _>>>>> Empty packet recieved !!", __func__);
    }
  }
  phTmlNfc_i2c_sem_post();
  return numRead;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_write
**
** Description      Writes requested number of bytes from given buffer into
**                  NFCC device
**
** Parameters       pDevHandle       - valid device handle
**                  pBuffer          - buffer for read data
**                  nNbBytesToWrite  - number of bytes requested to be written
**
** Returns          numWrote   - number of successfully written bytes
**                  -1         - phTmlNfc_i2c_write operation failure
**
*******************************************************************************/
int phTmlNfc_i2c_write(void *pDevHandle, uint8_t *pBuffer,
                       int nNbBytesToWrite) {
  int ret;
  int numWrote = 0;
  int numBytes = nNbBytesToWrite;
  if (NULL == pDevHandle) {
    return -1;
  }
  while (numWrote < nNbBytesToWrite) {
    phTmlNfc_i2c_sem_timed_wait();
    ret = phOsalNfc_Tml_write((intptr_t)pDevHandle, pBuffer + numWrote,
                              numBytes - numWrote);
    phTmlNfc_i2c_sem_post();
    if (ret > 0) {
      numWrote += ret;
    } else if (ret == 0) {
      NXPLOG_TML_D("%s EOF", __func__);
      return -1;
    } else {
      NXPLOG_TML_D("%s errno : %x", __func__, errno);
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      return -1;
    }
  }

  return numWrote;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_nfcc_reset
**
** Description      Reset NFCC device, using VEN pin
**
** Parameters       pDevHandle     - valid device handle
**                  eType          - reset level
**
** Returns           0   - reset operation success
**                  -1   - reset operation failure
**
*******************************************************************************/
int phTmlNfc_i2c_nfcc_reset(void *pDevHandle, enum NfccResetType eType) {
  int ret = -1;
  NXPLOG_TML_D("%s, VEN eType %u", __func__, eType);

  if (NULL == pDevHandle) {
    return -1;
  }

  ret = phOsalNfc_Tml_ioctl((intptr_t)pDevHandle, NFC_SET_PWR, eType, 2);

  if (ret < 0) {
    NXPLOG_TML_E("%s :failed errno = 0x%x", __func__, errno);
  }
  return ret;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_led_control
**
** Description      Controls the RED and GREEN LED
**
** Parameters       pDevHandle     - valid device handle
**                  eType          - led control
**
** Returns           0   - reset operation success
**                  -1   - reset operation failure
**
*******************************************************************************/
int phTmlNfc_i2c_led_control(void *pDevHandle, enum LEDControl eType) {
  int ret = -1;
  NXPLOG_TML_D("%s, LEDControl eType %u", __func__, eType);

  if (NULL == pDevHandle) {
    return -1;
  }

  ret = phOsalNfc_Tml_ioctl((intptr_t)pDevHandle, LEDS_CONTROL, eType, 2);

  if (ret < 0) {
    NXPLOG_TML_E("%s :failed errno = 0x%x", __func__, errno);
  }
  return ret;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_led_control
**
** Description      sets the mode switch to NFCC
**
** Parameters       pDevHandle     - valid device handle
**                  eType          - mode switch control
**
** Returns           0   - reset operation success
**                  -1   - reset operation failure
**
*******************************************************************************/
int phTmlNfc_i2c_nfcc_profile_switch(void *pDevHandle, enum ProfileMode eType) {
  int ret = -1;
  NXPLOG_TML_D("%s, LEDControl eType %u", __func__, eType);

  if (NULL == pDevHandle) {
    return -1;
  }

  ret =
      phOsalNfc_Tml_ioctl((intptr_t)pDevHandle, NFCC_PROFILE_SWITCH, eType, 2);

  if (ret < 0) {
    NXPLOG_TML_E("%s :failed errno = 0x%x", __func__, errno);
  }
  return ret;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_led_control
**
** Description      sets the mode switch to NFCC
**
** Parameters       pDevHandle     - valid device handle
**                  eType          - mode switch control
**
** Returns           0   - reset operation success
**                  -1   - reset operation failure
**
*******************************************************************************/
int phTmlNfc_i2c_smcu_profile_switch(void *pDevHandle, enum ProfileMode eType) {
  int ret = -1;
  NXPLOG_TML_D("%s, LEDControl eType %u", __func__, eType);

  if (NULL == pDevHandle) {
    return -1;
  }

  ret =
      phOsalNfc_Tml_ioctl((intptr_t)pDevHandle, SMCU_PROFILE_SWITCH, eType, 2);

  if (ret < 0) {
    NXPLOG_TML_E("%s :failed errno = 0x%x", __func__, errno);
  }
  return ret;
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_sem_post
**
** Description      phTmlNfc_i2c_sem_post i2c_read / write
**
** Parameters       none
**
** Returns          none
*******************************************************************************/
void phTmlNfc_i2c_sem_post() {
  int sem_val = 0;
  phOsalNfc_Thread_sem_getvalue(&mTxRxSemaphore.ObjectHandle, &sem_val);
  if (sem_val == 0) {
    phOsalNfc_Thread_sem_post(&mTxRxSemaphore.ObjectHandle);
  }
}

/*******************************************************************************
**
** Function         phTmlNfc_i2c_sem_timed_wait
**
** Description      Timed sem_wait for avoiding i2c_read & write overlap
**
** Parameters       none
**
** Returns          Sem_wait return status
*******************************************************************************/
int phTmlNfc_i2c_sem_timed_wait() {
  NFCSTATUS status = NFCSTATUS_FAILED;
  long sem_timedout = 500 * 1000 * 1000;
  int s = 0;
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 0;
  ts.tv_nsec += sem_timedout;
  while ((s = phOsalNfc_Thread_sem_timedwait(&mTxRxSemaphore.ObjectHandle,
                                             &ts)) == -1 &&
         errno == EINTR) {
    continue; /* Restart if interrupted by handler */
  }
  if (s != -1) {
    status = NFCSTATUS_SUCCESS;
  } else if (errno == ETIMEDOUT && s == -1) {
    NXPLOG_TML_E("%s :timed out errno = 0x%x", __func__, errno);
  }
  return status;
}
