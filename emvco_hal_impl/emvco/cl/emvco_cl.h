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

#include <emvco_common.h>
#include <emvco_tml.h>

bool_t is_set_emvco_mode;
bool_t is_stop_emvco_mode;

EMVCO_STATUS start_emvco_mode(const int8_t emvco_config);
EMVCO_STATUS stop_emvco_mode();
EMVCO_STATUS process_emvco_mode_rsp(osal_transact_info_t *);

#endif /* _EMVCO_CL_H_ */
