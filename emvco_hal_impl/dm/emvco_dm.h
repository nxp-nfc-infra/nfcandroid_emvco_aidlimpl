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
#ifndef _EMVCO_DM_H_
#define _EMVCO_DM_H_

#include <emvco_hal.h>
#include <emvco_util.h>
#include <pthread.h>
/********************* Definitions and structures *****************************/
#define MAX_INIT_RETRY_COUNT 3
#define MAX_RETRY_COUNT 5
#define NCI_MAX_DATA_LEN 300
#define NCI_POLL_DURATION 500
#define HAL_EMVCO_ENABLE_I2C_FRAGMENTATION_EVT 0x07
#undef P2P_PRIO_LOGIC_HAL_IMP
#define NCI_VERSION_2_0 0x20
#define NCI_VERSION_1_1 0x11
#define NCI_VERSION_1_0 0x10
#define NCI_VERSION_UNKNOWN 0x00
typedef void(control_granted_callback_t)();

/*ROM CODE VERSION FW*/
#define FW_MOBILE_ROM_VERSION_PN548AD 0x10
#define FW_MOBILE_ROM_VERSION_PN551 0x10
#define FW_MOBILE_ROM_VERSION_PN553 0x11
#define FW_MOBILE_ROM_VERSION_PN557 0x12
/* NCI Data */

#define NCI_MT_CMD 0x20
#define NCI_MT_RSP 0x40
#define NCI_MT_NTF 0x60

#define CORE_RESET_TRIGGER_TYPE_CORE_RESET_CMD_RECEIVED 0x02
#define CORE_RESET_TRIGGER_TYPE_POWERED_ON 0x01
#define CORE_RESET_NTF_MODE_SWITCH_TO_NFC_FORUM 0xA8
#define CORE_RESET_NTF_MODE_SWITCH_TO_EMVCO 0xA9

#define NCI_MSG_CORE_RESET 0x00
#define NCI_MSG_CORE_INIT 0x01
#define NCI_MT_MASK 0xE0
#define NCI_OID_MASK 0x3F

#define NXP_MAX_CONFIG_STRING_LEN 260
#define NCI_HEADER_SIZE 3

#define NFC_A_PASSIVE_POLL_MODE 1
#define NFC_B_PASSIVE_POLL_MODE 2
#define NFC_AB_PASSIVE_POLL_MODE 3
#define NFC_F_PASSIVE_POLL_MODE 4
#define NFC_AF_PASSIVE_POLL_MODE 5
#define NFC_BF_PASSIVE_POLL_MODE 6
#define NFC_ABF_PASSIVE_POLL_MODE 7
#define NFC_VAS_PASSIVE_POLL_MODE 8
#define NFC_AVAS_PASSIVE_POLL_MODE 9
#define NFC_BVAS_PASSIVE_POLL_MODE 10
#define NFC_ABVAS_PASSIVE_POLL_MODE 11
#define NFC_FVAS_PASSIVE_POLL_MODE 12
#define NFC_AFVAS_PASSIVE_POLL_MODE 13
#define NFC_BFVAS_PASSIVE_POLL_MODE 14
#define NFC_ABFVAS_PASSIVE_POLL_MODE 15

sem_t nfc_status_semaphore;

enum {
  EMVCO_OPEN_CHNL_CPLT_EVT = 0u,
  EMVCO_OPEN_CHNL_ERROR_EVT = 1u,
  EMVCO_CLOSE_CHNL_CPLT_EVT = 2u,
  EMVCO_POOLING_START_EVT = 3u,
  EMVCO_POLLING_STARTED_EVT = 4u,
  EMVCO_POLLING_STOP_EVT = 5u,
  EMVCO_UN_SUPPORTED_CARD_EVT = 6u,
};

enum {
  STATUS_OK = 0u,
  STATUS_FAILED = 1u,
};
typedef enum {
  STATE_OFF = 1,
  STATE_TURNING_ON,
  STATE_ON,
  STATE_TURNING_OFF
} nfc_status_t;

nfc_status_t nfc_status;

typedef enum { EMVCO_MODE_ON = 1, EMVCO_MODE_OFF } emvco_mode_status_t;

emvco_mode_status_t emvco_status;

emvco_stack_callback_t *m_p_nfc_stack_cback;
emvco_stack_data_callback_t *m_p_nfc_stack_data_cback;
emvco_state_change_callback_t *m_p_nfc_state_cback;

typedef struct nci_data {
  uint16_t len;
  uint8_t p_data[NCI_MAX_DATA_LEN];
} nci_data_t;

typedef enum {
  HAL_STATUS_CLOSE = 0,
  HAL_STATUS_OPEN,
  HAL_STATUS_MIN_OPEN
} emvco_hal_status;

