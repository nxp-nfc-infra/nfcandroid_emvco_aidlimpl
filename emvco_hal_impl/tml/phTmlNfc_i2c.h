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
#define NFCC_PROFILE_SWITCH _IOW(NFC_MAGIC, 0x04, uint32_t)
#define SMCU_PROFILE_SWITCH _IOW(NFC_MAGIC, 0x05, uint32_t)

/*
 * LED control via ioctl
 * RED_LED_OFF(0): RED LED OFF
 * RED_LED_ON(1):  RED LED ON
 * GREEN_LED_OFF(2): GREEN LED OFF
 * GREEN_LED_ON(3): GREEN LED ON
 */
#define LEDS_CONTROL _IOW(NFC_MAGIC, 0x06, uint32_t)

enum NfccResetType : uint32_t {
  MODE_POWER_OFF = 0x00,
  MODE_POWER_ON,
  MODE_FW_DWNLD_WITH_VEN,
  MODE_ISO_RST,
  MODE_FW_DWND_HIGH,
  MODE_POWER_RESET,
  MODE_FW_GPIO_LOW
};

enum LEDControl : uint32_t {
  RED_LED_OFF = 0x00,
  RED_LED_ON,
  GREEN_LED_OFF,
  GREEN_LED_ON
};

/* Profile mode type */
enum ProfileMode : uint32_t {
  NCI_MODE = 0,
  EMVCO_MODE,
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

int phTmlNfc_i2c_led_control(void *pDevHandle, enum LEDControl eType);

int phTmlNfc_i2c_nfcc_profile_switch(void *pDevHandle, enum ProfileMode eType);

int phTmlNfc_i2c_smcu_profile_switch(void *pDevHandle, enum ProfileMode eType);
#endif /* _PHOSALNFC_I2CTRANSPORT_H_*/