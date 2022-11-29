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

#include <emvco_common.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The callback passed in from the NFC stack that the HAL
 * can use to pass events back to the stack.
 */
typedef void(emvco_stack_callback_t)(emvco_event_t event,
                                     emvco_status_t event_status);

/*
 * The callback passed in from the NFC stack that the HAL
 * can use to pass incomming data to the stack.
 */
typedef void(emvco_stack_data_callback_t)(uint16_t data_len, uint8_t *p_data);

typedef void(emvco_state_change_callback_t)(bool enableNfc);

int open_emvco_app_data_channel(
    emvco_stack_callback_t *p_cback, emvco_stack_data_callback_t *p_data_cback,
    emvco_state_change_callback_t *p_nfc_state_cback);

int send_emvco_app_data(uint16_t data_len, const uint8_t *p_data);

int close_emvco_app_data_channel(bool);

void set_emvco_mode(const int8_t emvco_config, bool_t in_isStartEMVCo);

void on_nfc_state_change(int32_t nfc_state);

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
#endif /* _EMVCO_HAL_H_ */
