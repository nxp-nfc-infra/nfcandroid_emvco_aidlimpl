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
#ifndef _EMVCO_CL_H_
#define _EMVCO_CL_H_

/** \addtogroup EMVCO_STACK_CONTACT_LESS_CARD_API_INTERFACE
 *  @brief  interface to perform the EMVCo mode switch and start the EMVCo
 * polling.
 *  @{
 */

#include <emvco_common.h>
#include <emvco_tml.h>

bool_t is_set_emvco_mode;
bool_t is_stop_emvco_mode;

/**
 * @brief starts the EMVCo mode with the Device-Controller.
 *
 * @param[in] in_disc_mask EMVCo polling technologies are configured through
 * this parameter
 *
 * @return EMVCO_STATUS indicates success or failure
 *
 */
EMVCO_STATUS start_emvco_mode(const int8_t in_disc_mask);

/**
 * @brief stops the EMVCo mode with the Device-Controller.
 *
 * @return EMVCO_STATUS indicates success or failure
 *
 */
EMVCO_STATUS stop_emvco_mode();

/**
 * @brief process the NCI response and notification and ensures to do EMVCo
 * polling from EMVCo mode switch start.
 *
 * @param[in] osal_transact_info NCI response or ntf to be processed
 *
 * @return EMVCO_STATUS indicates success or failure
 *
 */
EMVCO_STATUS process_emvco_mode_rsp(osal_transact_info_t *osal_transact_info);

/** @}*/
#endif /* _EMVCO_CL_H_ */
