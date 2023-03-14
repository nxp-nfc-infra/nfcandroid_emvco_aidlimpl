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
#include <emvco_config.h>
#include <emvco_dm.h>
#include <emvco_hal.h>
#include <emvco_log.h>
#include <emvco_ncif.h>
#include <map.h>
#include <peripherals.h>
#define MAX_KEY_LENGTH 3
extern emvco_args_t *modeSwitchArgs;

int open_emvco_app_data_channel(
    emvco_stack_callback_t *p_cback, emvco_stack_data_callback_t *p_data_cback,
    emvco_state_change_callback_t *p_nfc_state_cback) {
  LOG_EMVCOHAL_D("open_emvco_app_data_channel");
  return open_app_data_channel(*p_cback, *p_data_cback, *p_nfc_state_cback);
}

int send_emvco_app_data(uint16_t data_len, const uint8_t *p_data) {
  LOG_EMVCOHAL_D("send_emvco_app_data");
  return send_app_data(data_len, p_data);
}

int close_emvco_app_data_channel(bool isShutdown) {
  LOG_EMVCOHAL_D("close_emvco_app_data_channel");
  return close_app_data_channel(isShutdown);
}

void set_emvco_mode(const int8_t emvco_config, bool_t is_start_emvco) {
  LOG_EMVCOHAL_D("set_emvco_mode");
  handle_set_emvco_mode(emvco_config, is_start_emvco);
}
discovery_mode_t get_current_discovery_mode() {
  LOG_EMVCOHAL_D("get_current_discovery_mode");
  return get_current_mode();
}
void on_nfc_state_change(int32_t nfc_state) {
  LOG_EMVCOHAL_D("on_nfc_state_change");
  handle_nfc_state_change(nfc_state);
}

EMVCO_STATUS stop_rf_discovery(uint8_t deactivation_type) {
  LOG_EMVCOHAL_D("stop_rf_discovery");
  return rf_deactivate(deactivation_type);
}
EMVCO_STATUS set_led(uint8_t led_control) {
  LOG_EMVCOHAL_D("set_led");
  if (EMVCO != modeSwitchArgs->current_discovery_mode) {
    LOG_EMVCOHAL_E("set_led_state failed - Not in EMVCo mode");
    return EMVCO_STATUS_FAILED;
  } else {
    return led_switch_control(led_control);
  }
}

EMVCO_STATUS set_byte_config(config_type_t type, const int32_t length,
                             const int8_t value) {
  char token[MAX_KEY_LENGTH];
  sprintf(token, "%d", (int)type);
  LOG_EMVCOHAL_D("set_byte_config token:%s, value:%x", token, value);

  struct map *m = get_config_map();
  map_set(m, &token, strlen(token) + 1, &value, length);
  return EMVCO_STATUS_SUCCESS;
}

EMVCO_STATUS set_byte_array_config(config_type_t type, const int32_t in_length,
                                   const uint8_t *in_value) {
  LOG_EMVCOHAL_D("set_byte_array_config not supported yet");
  (void)type;
  (void)in_length;
  (void)in_value;
  return EMVCO_STATUS_FEATURE_NOT_SUPPORTED;
}

EMVCO_STATUS set_string_config(config_type_t type, const int32_t in_length,
                               const char *p_value) {
  LOG_EMVCOHAL_D("set_string_config not supported yet");
  (void)type;
  (void)in_length;
  (void)p_value;
  return EMVCO_STATUS_FEATURE_NOT_SUPPORTED;
}