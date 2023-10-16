/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of NXP nor the names of its contributors may be used
 * to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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