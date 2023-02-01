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
#ifndef _EMVCO_NCI_EXT_H_
#define _EMVCO_NCI_EXT_H_

/** \addtogroup EMVCO_STACK_NCI_API_INTERFACE
 *  @brief  interface to send and process the extension command
 *  @{
 */

#include <emvco_dm.h>
#include <emvco_tml.h>
#include <string.h>

#define NCI_MT_DATA 0x00
#define NCI_MT_CMD 0x20
#define NCI_MT_RSP 0x40
#define NCI_MT_NTF 0x60
#define NCI_CORE_CONN_CREDITS_NTF 0x06
#define NCI_CORE_CONN_CREDITS_NTF_LEN 0x03
#define NCI_CORE_CONN_CREDITS_NTF_NO_OF_ENTRY 0x01
#define NCI_CORE_CONN_CREDITS_NTF_CONN_ID 0x00
#define NCI_CORE_CONN_CREDITS_NTF_CONN_CREDITS 0x01

void nci_ext_init(void);

/**
 *
 * @brief        Process extension function response
 *
 * @param[in] p_cmd command to be processed
 * @param[in] p_len command length
 *
 * @return          EMVCO_STATUS_SUCCESS if success
 *
 */
EMVCO_STATUS process_ext_rsp(uint8_t *p_data, uint16_t *p_cmd);

/**
 *
 * @brief        sends the extension command
 *
 * @param[in] p_cmd command to be processed
 * @param[in] p_len command length
 *
 * @return          EMVCO_STATUS_SUCCESS if success
 *
 */
EMVCO_STATUS send_ext_cmd(uint16_t p_len, uint8_t *p_cmd);

/**
 * @brief      This function inform the status of open_app_data_channel
 *                  function to EMVCo HAL.
 *
 * @param[in] p_cmd_data command
 * @param[in] cmd_len command length
 * @param[in] p_rsp_data response data
 * @param[in] rsp_len response length
 *
 * @return          It return EMVCO_STATUS_SUCCESS then continue with send else
 *                  sends EMVCO_STATUS_FAILED direct response is prepared and
 *                  do not send anything to NFCC.
 *
 ******************************************************************************/
EMVCO_STATUS send_app_data_ext(uint16_t *cmd_len, uint8_t *p_cmd_data,
                               uint16_t *rsp_len, uint8_t *p_rsp_data);
/** @}*/
#endif /* _PHNXPNICHAL_EXT_H_ */
