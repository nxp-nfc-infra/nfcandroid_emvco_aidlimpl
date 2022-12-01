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

#include <emvco_cl.h>
#include <emvco_log.h>
#include <emvco_nci_ext.h>
#include <nci_parser.h>

extern tml_emvco_context_t *gptml_emvco_context;

uint8_t cmd_start_discovery_f[6] = {0x21, 0x03, 0x03, 0x01, 0x02, 0x01};
uint8_t cmd_start_discovery_ab[8] = {0x21, 0x03, 0x05, 0x02,
                                     0x00, 0x01, 0x01, 0x01};
uint8_t cmd_start_discovery_abf[10] = {0x21, 0x03, 0x07, 0x03, 0x00,
                                       0x01, 0x01, 0x01, 0x02, 0x01};
uint8_t cmd_start_discovery_abvas[10] = {0x21, 0x03, 0x07, 0x03, 0x00,
                                         0x01, 0x01, 0x01, 0x74, 0x01};
uint8_t cmd_start_discovery_abfvas[12] = {0x21, 0x03, 0x09, 0x04, 0x00, 0x01,
                                          0x01, 0x01, 0x02, 0x01, 0x74, 0x01};
uint8_t cmd_start_discovery[12];

void phNxpNciHal_configure_pooling_tech(const int8_t emvco_config) {
  LOG_EMVCOHAL_D("%s emvco_config:%d", __func__, emvco_config);
  switch (emvco_config) {
  case NFC_F_PASSIVE_POLL_MODE:
    memcpy(cmd_start_discovery, cmd_start_discovery_f,
           sizeof(cmd_start_discovery_f) / sizeof(uint8_t));
    break;
  case NFC_AB_PASSIVE_POLL_MODE:
    memcpy(cmd_start_discovery, cmd_start_discovery_ab,
           sizeof(cmd_start_discovery_ab) / sizeof(uint8_t));
    break;
  case NFC_ABF_PASSIVE_POLL_MODE:
    memcpy(cmd_start_discovery, cmd_start_discovery_abf,
           sizeof(cmd_start_discovery_abf) / sizeof(uint8_t));
    break;
  case NFC_ABVAS_PASSIVE_POLL_MODE:
    memcpy(cmd_start_discovery, cmd_start_discovery_abvas,
           sizeof(cmd_start_discovery_abvas) / sizeof(uint8_t));
    break;
  case NFC_ABFVAS_PASSIVE_POLL_MODE:
    memcpy(cmd_start_discovery, cmd_start_discovery_abfvas,
           sizeof(cmd_start_discovery_abfvas) / sizeof(uint8_t));
    break;
  default:
    LOG_EMVCOHAL_D("%s default case", __func__);
    break;
  }
}

EMVCO_STATUS start_emvco_mode(const int8_t emvco_config) {
  LOG_EMVCOHAL_D("start_emvco_mode send cmd_prop_act");
  phNxpNciHal_configure_pooling_tech(emvco_config);
  is_set_emvco_mode = true;
  EMVCO_STATUS status = EMVCO_STATUS_SUCCESS;
  uint8_t cmd_prop_act[] = {0x2F, 0x02, 0x00};
  send_app_data(sizeof(cmd_prop_act), cmd_prop_act);
  return status;
}

EMVCO_STATUS stop_emvco_mode() {
  LOG_EMVCOHAL_D("stop_emvco_mode send nci_stop_discovery");
  is_stop_emvco_mode = true;
  EMVCO_STATUS status = EMVCO_STATUS_SUCCESS;
  int mode_switch_status = tml_ioctl(NFCCModeSwitchOff);
  LOG_EMVCOHAL_D("%s EMVCO_MODE_OFF status:%d", __func__, mode_switch_status);
  led_switch_control(EMVCO_MODE_OFF);
  int hal_close_status = close_app_data_channel(true);
  LOG_EMVCOHAL_D("%s EMVCO HAL close status:%d", __func__, hal_close_status);

  (*m_p_nfc_state_cback)(true);
  return status;
}

void *process_emvco_mode_rsp_impl(void *vargp) {
  osal_transact_info_t *pTransactionInfo = vargp;
  uint8_t *p_ntf = pTransactionInfo->p_buff;
  uint16_t p_len = pTransactionInfo->w_length;

  if (!is_set_emvco_mode) {
    return NULL;
  }
  LOG_EMVCOHAL_D("process_emvco_mode_rsp_impl data_len:%d is_set_emvco_mode:%d",
                 p_len, is_set_emvco_mode);
  uint8_t msg_type, pbf, group_id, op_code, *p_data;

  p_data = p_ntf;
  NCI_MSG_PRS_HDR0(p_data, msg_type, pbf, group_id);
  LOG_EMVCOHAL_D("process_emvco_mode_rsp msg_type:%d group_id:%d ", msg_type,
                 group_id);

  if (!(NCI_GID_CORE == group_id || NCI_GID_PROP == group_id ||
        NCI_GID_RF_MANAGE == group_id)) {
    return NULL;
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
          LOG_EMVCOHAL_D("send cmd_start_discovery");
          int size_of_cmd_start_discovery = cmd_start_discovery[2] + 3;
          send_app_data(size_of_cmd_start_discovery, cmd_start_discovery);
        }
        break;
      }
      break;
    case NCI_GID_RF_MANAGE:
      switch (op_code) {
      case MSG_RF_DISCOVER_RSP: {
        if (p_len == 4) {
          LOG_EMVCOHAL_D("EMVCO_POLLING_STARTED_MSG");
          lib_emvco_message_t msg;
          msg.e_msgType = EMVCO_POLLING_STARTED_MSG;
          msg.p_msg_data = NULL;
          msg.size = 0;
          tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);
        }
      } break;
      case RF_DEACTIVATE_NTF: {
        if (p_len == 4 && is_stop_emvco_mode) {
          is_stop_emvco_mode = false;
          LOG_EMVCOHAL_D("RF_DEACTIVATE_RSP");
          lib_emvco_message_t msg;
          msg.e_msgType = EMVCO_POLLING_STOP_EVT;
          msg.p_msg_data = NULL;
          msg.size = 0;
          tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);
        }
      } break;
      }
    }
    break;
  }
  return NULL;
}

EMVCO_STATUS
process_emvco_mode_rsp(osal_transact_info_t *pTransactionInfo) {
  pthread_t thread_id;
  int pthread_create_status = 0;

  pthread_create_status = pthread_create(
      &thread_id, NULL, process_emvco_mode_rsp_impl, pTransactionInfo);
  if (0 != pthread_create_status) {
    /* thread create failed */
    return EMVCO_STATUS_FAILED;
  }
  return EMVCO_STATUS_SUCCESS;
}
