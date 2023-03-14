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

#ifndef _EMVCO_HAL_H_
#define _EMVCO_HAL_H_

/** \addtogroup EMVCO_DATA_EXCHANGE_STACK_HAL_API_INTERFACE
 *  @brief interface to HAL Client to realize the EMVCo functionality.
 *  @{
 */

#include <emvco_common.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * The callback passed in from the EMVCo HAL to get the
 * EMVCo status EMVCo stack .
 */
typedef void(emvco_stack_callback_t)(emvco_event_t event,
                                     emvco_status_t event_status);

/**
 * @brief
 * The callback passed in from the EMVCo HAL that EMVCo
 * stack can use to pass status to EMVCo HAL.
 */
typedef void(emvco_stack_data_callback_t)(uint16_t data_len, uint8_t *p_data);

/**
 * @brief
 * The callback passed in from the EMVCo HAL that EMVCo
 * stack can use to pass data to EMVCo HAL.
 */
typedef void(emvco_state_change_callback_t)(bool enableNfc);

/**

 *
 * @brief           This function is called by EMVCo HAL during the
 *                  initialization of the NFCC. It opens the physical connection
 *                  with NFCC (PN7220) and creates required client thread for
 *                  operation.
 *                  After open is complete, status is informed to EMVCo HAL
 *                  through callback function.
 *
 * @param[in]       p_cback provides EMVCo event status to client
 * @param[in]       p_data_cback provides EMVCo data to client
 * @param[in]       p_nfc_state_change_req_cback EMVCo HAL requests NFC module
 to turn ON or OFF the NFC through this callback
 *
 * @return          int status of the operation performed
 *
 */
int open_emvco_app_data_channel(
    emvco_stack_callback_t *p_cback, emvco_stack_data_callback_t *p_data_cback,
    emvco_state_change_callback_t *p_nfc_state_cback);

/**
 *
 * @brief           This function write the data to NFCC through physical
 *                  interface (e.g. I2C) using the PN7220 driver interface.
 *                  Before sending the data to NFCC, phEMVCoHal_write_ext
 *                  is called to check if there is any extension processing
 *                  is required for the NCI packet being sent out.
 *
 * @param[in]       data_len length of the data to be written
 * @param[in]       p_data actual data to be written
 *
 * @return          int status of the write operation performed
 *
 */
int send_emvco_app_data(uint16_t data_len, const uint8_t *p_data);

/**
 *
 * @brief       This function close the NFCC interface and free all
 *              resources.This is called by EMVCo HAL on EMVCo service stop.
 *
 * @param[in]   bShutdown true, if host is going to shutdown mode. false if host
 * is not going to shutdown mode.
 *
 * @return      int status of the close operation performed
 *
 */
int close_emvco_app_data_channel(bool);

/**
 * @brief starts the EMVCo mode with the Device-Controller.
 *
 * @param[in] dm_disc_mask EMVCo polling technologies are configured through
 * this parameter
 * @param[in] is_start_emvco specifies to start or stop the EMVCo mode
 *
 * @return void
 *
 */
void set_emvco_mode(const int8_t emvco_config, bool_t is_start_emvco);

/**
 * @brief returns the current active profile type.
 * @param[in] void
 * @return discovery_mode_t - NFC/EMVCo/Unknown
 *
 */
discovery_mode_t get_current_discovery_mode();

/**
 *
 * @brief updates NFC state to EMVCo Stack.
 *
 *
 * @param[in] in_nfcState specifies the NFC state
 *
 * @return void
 */
void on_nfc_state_change(int32_t nfc_state);

/**
 *
 * @brief stops the RF field and moves in to the specified deactivation state.
 *
 *
 * @param[in] deactivation_type specifies the state to be in after RF
 * deactivation
 *
 * @return EMVCO_STATUS returns 0, if command processed successfully and returns
 * 1, if command is not processed due to in-valid state. EMVCo mode should be ON
 * to call this API
 */
EMVCO_STATUS stop_rf_discovery(uint8_t deactivation_type);

/**
 *
 * @brief allows to turn ON/OFF the specified LED.
 *
 *
 * @param[in] led_control specifies the LED to be turned ON or OFF
 *
 * @return EMVCO_STATUS returns 0, if command processed successfully and returns
 * 1, if command is not processed due to in-valid state. EMVCo mode should be ON
 * to call this API
 */
EMVCO_STATUS set_led(uint8_t led_control);

/**
 * @brief allows to set the single byte value.
 *
 * @param[in] type - name of the config
 * @param[in] length - length of the config value
 * @param[in] value - actual byte value to be set
 *
 * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed
 * successfully and returns EMVCO_STATUS_FAILED, if command is not processed
 * successfully.
 *
 */
EMVCO_STATUS set_byte_config(config_type_t type, const int32_t in_length,
                             const int8_t in_value);

/**
 * @brief allows to set the byte array value.
 *
 * @param[in] type - name of the config
 * @param[in] length - length of the config value
 * @param[in] value - actual byte array value to be set
 *
 * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed
 * successfully and returns EMVCO_STATUS_FAILED, if command is not processed
 * successfully
 *
 */
EMVCO_STATUS set_byte_array_config(config_type_t type, const int32_t in_length,
                                   const uint8_t *in_value);

/**
 * @brief allows to set the string value.
 *
 * @param[in] type - name of the config
 * @param[in] length - length of the config value
 * @param[in] value - actual string value to be set
 *
 * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed
 * successfully and returns EMVCO_STATUS_FAILED, if command is not processed
 * successfully
 *
 */
EMVCO_STATUS set_string_config(config_type_t type, const int32_t in_length,
                               const char *p_value);

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
/** @}*/
#endif /* _EMVCO_HAL_H_ */
