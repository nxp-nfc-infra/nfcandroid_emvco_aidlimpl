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

/*
 * @brief
 * The callback passed in from the EMVCo HAL to get the
 * EMVCo status EMVCo stack .
 */
typedef void(emvco_stack_callback_t)(emvco_event_t event,
                                     emvco_status_t event_status);

/*
 * @brief
 * The callback passed in from the EMVCo HAL that EMVCo
 * stack can use to pass status to EMVCo HAL.
 */
typedef void(emvco_stack_data_callback_t)(uint16_t data_len, uint8_t *p_data);

/*
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
 * @param[in] in_isStartEMVCo specifies to start or stop the EMVCo mode
 *
 * @return void
 *
 */
void set_emvco_mode(const int8_t emvco_config, bool_t in_isStartEMVCo);

/**
 *
 * @brief updates NFC state to EMVCo Stack.
 *
 *
 * @param[in] in_nfcState specifies the NFC state
 *
 * @return ::ndk::ScopedAStatus indicates onNfcStateChange request processed by
 * EMVCo HAL successfully or not
 */
void on_nfc_state_change(int32_t nfc_state);

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
/** @}*/
#endif /* _EMVCO_HAL_H_ */
