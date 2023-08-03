/******************************************************************************
 *
 *  Copyright 2023 NXP
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

#ifndef _EMVCO_TDA_H_
#define _EMVCO_TDA_H_

#include "emvco_status.h"
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tda_data defines the data buffer and length
 * can be send and received with controller
 */
typedef struct tda_data {
  uint32_t len;    /*!< length of the buffer */
  uint8_t *p_data; /*!< pointer to a buffer */
} tda_data;

/**
 * @brief tda_type provides the type of the smart
 * card connected over TDA
 */
typedef enum tda_type { CONTACT_CARD = 0, SAM, UN_KNOWN } tda_type_t;

/**
 * @brief card_tlv_info provides details of
 * smart card connected over TDA in TLV format
 */
typedef struct card_tlv_info {
  uint8_t type;
  uint8_t length;
  void *value;
} card_tlv_info_t;

typedef enum protocols {
  APDU = 0,
  RFU_01,
  T3T,
  TRANPARENT,
  RFU_04_7F,
  PROPRIETARY,
  RFU_FF
} protocols_t;

/**
 * @brief tda_status provides the status of the smart
 * card connected over TDA
 */
typedef enum tda_status {
  ENABLED = 0,
  DISABLED,
  UNRESPONSIVE,
  CC_STATUS_RFU
} tda_status_t;

/**
 * @brief tda provides all the details of
 * smart card connected over TDA
 */
struct tda {
  int8_t id;
  tda_status_t status;
  uint8_t number_of_protocols;
  protocols_t *protocols_t;
  uint8_t number_of_card_info;
  card_tlv_info_t *card_tlv_info;
};

/**
 * @brief tda_control has information to handle the TDA functionality
 */
typedef struct tda_control {
  struct tda *p_tda;
  int8_t curr_tda;
  int8_t num_tda_supported;
  uint8_t tda_state;
  int8_t last_tda_id_of_channel;
  sem_t sync_tda_write;
  pthread_mutex_t tda_data_lock;
  uint16_t ret_status;
} tda_control_t;

typedef struct mode_set_control {
  int8_t tda_id;
  int8_t mode;
  int8_t flag;
  int8_t curr_index;
} mode_set_control_t;

typedef struct tda tda;
typedef enum tda_type tda_type;

enum { TDA_STATE_INIT = 0, TDA_STATE_DISOVERY_DONE, TDA_STATE_MODE_SET_DONE };

typedef struct tda_channel {
  int8_t tda_id;
  int8_t channel_num;
  struct tda_channel *next;
} tda_channel_t;

typedef struct {
  int8_t tda_id;
  int8_t channel_num;
} tda_channel_pair_t;

typedef struct {
  tda_data *cmd_apdu;
  tda_data *rsp_apdu;
} transceive_buffer_t;

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
#endif /* _EMVCO_TDA_H_ */
