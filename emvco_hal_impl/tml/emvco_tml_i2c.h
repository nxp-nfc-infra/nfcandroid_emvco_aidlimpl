/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
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

#ifndef _EMVCO_TML_I2C_H_
#define _EMVCO_TML_I2C_H_

#include <emvco_tml.h>
#include <emvco_types.h>

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

typedef enum LEDControl : uint32_t {
  RED_LED_OFF = 0x00,
  RED_LED_ON,
  GREEN_LED_OFF,
  GREEN_LED_ON
} led_control_t;

/* Profile mode type */
enum ProfileMode : uint32_t {
  NCI_MODE = 0,
  EMVCO_MODE,
};

/**
 *
 * @brief      Closes NFCC device
 *
 * @param[in]       p_dev_handle - device handle
 *
 * @return          None
 *
 **/
void i2c_close(void *p_dev_handle);

/**
 *
 * @brief       Open and configure NFCC device and transport layer
 *
 * @param[in]       pConfig     - hardware information
 * @param[in]                 pLinkHandle - device handle
 *
 * @return           NFC status:
 *                  EMVCO_STATUS_SUCCESS - open_and_configure operation success
 *                  EMVCO_STATUS_INVALID_DEVICE - device open operation failure
 *
 */
EMVCO_STATUS i2c_open_and_configure(ptml_emvco_Config_t pConfig,
                                    void **pLinkHandle);

/**
 *
 * @brief      Reads requested number of bytes from NFCC device into
 *                 given buffer
 *
 * @param[in]       p_dev_handle       - valid device handle
 * @param[in]       p_buffer          - buffer for read data
 * @param[in]       nNbBytesToRead   - number of bytes requested to be read
 *
 * @return          numRead   - number of successfully read bytes
 *                  -1        - read operation failure
 *
 */
int i2c_read(void *p_dev_handle, uint8_t *p_buffer, int nNbBytesToRead);

/**
 *
 * @brief      Writes requested number of bytes from given buffer into
 *                  NFCC device
 *
 * @param[in]       p_dev_handle       - valid device handle
 * @param[in]       p_buffer          - buffer for read data
 * @param[in]       nNbBytesToWrite  - number of bytes requested to be written
 *
 * @return           numWrote   - number of successfully written bytes
 *                  -1         - write operation failure
 *
 */
int i2c_write(void *p_dev_handle, uint8_t *p_buffer, int nNbBytesToWrite);

/**
 *
 * @brief      Reset NFCC device, using VEN pin
 *
 * @param[in]       p_dev_handle     - valid device handle
 * @param[in]       eType          - NfccResetType
 *
 * @return            0   - reset operation success
 *                  -1   - reset operation failure
 *
 */
int i2c_nfcc_reset(void *p_dev_handle, enum NfccResetType eType);

/**
 *
 * @brief      Timed sem_wait for avoiding i2c_read & write overlap
 *
 * @param[in]       none
 *
 * @return          Sem_wait return status
 ***************************************************************************/
int i2c_sem_timed_wait();

/**
 * @brief       sem_post 2c_read / write
 *
 * @param[in]       none
 *
 * @return          none
 */
void i2c_sem_post();

/**
 *
 * @brief      Reads payload of FW rsp from NFCC device into given buffer
 *
 * @param[in]        p_dev_handle - valid device handle
 * @param[in]        p_buffer    - buffer for read data
 * @param[in]        numRead    - number of bytes read by calling function
 *
 * @return          always returns -1
 *
 */
int i2c_flush_data(void *p_dev_handle, uint8_t *p_buffer, int numRead);

/**
 *
 * @brief      Controls the RED and GREEN LED
 *
 * @param[in]       p_dev_handle     - valid device handle
 *                  eType          - led control
 *
 * @return           0   - reset operation success
 *                  -1   - reset operation failure
 *
 */
int i2c_led_control(void *p_dev_handle, enum LEDControl eType);

/**
 *
 * @brief      sets the mode switch to NFCC
 *
 * @param[in]       p_dev_handle     - valid device handle
 *                  eType          - mode switch control
 *
 * @return           0   - reset operation success
 *                  -1   - reset operation failure
 *
 */
int i2c_nfcc_profile_switch(void *p_dev_handle, enum ProfileMode eType);

/**
 *
 * @brief       sets the mode switch to NFCC
 *
 * @param[in]       p_dev_handle     - valid device handle
 *                  eType          - mode switch control
 *
 * @return           0   - reset operation success
 *                  -1   - reset operation failure
 *
 */
int i2c_smcu_profile_switch(void *p_dev_handle, enum ProfileMode eType);
#endif /* _EMVCO_TML_I2C_H_*/