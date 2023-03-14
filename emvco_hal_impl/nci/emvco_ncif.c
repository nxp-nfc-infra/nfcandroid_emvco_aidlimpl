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

#include "nci_defs.h"
#include "osal_memory.h"
#include <emvco_dm.h>
#include <emvco_log.h>
#include <emvco_nci_ext.h>
#include <emvco_ncif.h>
#include <emvco_util.h>

/* NCI HAL Control structure */
nci_hal_ctrl_t nci_hal_ctrl;
uint8_t *p_nci_data = NULL;

uint8_t send_core_reset(uint8_t reset_type) {
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

uint8_t send_core_init(uint8_t nci_version) {
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

uint8_t send_core_set_config(uint8_t *p_param_tlvs, uint8_t tlv_size) {
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

    if (*pt == 0xA0 || *pt == 0xA1) {
      size -= 3;
      pt += 2;
    } else {
      size -= 2;
      pt++;
    }

    num++;
    ulen = *pt++;
    pt += ulen;
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

uint8_t send_discover_cmd(uint8_t num, tEMVCO_DISCOVER_PARAMS *p_param) {
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

  send_app_data_unlocked(len, p_start);
  free(p);
  return (NCI_STATUS_OK);
}

uint8_t send_proprietary_act_cmd(uint16_t data_len, uint8_t *p_data) {
  send_ext_cmd(data_len, p_data);
  return (NCI_STATUS_OK);
}

void process_emvco_data(uint8_t *p_ntf, uint16_t p_len) {
  LOG_EMVCOHAL_D("%s \n", __func__);
  if (p_len < 1) {
    LOG_EMVCOHAL_E("Not valid Non fragment APDU received length less than 1");
  }

  int apdu_len = (int)p_ntf[2];
  if (p_ntf[0] == PBF_SEGMENT_MSG) {
    SET_CHAINED_DATA();
    memcpy(nci_hal_ctrl.frag_rsp.p_data + nci_hal_ctrl.frag_rsp.data_pos,
           (p_ntf + NCI_HEADER_SIZE), (p_len - NCI_HEADER_SIZE));
    nci_hal_ctrl.frag_rsp.data_pos += (p_len - NCI_HEADER_SIZE);
  } else if (p_ntf[0] == PBF_COMPLETE_MSG) {
    if (IS_CHAINED_DATA()) {
      if (nci_hal_ctrl.frag_rsp.data_pos > 0 &&
          (nci_hal_ctrl.frag_rsp.data_pos + apdu_len) < FRAG_MAX_DATA_LEN) {
        memcpy(nci_hal_ctrl.frag_rsp.p_data + nci_hal_ctrl.frag_rsp.data_pos,
               p_ntf + NCI_HEADER_SIZE, apdu_len);
        nci_hal_ctrl.frag_rsp.data_pos += apdu_len;
        (*nci_hal_ctrl.p_nfc_stack_data_cback)(nci_hal_ctrl.frag_rsp.data_pos,
                                               nci_hal_ctrl.frag_rsp.p_data);
      } else {
        LOG_EMVCOHAL_E("Invalid APDU data length:%d received",
                       nci_hal_ctrl.frag_rsp.data_pos + apdu_len);
      }
      nci_hal_ctrl.frag_rsp.data_pos = 0;
      RESET_CHAINED_DATA();
    } else {
      (*nci_hal_ctrl.p_nfc_stack_data_cback)(apdu_len, nci_hal_ctrl.p_rx_data +
                                                           NCI_HEADER_SIZE);
    }
  } else {
    (*nci_hal_ctrl.p_nfc_stack_data_cback)(nci_hal_ctrl.rx_data_len,
                                           nci_hal_ctrl.p_rx_data);
  }
}

static uint8_t *get_nci_loopback_data(uint8_t pbf, uint8_t *p_data,
                                      int data_len) {
  p_nci_data = osal_malloc((data_len + NCI_HEADER_SIZE) * sizeof(uint8_t));
  p_nci_data[0] = pbf;
  p_nci_data[1] = 0x00;
  p_nci_data[2] = data_len;
  memcpy((p_nci_data + NCI_HEADER_SIZE), p_data, data_len);
  return p_nci_data;
}
static void write_internal(uint8_t *p_data, uint16_t data_len) {
  EMVCO_STATUS status = EMVCO_STATUS_INVALID_PARAMETER;
  nci_hal_ctrl.retry_cnt = 0;
  static uint8_t reset_ntf[] = {0x60, 0x00, 0x06, 0xA0, 0x00,
                                0xC7, 0xD4, 0x00, 0x00};
retry:
  status = tml_write(p_data, data_len);
  if (status != EMVCO_STATUS_SUCCESS) {
    data_len = 0;
    if (nci_hal_ctrl.retry_cnt++ < MAX_RETRY_COUNT) {
      LOG_EMVCOHAL_D(
          "write_unlocked failed - controller Maybe in Standby Mode - Retry");
      /* 10ms delay to give NFCC wake up delay */
      usleep(1000 * 10);
      goto retry;
    } else {
      LOG_EMVCOHAL_E("write_unlocked failed - controller Maybe in Standby Mode "
                     "(max count = "
                     "0x%x)",
                     nci_hal_ctrl.retry_cnt);

      osal_sem_post(&(nci_hal_ctrl.sync_nci_write));

      status = tml_ioctl(ResetDevice);

      if (EMVCO_STATUS_SUCCESS == status) {
        LOG_EMVCOHAL_D("Controller Reset - SUCCESS\n");
      } else {
        LOG_EMVCOHAL_D("Controller Reset - FAILED\n");
      }
      if (nci_hal_ctrl.p_nfc_stack_data_cback != NULL &&
          nci_hal_ctrl.p_rx_data != NULL &&
          nci_hal_ctrl.hal_open_status == true) {
        LOG_EMVCOHAL_D(
            "Send the Core Reset NTF to upper layer, which will trigger the "
            "recovery\n");
        // Send the Core Reset NTF to upper layer, which will trigger the
        // recovery.
        nci_hal_ctrl.rx_data_len = sizeof(reset_ntf);
        osal_memcpy(nci_hal_ctrl.p_rx_data, reset_ntf, sizeof(reset_ntf));
        (*nci_hal_ctrl.p_nfc_stack_data_cback)(nci_hal_ctrl.rx_data_len,
                                               nci_hal_ctrl.p_rx_data);
      }
    }
  }
}
void send_emvco_data(uint8_t *p_data, uint16_t data_len) {
  if ((p_data[0] == PBF_SEGMENT_MSG) || (p_data[0] == PBF_COMPLETE_MSG)) {
    while (data_len > MAX_FRAGMENT_SIZE) {
      LOG_EMVCOHAL_D("%s sending segment packet \n", __func__);
      p_nci_data =
          get_nci_loopback_data(PBF_SEGMENT_MSG, p_data, MAX_FRAGMENT_SIZE);
      write_internal(p_nci_data, MAX_FRAGMENT_SIZE + NCI_HEADER_SIZE);
      if (p_nci_data != NULL) {
        free(p_nci_data);
        p_nci_data = NULL;
      }
      data_len -= MAX_FRAGMENT_SIZE;
      p_data += MAX_FRAGMENT_SIZE;
    }
    if (data_len > 0) {
      p_nci_data = get_nci_loopback_data(PBF_COMPLETE_MSG, p_data, data_len);
      write_internal(p_nci_data, data_len + NCI_HEADER_SIZE);
      if (p_nci_data != NULL) {
        free(p_nci_data);
        p_nci_data = NULL;
      }
    }
  } else {
    write_internal(p_data, data_len);
  }
}

uint8_t send_deactivate_cmd(uint8_t de_act_type) {
  uint8_t *pp, *p;
  int len = NCI_MSG_HDR_SIZE + NCI_DISCOVER_PARAM_SIZE_DEACT;
  p = (uint8_t *)osal_malloc(len);
  if (p == NULL)
    return (NCI_STATUS_FAILED);
  pp = p;

  NCI_MSG_BLD_HDR0(pp, MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_DEACTIVATE);
  UINT8_TO_STREAM(pp, NCI_DISCOVER_PARAM_SIZE_DEACT);
  UINT8_TO_STREAM(pp, de_act_type);

  send_app_data_unlocked(len, p);
  free(p);
  return (NCI_STATUS_OK);
}