typedef enum {
  GPIO_UNKNOWN = 0x00,
  GPIO_STORE = 0x01,
  GPIO_STORE_DONE = 0x02,
  GPIO_RESTORE = 0x10,
  GPIO_RESTORE_DONE = 0x20,
  GPIO_CLEAR = 0xFF
} gpio_info_state;

typedef struct gpio_info {
  gpio_info_state state;
  uint8_t values[2];
} gpio_info_t;

/* Macros to enable and disable extensions */
#define HAL_ENABLE_EXT() (nci_hal_ctrl.hal_ext_enabled = 1)
#define HAL_DISABLE_EXT() (nci_hal_ctrl.hal_ext_enabled = 0)
typedef struct nci_info {
  uint8_t nci_version;
  bool_t wait_for_ntf;
} nci_info_t;
/* NCI Control structure */
typedef struct nci_hal_ctrl {
  emvco_hal_status halStatus;  /* Indicate if hal is open or closed */
  pthread_t emvco_hal_client_thread; /* Integration thread handle */
  uint8_t thread_running;      /* Thread running if set to 1, else set to 0 */
  driver_config_data gDrvCfg;  /* Driver config data */

  /* Rx data */
  uint8_t *p_rx_data;
  uint16_t rx_data_len;

  /* Rx data */
  uint8_t *p_rx_ese_data;
  uint16_t rx_ese_data_len;

  /* libnfc-nci callbacks */
  emvco_stack_callback_t *p_nfc_stack_cback;
  emvco_stack_data_callback_t *p_nfc_stack_data_cback;
  emvco_state_change_callback_t *p_nfc_state_cback;

  /* control granted callback */
  control_granted_callback_t *p_control_granted_cback;

  /* HAL open status */
  bool_t hal_open_status;

  /* HAL extensions */
  uint8_t hal_ext_enabled;

  /* Waiting semaphore */
  nci_hal_sem ext_cb_data;
  sem_t sync_spi_nfc;

  uint16_t cmd_len;
  uint8_t p_cmd_data[NCI_MAX_DATA_LEN];
  uint16_t rsp_len;
  uint8_t p_rsp_data[NCI_MAX_DATA_LEN];

  /* retry count used to force download */
  uint16_t retry_cnt;
  uint8_t read_retry_cnt;
  nci_info_t nci_info;

  /* to store and restore gpio values */
  gpio_info_t gpio_info;
} nci_hal_ctrl_t;

typedef struct nci_clock {
  bool_t is_clock_set;
  uint8_t p_rx_data[20];
  bool_t is_set_config;
} nci_clock_t;

typedef struct rf_setting {
  bool_t is_get_config;
  uint8_t p_rx_data[20];
} rf_setting_t;

typedef struct eeprom_area {
  bool_t is_get_eeprom_area;
  uint8_t p_rx_data[32];
} eeprom_area_t;

typedef enum {
  NFC_FORUM_PROFILE,
  EMV_CO_PROFILE,
  INVALID_PROFILe
} nci_profile_t;
/* NXP Poll Profile control structure */
typedef struct nci_profile_Control {
  nci_profile_t profile_type;
  uint8_t
      b_clk_src_val; /* Holds the System clock source read from config file */
  uint8_t b_clk_freq_val; /* Holds the System clock frequency read from config
                             file */
  uint8_t b_timeout;      /* Holds the Timeout Value */
} nci_profile_Control_t;

/* Internal messages to handle callbacks */
#define EMVCO_OPEN_CHNL_CPLT_MSG 0x411
#define EMVCO_OPEN_CHNL_ERROR_MSG 0x412
#define EMVCO_CLOSE_CHNL_CPLT_MSG 0x413
#define EMVCO_POOLING_STARTING_MSG 0x414
#define EMVCO_POOLING_START_FAILED_MSG 0x415
#define EMVCO_POLLING_STARTED_MSG 0x416

#define EMVCO_DATA_RX_EVT 0xF01

#define NCIHAL_CMD_CODE_LEN_BYTE_OFFSET (2U)
#define NCIHAL_CMD_CODE_BYTE_LEN (3U)

/******************** NCI HAL exposed functions *******************************/

int open_app_data_channel(emvco_stack_callback_t *p_cback,
                          emvco_stack_data_callback_t *p_data_cback,
                          emvco_state_change_callback_t *p_nfc_state_cback);
int close_app_data_channel(bool);
int send_app_data_internal(uint16_t data_len, const uint8_t *p_data);
int send_app_data(uint16_t data_len, const uint8_t *p_data);

int send_app_data_unlocked(uint16_t data_len, const uint8_t *p_data);
EMVCO_STATUS core_reset_recovery();

void led_switch_control(emvco_status_t emvco_status);
void handle_set_emvco_mode(const int8_t emvco_config, bool_t in_isStartEMVCo);
void handle_nfc_state_change(int32_t nfc_state);

#endif /* _EMVCO_DM_H_ */
