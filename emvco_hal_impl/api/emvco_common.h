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

#ifndef _EMVCO_COMMON_H_
#define _EMVCO_COMMON_H_

#include "emvco_status.h"
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  boolean data type
 */
typedef uint8_t bool_t;

/**
 * @brief  EMVCo status specifies EMVCO_STATUS_SUCCESS or EMVCO_STATUS_FAILED
 */
typedef uint16_t EMVCO_STATUS; /* Return values */

/**
 * @brief emvco_event_t EMVCo data event
 *
 */
typedef uint8_t emvco_event_t;
/**
 * @brief emvco_event_t EMVCo statuc event
 *
 */
typedef uint8_t emvco_status_t;
/**
 * @brief discovery_mode_t stores the current active profile. NFC/EMVCO/UNKNOWN
 *
 */
typedef enum { NFC = 1, EMVCO, UNKNOWN } discovery_mode_t;

discovery_mode_t discovery_mode;
/**
 * @brief config_type_t defines the supported config key types
 *        can be configured through setConfig API
 *
 */
typedef enum { POLL_PROFILE_SEL = 0 } config_type_t;
/**
 * @brief deactivation_type_t defines the supported deactivation
 *        types which will be used along with deactivation command.
 *
 */
typedef enum { IDLE = 0, DISCOVER = 3 } deactivation_type_t;

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
  uint8_t id;
  tda_status_t status;
  uint8_t number_of_protocols;
  protocols_t *protocols_t;
  uint8_t number_of_card_info;
  card_tlv_info_t *card_tlv_info;
};

typedef struct tda_control {
  struct tda *p_tda;
  uint8_t curr_tda;
  uint8_t num_tda_supported;
  uint8_t tda_state;
  uint8_t last_tda_id_of_channel;
  pthread_mutex_t tda_data_lock;
} tda_control_t;

typedef struct mode_set_control {
  uint8_t id;
  uint8_t flag;
  uint8_t curr_index;
} mode_set_control_t;

typedef struct tda tda;
typedef enum tda_type tda_type;

enum { TDA_STATE_INIT = 0, TDA_STATE_DISOVERY_DONE, TDA_STATE_MODE_SET_DONE };

typedef struct tda_channel {
  uint8_t tda_id;
  uint8_t channel_num;
  struct tda_channel *next;
} tda_channel_t;

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
#endif /* _EMVCO_COMMON_H_ */
