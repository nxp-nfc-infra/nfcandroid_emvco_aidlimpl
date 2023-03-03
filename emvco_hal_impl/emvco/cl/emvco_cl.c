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

#include <emvco_cl.h>
#include <emvco_dm.h>
#include <emvco_log.h>
#include <emvco_nci_ext.h>
#include <emvco_ncif.h>
#include <errno.h>
#include <nci_parser.h>
#include <osal_memory.h>
#include <osal_thread.h>
#include <peripherals.h>

extern tml_emvco_context_t *gptml_emvco_context;
extern nci_hal_ctrl_t nci_hal_ctrl;
emvco_args_t *modeSwitchArgs;

pthread_mutex_t emvco_lock = PTHREAD_MUTEX_INITIALIZER;

/* the RF Discovery Frequency for each technology */
const tDISC_FREQ_CFG rf_disc_freq_cfg = {
    1, /* Frequency for EMVCo Technology A               */
    1, /* Frequency for EMVCo Technology B               */
    1, /* Frequency for EMVCo Technology F               */
    1  /* Frequency for EMVCo Technology VAS             */
};

tDISC_FREQ_CFG *p_rf_disc_freq_cfg = (tDISC_FREQ_CFG *)&rf_disc_freq_cfg;

void open_app_data_channel_internal() {
  int hal_open_status = open_app_data_channelImpl(
      m_p_nfc_stack_cback, m_p_nfc_stack_data_cback, m_p_nfc_state_cback);
  LOG_EMVCOHAL_D("%s EMVCo HAL open status:%d", __func__, hal_open_status);
  if (hal_open_status == EMVCO_STATUS_SUCCESS) {
    lib_emvco_message_t msg;
    msg.e_msgType = EMVCO_POOLING_STARTING_MSG;
    msg.p_msg_data = NULL;
    msg.size = 0;
    tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);
    start_emvco_mode();
  } else {
    lib_emvco_message_t msg;
    msg.e_msgType = EMVCO_POOLING_START_FAILED_MSG;
    msg.p_msg_data = NULL;
    msg.size = 0;
    tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);
  }
}

void handle_nfc_state_change(int32_t nfc_state) {
  LOG_EMVCOHAL_D("%s nfc_state:%d", __func__, nfc_state);
  nfc_status = nfc_state;
  if (nfc_status == STATE_OFF && modeSwitchArgs->is_start_emvco) {
    open_app_data_channel_internal();
  } else if (nfc_status == STATE_ON) {
    modeSwitchArgs->current_discovery_mode = NFC;
  }
  return;
}

uint32_t mode_switch_control(emvco_control_code_t e_ctrl_code) {
  int mode_switch_status;
  LOG_EMVCOHAL_D("%s e_ctrl_code:%d", __func__, e_ctrl_code);
  mode_switch_status = tml_ioctl(e_ctrl_code);
  LOG_EMVCOHAL_D("%s modeswitch IOCTL status:%d", __func__, mode_switch_status);
  return mode_switch_status;
}

bool is_valid_emvco_polling_tech(int8_t emvco_config) {
  if (emvco_config >= NFC_A_PASSIVE_POLL_MODE &&
      emvco_config <= NFC_ABFVAS_PASSIVE_POLL_MODE &&
      emvco_config != NFC_A_PASSIVE_POLL_MODE &&
      emvco_config != NFC_B_PASSIVE_POLL_MODE &&
      emvco_config != NFC_AF_PASSIVE_POLL_MODE &&
      emvco_config != NFC_BF_PASSIVE_POLL_MODE &&
      emvco_config != NFC_VAS_PASSIVE_POLL_MODE &&
      emvco_config != NFC_AVAS_PASSIVE_POLL_MODE &&
      emvco_config != NFC_BVAS_PASSIVE_POLL_MODE &&
      emvco_config != NFC_FVAS_PASSIVE_POLL_MODE &&
      emvco_config != NFC_AFVAS_PASSIVE_POLL_MODE &&
      emvco_config != NFC_BFVAS_PASSIVE_POLL_MODE) {
    return true;
  } else {
    return false;
  }
}

void handle_set_emvco_mode(const int8_t emvco_config, bool_t is_start_emvco) {
  LOG_EMVCOHAL_D("%s emvco_config:%d is_start_emvco:%d", __func__, emvco_config,
                 is_start_emvco);
  pthread_mutex_lock(&emvco_lock);
  modeSwitchArgs->is_start_emvco = is_start_emvco;
  modeSwitchArgs->emvco_config = emvco_config;
  if (is_start_emvco) {
    if (is_valid_emvco_polling_tech(emvco_config)) {
      m_p_nfc_state_cback(false);
    } else {
      LOG_EMVCOHAL_D("%s In-valid polling technlogy", __func__);
      (*m_p_nfc_stack_cback)(EMVCO_POOLING_START_EVT, STATUS_FAILED);
    }
  } else {
    if (nci_hal_ctrl.halStatus == HAL_STATUS_OPEN) {
      LOG_EMVCOHAL_D("%s HAL is open, stop_emvco_mode", __func__);
      stop_emvco_mode();
    } else {
      LOG_EMVCOHAL_D("%s HAL is not open, No need to clean up", __func__);
    }
  }
  pthread_mutex_unlock(&emvco_lock);
  return;
}

discovery_mode_t get_current_mode() {
  LOG_EMVCOHAL_D("%s current_discovery_mode:%d", __func__,
                 modeSwitchArgs->current_discovery_mode);
  return modeSwitchArgs->current_discovery_mode;
}
/**
 *
 * Description      Build RF discovery configurations from
 *                  tNFA_DM_DISC_TECH_PROTO_MASK
 *
 * Returns          number of RF discovery configurations
 *
 */
