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
#ifndef _EMVCO_CL_H_
#define _EMVCO_CL_H_

/** \addtogroup EMVCO_STACK_CONTACT_LESS_CARD_API_INTERFACE
 *  @brief  interface to perform the EMVCo mode switch and start the EMVCo
 * polling.
 *  @{
 */

#include <emvco_common.h>
#include <emvco_tml.h>

/* max discovery technology parameters */
#define MAX_DISC_PARAMS 16

#define DISC_MASK_PA_NFC_DEP 0x00000008

/* Discovery Types/Detected Technology and Mode */
#define NCI_DISCOVERY_TYPE_POLL_A 0x00
#define NCI_DISCOVERY_TYPE_POLL_B 0x01
#define NCI_DISCOVERY_TYPE_POLL_F 0x02
#define NCI_DISCOVERY_TYPE_POLL_VAS 0x74

#define DISC_MASK_EMVCO_A_PASSIVE_POLL_MODE 0x01
#define DISC_MASK_EMVCO_B_PASSIVE_POLL_MODE 0x02
#define DISC_MASK_EMVCO_F_PASSIVE_POLL_MODE 0x04
#define DISC_MASK_EMVCO_VAS_PASSIVE_POLL_MODE 0x08

/**
 * @brief compile-time configuration structure for the RF Discovery Frequency
 * for each technology
 */
typedef struct {
  uint8_t pa;   /* Frequency for EMVCo Technology A   */
  uint8_t pb;   /* Frequency for EMVCo Technology B   */
  uint8_t pf;   /* Frequency for EMVCo Technology F   */
  uint8_t pvas; /* Frequency for EMVCo Technology VAS */
} tDISC_FREQ_CFG;

/**
 * @brief EMVCo polling type structure species type value and frequency value
 * technology
 */
typedef struct {
  uint8_t type;
  uint8_t frequency;
} tEMVCO_DISCOVER_PARAMS;

/**
 * @brief EMVCo polling discovery mask
 *
 */
typedef uint32_t tDISC_TECH_PROTO_MASK;

/**
 * @brief emvco discovery configuration structure for emvco polling
 * contains emvco polling type mask and start/stop flag
 */
typedef struct emvco_args {
  int8_t emvco_config;
  bool_t is_start_emvco;
  int8_t current_discovery_mode;
} emvco_args_t;

void nfc_state_changed(int32_t nfc_state);

uint8_t get_rf_discover_config(tDISC_TECH_PROTO_MASK dm_disc_mask,
                               tEMVCO_DISCOVER_PARAMS disc_params[],
                               uint8_t max_params);

uint32_t mode_switch_control(emvco_control_code_t e_ctrl_code);

void set_emvco_mode_impl(const int8_t emvco_config, bool_t is_start_emvco);
/**
 * @brief starts the EMVCo mode with the Device-Controller.
 *
 * @return EMVCO_STATUS indicates success or failure
 *
 */
EMVCO_STATUS start_emvco_mode();

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

/**
 * @brief starts/stops the EMVCo mode with the Device-Controller.
 *
 * @param[in] in_disc_mask EMVCo polling technologies are configured through
 * this parameter
 * @param[in] is_start_emvco specifies to start or stop the EMVCo mode
 *
 * @return void
 *
 */
void handle_set_emvco_mode(const int8_t in_disc_mask, bool_t is_start_emvco);

/**
 * @brief returns the current active profile type.
 * @param[in] void
 * @return discovery_mode_t - NFC/EMVCo/Unknown
 *
 */
discovery_mode_t get_current_mode();

/**
 *
 * @brief updates NFC state to EMVCo Stack.
 *
 *
 * @param[in] nfc_state specifies the NFC state
 *
 * @return void
 */
void handle_nfc_state_change(int32_t nfc_state);

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
EMVCO_STATUS rf_deactivate(uint8_t deactivation_type);

/** @}*/
#endif /* _EMVCO_CL_H_ */
