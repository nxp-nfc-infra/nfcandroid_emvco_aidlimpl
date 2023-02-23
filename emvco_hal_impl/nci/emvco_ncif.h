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
/** \addtogroup EMVCO_STACK_NCI_API_INTERFACE
 *  @brief interface to send the NCI packets and process the fragmented data
 * received & loop back to NFCC
 *  @{
 */
#ifndef EMVCO_NCIF_H
#define EMVCO_NCIF_H

#include <emvco_cl.h>
#include <stdint.h>

#define MAX_FRAGMENT_SIZE 253
#define PBF_COMPLETE_MSG 0x00
#define PBF_SEGMENT_MSG 0x10
/* Macros to update the chained data state */
#define SET_CHAINED_DATA() (nci_hal_ctrl.frag_rsp.is_chained = 1)
#define RESET_CHAINED_DATA() (nci_hal_ctrl.frag_rsp.is_chained = 0)
#define IS_CHAINED_DATA() (1 == nci_hal_ctrl.frag_rsp.is_chained)
/**
 *
 * @brief      compose and send CORE RESET command to command queue
 * @param[in]  reset_type specfies to keep the configuration or not
 * @return          NFC status:
 *                  NFCSTATUS_SUCCESS - command processed successfully
 *                  NFCSTATUS_FAILED - failed to process the command
 *
 */
uint8_t send_core_reset(uint8_t reset_type);

/**
 * @brief       compose and send CORE INIT command to command queue
 * @param[in]   nci_version specfies the nci version
 * @return          NFC status:
 *                  NFCSTATUS_SUCCESS - command processed successfully
 *                  NFCSTATUS_FAILED - failed to process the command
 *
 */
uint8_t send_core_init(uint8_t nci_version);

/**
 *
 * @brief       compose and send CORE SET_CONFIG command to command queue
 * @param[in]       p_param_tlvs parameter in TLV format
 * @param[in]       tlv_size tlv parameter size
 * @return          NFC status:
 *                  NFCSTATUS_SUCCESS - command processed successfully
 *                  NFCSTATUS_FAILED - failed to process the command
 *
 */
uint8_t send_core_set_config(uint8_t *p_param_tlvs, uint8_t tlv_size);

/**
 *
 * @brief           compose and send RF Management DISCOVER command to command
 *                  queue
 *
 * @param[in]       num number of polling types
 * @param[in]       p_param contains type and frequency of the polling type
 *
 * @return          NFC status:
 *                  NFCSTATUS_SUCCESS - command processed successfully
 *                  NFCSTATUS_FAILED - failed to process the command
 *
 */
uint8_t send_discover_cmd(uint8_t num, tEMVCO_DISCOVER_PARAMS *p_param);

/**
 *
 * @brief           compose and sends proprietary command to controller
 *
 * @param[in]       data_len command length
 * @param[in]       p_data   command data
 *
 * @return          NFC status:
 *                  NFCSTATUS_SUCCESS - command processed successfully
 *                  NFCSTATUS_FAILED - failed to process the command
 *
 **/
uint8_t send_proprietary_act_cmd(uint16_t data_len, uint8_t *p_data);

/**
 *
 * @brief           Process the Received data packet from NFCC,
 *                  assembles and send it as complete packet
 *                  to upper layer
 *
 * @param[in]       data_len command length
 * @param[in]       p_data   command data
 *
 * @return          void
 *
 **/
void process_emvco_data(uint8_t *p_data, uint16_t data_len);

/**
 *
 * @brief           Process the Received data packet from Upper layer,
 *                  de-assembles, if length is more the Max fragment data
 *                  length and send it to NFCC
 *
 * @param[in]       data_len command length
 * @param[in]       p_data   command data
 *
 * @return          void
 *
 **/
void send_emvco_data(uint8_t *p_ntf, uint16_t p_len);

/** @}*/
#endif /* EMVCO_NCIF_H */