uint8_t get_rf_discover_config(tDISC_TECH_PROTO_MASK dm_disc_mask,
                               tEMVCO_DISCOVER_PARAMS disc_params[],
                               uint8_t max_params) {
  uint8_t num_params = 0;
  /* Check polling A */
  if (dm_disc_mask & DISC_MASK_EMVCO_A_PASSIVE_POLL_MODE) {
    disc_params[num_params].type = NCI_DISCOVERY_TYPE_POLL_A;
    disc_params[num_params].frequency = p_rf_disc_freq_cfg->pa;
    num_params++;
  }
  if (dm_disc_mask & DISC_MASK_EMVCO_B_PASSIVE_POLL_MODE) {
    disc_params[num_params].type = NCI_DISCOVERY_TYPE_POLL_B;
    disc_params[num_params].frequency = p_rf_disc_freq_cfg->pb;
    num_params++;
  }
  if (dm_disc_mask & DISC_MASK_EMVCO_F_PASSIVE_POLL_MODE) {
    disc_params[num_params].type = NCI_DISCOVERY_TYPE_POLL_F;
    disc_params[num_params].frequency = p_rf_disc_freq_cfg->pb;
    num_params++;
  }
  if (dm_disc_mask & DISC_MASK_EMVCO_VAS_PASSIVE_POLL_MODE) {
    disc_params[num_params].type = NCI_DISCOVERY_TYPE_POLL_VAS;
    disc_params[num_params].frequency = p_rf_disc_freq_cfg->pb;
    num_params++;
  }
  if (num_params >= max_params)
    return num_params;

  return num_params;
}

EMVCO_STATUS start_emvco_mode() {
  LOG_EMVCOHAL_D("%s", __func__);
  uint8_t cmd_prop_act[] = {0x2F, 0x02, 0x00};
  send_proprietary_act_cmd(sizeof(cmd_prop_act), cmd_prop_act);
  return EMVCO_STATUS_SUCCESS;
}

EMVCO_STATUS stop_emvco_mode() {
  LOG_EMVCOHAL_D("stop_emvco_mode nci_stop_discovery without modeswitch");
  led_switch_control(GREEN_LED_OFF);
  int hal_close_status = close_app_data_channel(true);
  LOG_EMVCOHAL_D("%s EMVCO HAL close status:%d", __func__, hal_close_status);

  (*m_p_nfc_state_cback)(true);
  return EMVCO_STATUS_SUCCESS;
}

EMVCO_STATUS process_emvco_mode_rsp(osal_transact_info_t *pTransactionInfo) {
  LOG_EMVCOHAL_D("process_emvco_mode_rsp");
  uint8_t *p_ntf = pTransactionInfo->p_buff;
  uint16_t p_len = pTransactionInfo->w_length;
  if (!modeSwitchArgs->is_start_emvco) {
    return EMVCO_STATUS_FAILED;
  }
  LOG_EMVCOHAL_D("process_emvco_mode_rsp data_len:%d "
                 "modeSwitchArgs->is_start_emvco:%d",
                 p_len, modeSwitchArgs->is_start_emvco);
  uint8_t msg_type, pbf, group_id, op_code, *p_data;

  p_data = p_ntf;
  NCI_MSG_PRS_HDR0(p_data, msg_type, pbf, group_id);
  LOG_EMVCOHAL_D("process_emvco_mode_rsp msg_type:%d group_id:%d ", msg_type,
                 group_id);

  if (!(NCI_GID_CORE == group_id || NCI_GID_PROP == group_id ||
        NCI_GID_RF_MANAGE == group_id)) {
    return EMVCO_STATUS_FAILED;
  }
  NCI_MSG_PRS_HDR1(p_data, op_code);
  LOG_EMVCOHAL_D("process_emvco_mode_rsp op_code:%d", op_code);
  p_data = p_ntf;

  switch (msg_type) {
  case NCI_MSG_TYPE_RSP:
    switch (group_id) {
    case NCI_GID_PROP:
      switch (op_code) {
      case MSG_CORE_PROPRIETARY_RSP:
        if (p_len == 8) {
          uint8_t num_params;
          tEMVCO_DISCOVER_PARAMS disc_params[MAX_DISC_PARAMS];
          num_params = get_rf_discover_config(modeSwitchArgs->emvco_config,
                                              disc_params, MAX_DISC_PARAMS);
          LOG_EMVCOHAL_D("RFDiscover num_params:%d", num_params);
          send_discover_cmd(num_params, disc_params);
        }
        break;
      }
      break;
    case NCI_GID_RF_MANAGE:
      switch (op_code) {
      case MSG_RF_DISCOVER_RSP: {
        if (p_len == 4) {
          LOG_EMVCOHAL_D("EMVCO_POLLING_STARTED_MSG");
          nci_hal_ctrl.frag_rsp.data_pos = 0;
          RESET_CHAINED_DATA();

          modeSwitchArgs->current_discovery_mode = EMVCO;
          lib_emvco_message_t msg;
          msg.e_msgType = EMVCO_POLLING_STARTED_MSG;
          msg.p_msg_data = NULL;
          msg.size = 0;
          tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);
        }
      } break;
      }
    }
    break;
  }
  return EMVCO_STATUS_SUCCESS;
}

EMVCO_STATUS rf_deactivate(uint8_t deactivation_type) {
  LOG_EMVCOHAL_D("rf_deactivate");
  if (EMVCO != modeSwitchArgs->current_discovery_mode) {
    LOG_EMVCOHAL_E("rf_deactivate failed - Not in EMVCo mode");
    return EMVCO_STATUS_FAILED;
  } else {
    return send_deactivate_cmd(deactivation_type);
  }
}
