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
#ifndef _PHNXPNCIHAL_EXT_H_
#define _PHNXPNCIHAL_EXT_H_

#include <phNxpNciHal.h>
#include <phTmlNfc.h>
#include <string.h>

bool_t issetEMVCoMode;
bool_t isStopEMVCoMode;

void phNxpNciHal_ext_init(void);
NFCSTATUS phNxpNciHal_process_ext_rsp(uint8_t *p_ntf, uint16_t *p_len);
NFCSTATUS phNxpNciHal_send_ext_cmd(uint16_t cmd_len, uint8_t *p_cmd);
NFCSTATUS phNxpNciHal_send_ese_hal_cmd(uint16_t cmd_len, uint8_t *p_cmd);
NFCSTATUS phNxpNciHal_write_ext(uint16_t *cmd_len, uint8_t *p_cmd_data,
                                uint16_t *rsp_len, uint8_t *p_rsp_data);
NFCSTATUS phNxpNciHal_start_emvco_mode(const int8_t emvco_config);
NFCSTATUS phNxpNciHal_stop_emvco_mode();
NFCSTATUS phNxpNciHal_process_emvco_mode_rsp(phTmlNfc_TransactInfo_t *);
NFCSTATUS phNxpNciHal_send_emvco_mode_cmd(uint16_t cmd_len, uint8_t *p_cmd);
void phNxpNciHal_ext_handleNfcStateChanged(int32_t nfc_state);
#endif /* _PHNXPNICHAL_EXT_H_ */
