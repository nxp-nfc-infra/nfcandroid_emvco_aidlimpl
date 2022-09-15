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

#ifndef _PHOSALNFC_I2CTRANSPORT_H_
#define _PHOSALNFC_I2CTRANSPORT_H_

#include <phNfcTypes.h>
#include <phTmlNfc.h>

#define NFC_MAGIC 0xE9
/*
 * NFCC power control via ioctl
 * NFC_SET_PWR(0): power off
 * NFC_SET_PWR(1): power on
 * NFC_SET_PWR(2): reset and power on with firmware download enabled
 */
#define NFC_SET_PWR _IOW(NFC_MAGIC, 0x01, uint32_t)

/*
 * get platform interface type(i2c or i3c) for common MW
 * return 0 - i2c, 1 - i3c
 */
#define NFC_GET_PLATFORM_TYPE _IO(NFC_MAGIC, 0x04)
/*
 * get boot state
 * return unknown, fw dwl, fw teared, nci
 */
#define NFC_GET_NFC_STATE _IO(NFC_MAGIC, 0x05)

/* NFC HAL can call this ioctl to get the current IRQ state */
#define NFC_GET_IRQ_STATE _IO(NFC_MAGIC, 0x06)

enum NfccResetType : uint32_t {
  MODE_POWER_OFF = 0x00,
  MODE_POWER_ON,
  MODE_FW_DWNLD_WITH_VEN,
  MODE_ISO_RST,
  MODE_FW_DWND_HIGH,
  MODE_POWER_RESET,
  MODE_FW_GPIO_LOW
};

/*****************************************************************************
 **
 ** Function         Close
 **
 ** Description      Closes NFCC device
 **
 ** Parameters       pDevHandle - device handle
 **
 ** Returns          None
 **
 *****************************************************************************/
void phTmlNfc_i2c_close(void *pDevHandle);

/*****************************************************************************
 **
 ** Function         OpenAndConfigure
 **
 ** Description      Open and configure NFCC device and transport layer
 **
 ** Parameters       pConfig     - hardware information
 **                  pLinkHandle - device handle
 **
 ** Returns          NFC status:
 **                  NFCSTATUS_SUCCESS - open_and_configure operation success
 **                  NFCSTATUS_INVALID_DEVICE - device open operation failure
 **
 ****************************************************************************/
NFCSTATUS phTmlNfc_i2c_open_and_configure(pphTmlNfc_Config_t pConfig,
                                          void **pLinkHandle);

/*****************************************************************************
 **
 ** Function         Read
 **
 ** Description      Reads requested number of bytes from NFCC device into
 **                 given buffer
 **
 ** Parameters       pDevHandle       - valid device handle
 **                  pBuffer          - buffer for read data
 **                  nNbBytesToRead   - number of bytes requested to be read
 **
 ** Returns          numRead   - number of successfully read bytes
 **                  -1        - read operation failure
 **
 ****************************************************************************/
int phTmlNfc_i2c_read(void *pDevHandle, uint8_t *pBuffer, int nNbBytesToRead);

/*****************************************************************************
 **
 ** Function         Write
 **
 ** Description      Writes requested number of bytes from given buffer into
 **                  NFCC device
 **
 ** Parameters       pDevHandle       - valid device handle
 **                  pBuffer          - buffer for read data
 **                  nNbBytesToWrite  - number of bytes requested to be
 *written
 **
 ** Returns          numWrote   - number of successfully written bytes
 **                  -1         - write operation failure
 **
 *****************************************************************************/
int phTmlNfc_i2c_write(void *pDevHandle, uint8_t *pBuffer, int nNbBytesToWrite);

/*****************************************************************************
 **
 ** Function         Reset
 **
 ** Description      Reset NFCC device, using VEN pin
 **
 ** Parameters       pDevHandle     - valid device handle
 **                  eType          - NfccResetType
 **
 ** Returns           0   - reset operation success
 **                  -1   - reset operation failure
 **
 ****************************************************************************/
int phTmlNfc_i2c_nfcc_reset(void *pDevHandle, enum NfccResetType eType);

/*****************************************************************************
**
** Function         GetNfcState
**
** Description      Get NFC state
**
** Parameters       pDevHandle     - valid device handle
** Returns           0   - unknown
**                   1   - FW DWL
**                   2   - NCI
**
*****************************************************************************/
int phTmlNfc_i2c_get_nfc_state(void *pDevHandle);

/*****************************************************************************
 **
 ** Function         SemTimedWait
 **
 ** Description      Timed sem_wait for avoiding i2c_read & write overlap
 **
 ** Parameters       none
 **
 ** Returns          Sem_wait return status
 ****************************************************************************/
int phTmlNfc_i2c_sem_timed_wait();

/*****************************************************************************
 **
 ** Function         SemPost
 **
 ** Description      sem_post 2c_read / write
 **
 ** Parameters       none
 **
 ** Returns          none
 ****************************************************************************/
void phTmlNfc_i2c_sem_post();

int phTmlNfc_i2c_flush_data(void *pDevHandle, uint8_t *pBuffer, int numRead);

#endif /* _PHOSALNFC_I2CTRANSPORT_H_*/