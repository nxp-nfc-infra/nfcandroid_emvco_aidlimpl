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

#include <emvco_dm.h>
#include <emvco_tml.h>
#include <string.h>

void nci_ext_init(void);
EMVCO_STATUS process_ext_rsp(uint8_t *p_ntf, uint16_t *p_len);
EMVCO_STATUS send_ext_cmd(uint16_t cmd_len, uint8_t *p_cmd);
EMVCO_STATUS send_app_data_ext(uint16_t *cmd_len, uint8_t *p_cmd_data,
                               uint16_t *rsp_len, uint8_t *p_rsp_data);
#endif /* _PHNXPNICHAL_EXT_H_ */
