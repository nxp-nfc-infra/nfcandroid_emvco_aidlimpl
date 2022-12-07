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

#include "nci_defs.h"
#include "osal_memory.h"
#include <emvco_dm.h>
#include <emvco_log.h>
#include <emvco_ncif.h>
#include <emvco_util.h>

uint8_t snd_discover_cmd(uint8_t num, tEMVCO_DISCOVER_PARAMS *p_param) {
  tEMVCO_DISCOVER_PARAMS *p;
  uint8_t *pp, *p_start, *p_disc_size;
  int xx;
  int size;
  int len;

  size = num * sizeof(tEMVCO_DISCOVER_PARAMS) + 1;
  p = (tEMVCO_DISCOVER_PARAMS *)osal_malloc(size);
  if (p == NULL)
    return (NCI_STATUS_FAILED);

  pp = (uint8_t *)p;
  p_start = pp;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_DISCOVER);
  p_disc_size = pp;
  pp++;

  UINT8_TO_STREAM(pp, num);
  for (xx = 0; xx < num; xx++) {
    UINT8_TO_STREAM(pp, p_param[xx].type);
    UINT8_TO_STREAM(pp, p_param[xx].frequency);
  }

  *p_disc_size = (uint8_t)((pp - p_start) - NCI_MSG_HDR_SIZE);
  len = NCI_MSG_HDR_SIZE + (*p_disc_size);

  send_app_data(len, p_start);
  free(p);
  return (NCI_STATUS_OK);
}

uint8_t snd_proprietary_act_cmd(uint16_t data_len, const uint8_t *p_data) {
  send_app_data(data_len, p_data);
  return (NCI_STATUS_OK);
}