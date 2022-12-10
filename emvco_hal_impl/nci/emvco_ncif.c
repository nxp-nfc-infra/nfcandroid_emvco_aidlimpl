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
#include <emvco_nci_ext.h>
#include <emvco_ncif.h>
#include <emvco_util.h>

uint8_t snd_core_reset(uint8_t reset_type) {
  uint8_t *pp, *p;
  int len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_RESET;
  p = (uint8_t *)osal_malloc(len);
  if (p == NULL)
    return (NCI_STATUS_FAILED);
  pp = p;

  NCI_MSG_BLD_HDR0(pp, MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, MSG_CORE_RESET);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_RESET);
  UINT8_TO_STREAM(pp, reset_type);
  send_ext_cmd(len, p);
  free(p);
  return (NCI_STATUS_OK);
}

uint8_t snd_core_init(uint8_t nci_version) {
  uint8_t *pp, *p;
  int len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_INIT(nci_version);
  p = (uint8_t *)osal_malloc(len);
  if (p == NULL)
    return (NCI_STATUS_FAILED);
  pp = p;

  NCI_MSG_BLD_HDR0(pp, MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, MSG_CORE_INIT);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_INIT(nci_version));
  if (nci_version == NCI_VERSION_2_0) {
    UINT8_TO_STREAM(pp, NCI2_0_CORE_INIT_CMD_BYTE_0);
    UINT8_TO_STREAM(pp, NCI2_0_CORE_INIT_CMD_BYTE_1);
  }
  send_ext_cmd(len, p);
  free(p);
  return (NCI_STATUS_OK);
}

uint8_t snd_core_set_config(uint8_t *p_param_tlvs, uint8_t tlv_size) {
  uint8_t *p;
  uint8_t *pp;
  uint8_t num = 0, ulen, size, *pt;
  int len = NCI_MSG_HDR_SIZE + tlv_size + 1;

  p = (uint8_t *)osal_malloc(len);
  if (p == NULL)
    return (NCI_STATUS_FAILED);

  pp = (uint8_t *)(p);

  NCI_MSG_BLD_HDR0(pp, MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, MSG_CORE_SET_CONFIG);
  UINT8_TO_STREAM(pp, (uint8_t)(tlv_size + 1));
  size = tlv_size;
  pt = p_param_tlvs;
  while (size > 1) {
    size -= 3;
    pt += 2;
    num++;
    ulen = *pt;
    pt += (ulen + 1);
    if (size >= ulen) {
      size -= ulen;
    } else {
      free(p);
      return NCI_STATUS_FAILED;
    }
  }

  UINT8_TO_STREAM(pp, num);
  ARRAY_TO_STREAM(pp, p_param_tlvs, tlv_size);

  send_ext_cmd(len, p);
  free(p);
  return (NCI_STATUS_OK);
}

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

  NCI_MSG_BLD_HDR0(pp, MT_CMD, NCI_GID_RF_MANAGE);
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

uint8_t snd_proprietary_act_cmd(uint16_t data_len, uint8_t *p_data) {
  send_ext_cmd(data_len, p_data);
  return (NCI_STATUS_OK);
}