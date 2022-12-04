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

#include <cutils/properties.h>
#include <emvco_cl.h>
#include <emvco_config.h>
#include <emvco_dm.h>
#include <emvco_log.h>
#include <emvco_nci_ext.h>
#include <emvco_tml.h>
#include <errno.h>
#include <log/log.h>
#include <nci_parser.h>
#include <osal_memory.h>
#include <osal_message_queue_lib.h>
#include <osal_thread.h>
#include <peripherals.h>
#include <stdlib.h>
#include <sys/stat.h>

/*********************** Global Variables *************************************/
#define PN547C2_CLOCK_SETTING
#define CORE_RES_STATUS_BYTE 3
/* Processing of ISO 15693 EOF */
extern uint8_t icode_send_eof;
extern uint8_t icode_detected;
static uint8_t cmd_icode_eof[] = {0x00, 0x00, 0x00};

static uint8_t config_access = false;
static uint8_t config_success = true;

extern nci_clock_t nci_clock;
extern emvco_args_t *modeSwitchArgs;

/* NCI HAL Control structure */
nci_hal_ctrl_t nci_hal_ctrl;

/* NXP Poll Profile structure */
nci_profile_Control_t nxpprofile_ctrl;

/* TML Context */
extern tml_emvco_context_t *gptml_emvco_context;
uint32_t wFwVerRsp;

extern uint16_t wFwVer;
extern uint16_t rom_version;

uint32_t timeoutTimerId = 0;
bool emvco_debug_enabled = true;

/*  Used to send Callback Transceive data during Mifare Write.
 *  If this flag is enabled, no need to send response to Upper layer */
bool sendRspToUpperLayer = true;

nci_hal_sem config_data;

nci_clock_t nci_clock = {0, {0}, false};

rf_setting_t phNxpNciRfSet = {false, {0}};

eeprom_area_t eeprom_area = {false, {0}};

extern pthread_cond_t nfcStatusCondVar;
extern pthread_mutex_t nfcStatusSyncLock;

/**************** local methods used in this file only ************************/
static void open_app_data_channel_complete(EMVCO_STATUS status);
static int min_open_app_data_channel();
static int handle_power_cycle(void);
int min_close_app_data_channel(void);
static void min_open_app_data_channel_complete(EMVCO_STATUS status);
static int check_ncicmd_write_window(uint16_t cmd_len, uint8_t *p_cmd);
static void send_app_data_complete(void *p_context,
                                   osal_transact_info_t *pInfo);
static void read_app_data_complete(void *p_context,
                                   osal_transact_info_t *pInfo);
static void close_app_data_channel_complete(EMVCO_STATUS status);
static void power_cycle_complete(EMVCO_STATUS status);
static void kill_emvco_hal_client_thread(nci_hal_ctrl_t *p_nxpncihal_ctrl);
static void *emvco_hal_client_thread(void *arg);
static void print_res_status(uint8_t *p_rx_data, uint16_t *p_len);

static void initialize_debug_enabled_flag();

/******************************************************************************
 * Function         initialize_debug_enabled_flag
 *
 * Description      This function gets the value for emvco_debug_enabled
 *
 * Returns          void
 *
 ******************************************************************************/
static void initialize_debug_enabled_flag() {
  unsigned long num = 0;
  if (get_num_value(NAME_EMVCO_DEBUG_ENABLED, &num, sizeof(num))) {
    emvco_debug_enabled = (num == 0) ? false : true;
  }
  LOG_EMVCOHAL_D("emvco_debug_enabled : %d", emvco_debug_enabled);
}

/******************************************************************************
 * Function         emvco_hal_client_thread
 *
 * Description      This function is a thread handler which handles all TML and
 *                  NCI messages.
 *
 * Returns          void
 *
 ******************************************************************************/
static void *emvco_hal_client_thread(void *arg) {
  nci_hal_ctrl_t *p_nci_hal_ctrl = (nci_hal_ctrl_t *)arg;
  lib_emvco_message_t msg;

  LOG_EMVCOHAL_D("thread started");

  p_nci_hal_ctrl->thread_running = 1;

  while (p_nci_hal_ctrl->thread_running == 1) {
    /* Fetch next message from the NFC stack message queue */
    if (osal_msg_rcv(p_nci_hal_ctrl->gDrvCfg.n_client_id, &msg, 0, 0) == -1) {
      LOG_EMVCOHAL_E("NFC client received bad message");
      continue;
    }

    if (p_nci_hal_ctrl->thread_running == 0) {
      break;
    }

    switch (msg.e_msgType) {
    case EMVCO_DEFERRED_CALL_MSG: {
      REENTRANCE_LOCK();
      lib_emvco_deferred_call_t *deferCall =
          (lib_emvco_deferred_call_t *)(msg.p_msg_data);

      deferCall->p_callback(deferCall->p_parameter);

      osal_transact_info_t *pTransactionInfo =
          (osal_transact_info_t *)deferCall->p_parameter;
      process_emvco_mode_rsp(pTransactionInfo);
      REENTRANCE_UNLOCK();

      break;
    }

    case EMVCO_OPEN_CHNL_CPLT_MSG: {
      REENTRANCE_LOCK();
      if (nci_hal_ctrl.p_nfc_stack_cback != NULL) {
        /* Send the event */
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_OPEN_CHNL_CPLT_EVT, STATUS_OK);
      }
      REENTRANCE_UNLOCK();
      break;
    }

    case EMVCO_CLOSE_CHNL_CPLT_MSG: {
      REENTRANCE_LOCK();
      modeSwitchArgs->is_start_emvco = false;
      if (nci_hal_ctrl.p_nfc_stack_cback != NULL) {
        /* Send the event */
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_POLLING_STOP_EVT, STATUS_OK);
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_CLOSE_CHNL_CPLT_EVT, STATUS_OK);
      }
      kill_emvco_hal_client_thread(&nci_hal_ctrl);
      REENTRANCE_UNLOCK();
      break;
    }

    case EMVCO_OPEN_CHNL_ERROR_MSG: {
      REENTRANCE_LOCK();
      if (nci_hal_ctrl.p_nfc_stack_cback != NULL) {
        /* Send the event */
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_OPEN_CHNL_ERROR_EVT,
                                          STATUS_FAILED);
      }
      REENTRANCE_UNLOCK();
      break;
    }

    case EMVCO_DATA_RX_EVT: {
      REENTRANCE_LOCK();
      if (nci_hal_ctrl.p_nfc_stack_data_cback != NULL) {
        (*nci_hal_ctrl.p_nfc_stack_data_cback)(nci_hal_ctrl.rsp_len,
                                               nci_hal_ctrl.p_rsp_data);
      }
      REENTRANCE_UNLOCK();
      break;
    }
    case EMVCO_POLLING_STARTED_MSG: {
      REENTRANCE_LOCK();
      if (nci_hal_ctrl.p_nfc_stack_cback != NULL) {
        /* Send the event */
        LOG_EMVCOHAL_E("EMVCO_POLLING_STARTED_EVT propagted to upper layer");
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_POLLING_STARTED_EVT, STATUS_OK);
      }
      REENTRANCE_UNLOCK();
      break;
    }
    case EMVCO_POOLING_STARTING_MSG: {
      REENTRANCE_LOCK();
      if (nci_hal_ctrl.p_nfc_stack_cback != NULL) {
        /* Send the event */
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_POOLING_START_EVT, STATUS_OK);
      }
      REENTRANCE_UNLOCK();
      break;
    }
    case EMVCO_POOLING_START_FAILED_MSG: {
      REENTRANCE_LOCK();
      if (nci_hal_ctrl.p_nfc_stack_cback != NULL) {
        /* Send the event */
        (*nci_hal_ctrl.p_nfc_stack_cback)(EMVCO_POOLING_START_EVT,
                                          STATUS_FAILED);
      }
      REENTRANCE_UNLOCK();
      break;
    }
    }
  }

  return NULL;
}

/******************************************************************************
 * Function         kill_emvco_hal_client_thread
 *
 * Description      This function safely kill the client thread and clean all
 *                  resources.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void kill_emvco_hal_client_thread(nci_hal_ctrl_t *p_nci_hal_ctrl) {
  LOG_EMVCOHAL_D("Terminating phNxpNciHal client thread...");
  p_nci_hal_ctrl->p_nfc_stack_cback = NULL;
  p_nci_hal_ctrl->p_nfc_stack_data_cback = NULL;
  p_nci_hal_ctrl->p_nfc_state_cback = NULL;
  p_nci_hal_ctrl->thread_running = 0;

  return;
}

int open_app_data_channelImpl(
    emvco_stack_callback_t *p_cback, emvco_stack_data_callback_t *p_data_cback,
    emvco_state_change_callback_t *p_nfc_state_cback) {
  EMVCO_STATUS wConfigStatus = EMVCO_STATUS_SUCCESS;
  EMVCO_STATUS status = EMVCO_STATUS_SUCCESS;
  LOG_EMVCOHAL_D("open_app_data_channel nfc_status:%d", nfc_status);

  if (nfc_status != STATE_OFF) {
    if (0 != osal_sem_init(&nfc_status_semaphore, 0, 1)) {
      LOG_EMVCOHAL_D("osal_sem_init() Failed, errno = 0x%02X", errno);
    }
    LOG_EMVCOHAL_D("Disable NFC");
    p_nfc_state_cback(false);

    pthread_mutex_lock(&nfcStatusSyncLock);
    pthread_cond_wait(&nfcStatusCondVar, &nfcStatusSyncLock);
    pthread_mutex_unlock(&nfcStatusSyncLock);
  }

  if (nci_hal_ctrl.halStatus == HAL_STATUS_OPEN) {
    LOG_EMVCOHAL_D("open_app_data_channel already open");
    return EMVCO_STATUS_SUCCESS;
  } else if (nci_hal_ctrl.halStatus == HAL_STATUS_CLOSE) {
    status = min_open_app_data_channel();
    if (status != EMVCO_STATUS_SUCCESS) {
      LOG_EMVCOHAL_E("min_open_app_data_channel failed");
      goto clean_and_return;
    }
  }
  nci_hal_ctrl.p_nfc_stack_cback = p_cback;
  nci_hal_ctrl.p_nfc_stack_data_cback = p_data_cback;
  nci_hal_ctrl.p_nfc_state_cback = p_nfc_state_cback;
  open_app_data_channel_complete(wConfigStatus);

  return wConfigStatus;

clean_and_return:
  CONCURRENCY_UNLOCK();
  if (p_cback != NULL) {
    (*p_cback)(EMVCO_OPEN_CHNL_CPLT_EVT, STATUS_FAILED);
  }

  nci_hal_ctrl.p_nfc_stack_cback = NULL;
  nci_hal_ctrl.p_nfc_stack_data_cback = NULL;
  cleanup_monitor();
  nci_hal_ctrl.halStatus = HAL_STATUS_CLOSE;
  return EMVCO_STATUS_FAILED;
}

/******************************************************************************
 * Function         min_open_app_data_channel
 *
 * Description      This function initializes the least required resources to
 *                  communicate to NFCC.This is mainly used to communicate to
 *                  NFCC when NFC service is not available.
 *
 *
 * Returns          This function return EMVCO_STATUS_SUCCESS (0) in case of
 *                  success. In case of failure returns other failure value.
 *
 ******************************************************************************/
int min_open_app_data_channel() {
  osal_emvco_config_t tOsalConfig;
  tml_emvco_Config_t tTmlConfig;
  char *nfc_dev_node = NULL;
  const uint16_t max_len = 260;
  uint32_t mode_switch_status;
  EMVCO_STATUS wConfigStatus = EMVCO_STATUS_SUCCESS;
  EMVCO_STATUS status = EMVCO_STATUS_SUCCESS;
  LOG_EMVCOHAL_D("phNxpNci_MinOpen(): enter");
  /*NCI_INIT_CMD*/
  static uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x00};
  /*NCI2_0_INIT_CMD*/
  static uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
  /*SET_CONFIG_IDLE_POWER_OFF_CMD*/
  static uint8_t cmd_set_config_deactivate_idle_power_off[] = {
      0x20, 0x02, 0x05, 0x01, 0xA0, 0x44, 0x01, 0x02};

  if (nci_hal_ctrl.halStatus == HAL_STATUS_MIN_OPEN) {
    LOG_EMVCOHAL_D("min_open_app_data_channel(): already open");
    return EMVCO_STATUS_SUCCESS;
  }

  int init_retry_cnt = 0;
  int8_t ret_val = 0x00;

  initialize_debug_enabled_flag();
  /* initialize trace level */
  initialize_log_level();

  /*Create the timer for extns write response*/
  timeoutTimerId = osal_timer_create();

  if (init_monitor() == NULL) {
    LOG_EMVCOHAL_E("Init monitor failed");
    return EMVCO_STATUS_FAILED;
  }

  CONCURRENCY_LOCK();
  osal_memset(&nci_hal_ctrl, 0x00, sizeof(nci_hal_ctrl));
  osal_memset(&tOsalConfig, 0x00, sizeof(tOsalConfig));
  osal_memset(&tTmlConfig, 0x00, sizeof(tTmlConfig));
  osal_memset(&nxpprofile_ctrl, 0, sizeof(nci_profile_Control_t));

  /*Init binary semaphore for Spi Nfc synchronization*/
  if (0 != osal_sem_init(&nci_hal_ctrl.sync_spi_nfc, 0, 1)) {
    LOG_EMVCOHAL_E("osal_sem_init() FAiled, errno = 0x%02X", errno);
    goto clean_and_return;
  }

  /* By default HAL status is HAL_STATUS_OPEN */
  nci_hal_ctrl.halStatus = HAL_STATUS_OPEN;

  /*nci version NCI_VERSION_UNKNOWN version by default*/
  nci_hal_ctrl.nci_info.nci_version = NCI_VERSION_UNKNOWN;
  /* Read the nfc device node name */
  nfc_dev_node = (char *)malloc(max_len * sizeof(char));
  if (nfc_dev_node == NULL) {
    LOG_EMVCOHAL_D("malloc of nfc_dev_node failed ");
    goto clean_and_return;
  } else if (!get_str_value(NAME_CFG_NXP_NFC_DEV_NODE, nfc_dev_node, max_len)) {
    LOG_EMVCOHAL_D(
        "Invalid nfc device node name keeping the default device node "
        "/dev/nxpnfc");
    strlcpy(nfc_dev_node, "/dev/nxpnfc", (max_len * sizeof(char)));
  }

  /* Configure hardware link */
  nci_hal_ctrl.gDrvCfg.n_client_id = osal_msg_get(0, 0600);
  nci_hal_ctrl.gDrvCfg.n_link_type = ENUM_LINK_TYPE_I2C;
  tTmlConfig.p_dev_name = (int8_t *)nfc_dev_node;
  tOsalConfig.dw_callback_thread_id =
      (uintptr_t)nci_hal_ctrl.gDrvCfg.n_client_id;
  tOsalConfig.p_log_file = NULL;
  tTmlConfig.dw_get_msg_thread_id = (uintptr_t)nci_hal_ctrl.gDrvCfg.n_client_id;

  /* Initialize TML layer */
  wConfigStatus = tml_init(&tTmlConfig);
  if (wConfigStatus != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_E("tml_init Failed");
    goto clean_and_return;
  } else {
    if (nfc_dev_node != NULL) {
      osal_free(nfc_dev_node);
      nfc_dev_node = NULL;
    }
  }

  /* Create the client thread */
  ret_val = osal_thread_create(&nci_hal_ctrl.emvco_hal_client_thread, NULL,
                               emvco_hal_client_thread, &nci_hal_ctrl);
  if (ret_val != 0) {
    LOG_EMVCOHAL_E("osal_thread_create failed");
    wConfigStatus = tml_shutdown_cleanup();
    goto clean_and_return;
  }

  CONCURRENCY_UNLOCK();

  /* call read pending */
  status =
      tml_read(nci_hal_ctrl.p_cmd_data, NCI_MAX_DATA_LEN,
               (transact_completion_callback_t)&read_app_data_complete, NULL);
  if (status != EMVCO_STATUS_PENDING) {
    LOG_EMVCOHAL_E("TML Read status error status = %x", status);
    wConfigStatus = tml_shutdown_cleanup();
    wConfigStatus = EMVCO_STATUS_FAILED;
    goto clean_and_return;
  }

  nci_ext_init();

init_retry:

  mode_switch_status = tml_ioctl(NFCCModeSwitchOn);
  LOG_EMVCOHAL_D("%s modeswitch IOCTL status:%d", __func__, mode_switch_status);
  led_switch_control(EMVCO_MODE_ON);

  status = send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_E("NCI_CORE_RESET: Failed");
    if (init_retry_cnt < 3) {
      init_retry_cnt++;
      (void)handle_power_cycle();
      goto init_retry;
    } else
      init_retry_cnt = 0;
    wConfigStatus = tml_shutdown_cleanup();
    wConfigStatus = EMVCO_STATUS_FAILED;
    goto clean_and_return;
  }

  if (nci_hal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
    status = send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
  } else {
    status = send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
  }
  status = send_ext_cmd(sizeof(cmd_set_config_deactivate_idle_power_off),
                        cmd_set_config_deactivate_idle_power_off);
  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_E("NCI_SET_CONFIG_DEACTIVATE : Failed");
    goto init_retry;
  }

  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_E("NCI_CORE_INIT : Failed");
    if (init_retry_cnt < 3) {
      init_retry_cnt++;
      (void)handle_power_cycle();
      goto init_retry;
    } else
      init_retry_cnt = 0;
    wConfigStatus = tml_shutdown_cleanup();
    wConfigStatus = EMVCO_STATUS_FAILED;
    goto clean_and_return;
  }
  /* Call open complete */
  min_open_app_data_channel_complete(wConfigStatus);
  LOG_EMVCOHAL_D("min_open_app_data_channel(): exit");
  return wConfigStatus;

clean_and_return:
  min_close_app_data_channel();
  CONCURRENCY_UNLOCK();
  if (nfc_dev_node != NULL) {
    osal_free(nfc_dev_node);
    nfc_dev_node = NULL;
  }
  return EMVCO_STATUS_FAILED;
}

int open_app_data_channel(emvco_stack_callback_t *p_cback,
                          emvco_stack_data_callback_t *p_data_cback,
                          emvco_state_change_callback_t *p_nfc_state_cback) {
  LOG_EMVCOHAL_D("%s:", __func__);
  m_p_nfc_stack_cback = p_cback;
  m_p_nfc_stack_data_cback = p_data_cback;
  m_p_nfc_state_cback = p_nfc_state_cback;
  return EMVCO_STATUS_SUCCESS;
}

EMVCO_STATUS core_reset_recovery() {
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;

  /*NCI_INIT_CMD*/
  static uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01,
                                    0x00}; // keep configuration
  static uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
  /* reset config cache */
  uint8_t retry_core_init_cnt = 0;

  LOG_EMVCOHAL_D("%s: recovery", __func__);

retry_core_init:
  if (retry_core_init_cnt > 3) {
    goto FAILURE;
  }

  status = tml_ioctl(ResetDevice);
  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_D("Controller Reset - FAILED\n");
    goto FAILURE;
  }
  status = send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if ((status != EMVCO_STATUS_SUCCESS) &&
      (nci_hal_ctrl.retry_cnt >= MAX_RETRY_COUNT)) {
    retry_core_init_cnt++;
    goto retry_core_init;
  } else if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_D("NCI_CORE_RESET: Failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }
  if (nci_hal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
    status = send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
  } else {
    status = send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
  }
  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_D("NCI_CORE_INIT : Failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }

  return EMVCO_STATUS_SUCCESS;
FAILURE:
  abort();
}

/******************************************************************************
 * Function         min_open_app_data_channel_complete
 *
 * Description      This function updates the status of
 *min_open_app_data_channel_complete to halstatus.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void min_open_app_data_channel_complete(EMVCO_STATUS status) {
  if (status == EMVCO_STATUS_SUCCESS) {
    nci_hal_ctrl.halStatus = HAL_STATUS_MIN_OPEN;
  }

  return;
}

/******************************************************************************
 * Function         open_app_data_channel_complete
 *
 * Description      This function inform the status of open_app_data_channel
 *                  function to libnfc-nci.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void open_app_data_channel_complete(EMVCO_STATUS status) {
  static lib_emvco_message_t msg;

  if (status == EMVCO_STATUS_SUCCESS) {
    msg.e_msgType = EMVCO_OPEN_CHNL_CPLT_MSG;
    nci_hal_ctrl.hal_open_status = true;
    nci_hal_ctrl.halStatus = HAL_STATUS_OPEN;
  } else {
    msg.e_msgType = EMVCO_OPEN_CHNL_ERROR_MSG;
  }

  msg.p_msg_data = NULL;
  msg.size = 0;

  tml_deferred_call(gptml_emvco_context->dw_callback_thread_id,
                    (lib_emvco_message_t *)&msg);

  return;
}

int send_app_data(uint16_t data_len, const uint8_t *p_data) {
  return send_app_data_internal(data_len, p_data);
}

int send_app_data_internal(uint16_t data_len, const uint8_t *p_data) {
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  static lib_emvco_message_t msg;
  if (nci_hal_ctrl.halStatus != HAL_STATUS_OPEN) {
    return EMVCO_STATUS_FAILED;
  }
  if (data_len > NCI_MAX_DATA_LEN) {
    LOG_EMVCOHAL_E("cmd_len exceeds limit NCI_MAX_DATA_LEN");
    __osal_log_error_write(0x534e4554, "121267042");
    goto clean_and_return;
  }
  /* Create local copy of cmd_data */
  osal_memcpy(nci_hal_ctrl.p_cmd_data, p_data, data_len);
  nci_hal_ctrl.cmd_len = data_len;
#ifdef P2P_PRIO_LOGIC_HAL_IMP
  /* Specific logic to block RF disable when P2P priority logic is busy */
  if (data_len < NORMAL_MODE_HEADER_LEN) {
    /* Avoid OOB Read */
    android_errorWriteLog(0x534e4554, "128530069");
  } else if (p_data[0] == 0x21 && p_data[1] == 0x06 && p_data[2] == 0x01 &&
             EnableP2P_PrioLogic == true) {
    LOG_EMVCOHAL_D("P2P priority logic busy: Disable it.");
    phNxpNciHal_clean_P2P_Prio();
  }
#endif

  /* Check for NXP ext before sending write */
  status = send_app_data_ext(&nci_hal_ctrl.cmd_len, nci_hal_ctrl.p_cmd_data,
                             &nci_hal_ctrl.rsp_len, nci_hal_ctrl.p_rsp_data);
  if (status != EMVCO_STATUS_SUCCESS) {
    /* Do not send packet to controller, send response directly */
    msg.e_msgType = EMVCO_DATA_RX_EVT;
    msg.p_msg_data = NULL;
    msg.size = 0;

    tml_deferred_call(gptml_emvco_context->dw_callback_thread_id,
                      (lib_emvco_message_t *)&msg);
    goto clean_and_return;
  }

  CONCURRENCY_LOCK();
  data_len =
      send_app_data_unlocked(nci_hal_ctrl.cmd_len, nci_hal_ctrl.p_cmd_data);
  CONCURRENCY_UNLOCK();

  if (icode_send_eof == 1) {
    usleep(10000);
    icode_send_eof = 2;
    status = send_ext_cmd(3, cmd_icode_eof);
    if (status != EMVCO_STATUS_SUCCESS) {
      LOG_EMVCOHAL_E("ICODE end of frame command failed");
    }
  }

clean_and_return:
  /* No data written */
  return data_len;
}

int send_app_data_unlocked(uint16_t data_len, const uint8_t *p_data) {
  EMVCO_STATUS status = EMVCO_STATUS_INVALID_PARAMETER;
  nci_hal_sem cb_data;
  nci_hal_ctrl.retry_cnt = 0;
  static uint8_t reset_ntf[] = {0x60, 0x00, 0x06, 0xA0, 0x00,
                                0xC7, 0xD4, 0x00, 0x00};
  /* Create the local semaphore */
  if (init_cb_data(&cb_data, NULL) != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_D("send_app_data_unlocked Create cb data failed");
    data_len = 0;
    goto clean_and_return;
  }

  /* Create local copy of cmd_data */
  osal_memcpy(nci_hal_ctrl.p_cmd_data, p_data, data_len);
  nci_hal_ctrl.cmd_len = data_len;

  /* check for write synchronyztion */
  if (check_ncicmd_write_window(nci_hal_ctrl.cmd_len,
                                nci_hal_ctrl.p_cmd_data) !=
      EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_D("send_app_data_unlocked check nci write window failed");
    data_len = 0;
    goto clean_and_return;
  }

retry:

  data_len = nci_hal_ctrl.cmd_len;

  status = tml_write((uint8_t *)nci_hal_ctrl.p_cmd_data,
                     (uint16_t)nci_hal_ctrl.cmd_len,
                     (transact_completion_callback_t)&send_app_data_complete,
                     (void *)&cb_data);
  if (status != EMVCO_STATUS_PENDING) {
    LOG_EMVCOHAL_E("write_unlocked status error");
    data_len = 0;
    goto clean_and_return;
  }

  /* Wait for callback response */
  if (SEM_WAIT(cb_data)) {
    LOG_EMVCOHAL_E("write_unlocked semaphore error");
    data_len = 0;
    goto clean_and_return;
  }

  if (cb_data.status != EMVCO_STATUS_SUCCESS) {
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

      osal_sem_post(&(nci_hal_ctrl.sync_spi_nfc));

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

clean_and_return:
  cleanup_cb_data(&cb_data);
  return data_len;
}

/******************************************************************************
 * Function         send_app_data_complete
 *
 * Description      This function handles write callback.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void send_app_data_complete(void *p_context,
                                   osal_transact_info_t *pInfo) {
  nci_hal_sem *p_cb_data = (nci_hal_sem *)p_context;
  if (pInfo->w_status == EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_D("write successful status = 0x%x", pInfo->w_status);
  } else {
    LOG_EMVCOHAL_D("write error status = 0x%x", pInfo->w_status);
  }

  p_cb_data->status = pInfo->w_status;

  SEM_POST(p_cb_data);

  return;
}

/******************************************************************************
 * Function         read_app_data_complete
 *
 * Description      This function is called whenever there is an NCI packet
 *                  received from NFCC. It could be RSP or NTF packet. This
 *                  function provide the received NCI packet to libnfc-nci
 *                  using data callback of libnfc-nci.
 *                  There is a pending read called from each
 *                  read_app_data_complete so each a packet received from
 *                  NFCC can be provide to libnfc-nci.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void read_app_data_complete(void *p_context,
                                   osal_transact_info_t *pInfo) {
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  int sem_val;
  UNUSED(p_context);
  if (nci_hal_ctrl.read_retry_cnt == 1) {
    nci_hal_ctrl.read_retry_cnt = 0;
  }
  if (pInfo->w_status == EMVCO_STATUS_SUCCESS) {

    osal_sem_getvalue(&(nci_hal_ctrl.sync_spi_nfc), &sem_val);
    if (((pInfo->p_buff[0] & NCI_MT_MASK) == NCI_MT_RSP) && sem_val == 0) {
      osal_sem_post(&(nci_hal_ctrl.sync_spi_nfc));
    }
    /*Check the Omapi command response and store in dedicated buffer to solve
     * sync issue*/
    if (pInfo->p_buff[0] == 0x4F && pInfo->p_buff[1] == 0x01 &&
        pInfo->p_buff[2] == 0x01) {
      nci_hal_ctrl.p_rx_ese_data = pInfo->p_buff;
      nci_hal_ctrl.rx_ese_data_len = pInfo->w_length;
      SEM_POST(&(nci_hal_ctrl.ext_cb_data));
    } else {
      nci_hal_ctrl.p_rx_data = pInfo->p_buff;
      nci_hal_ctrl.rx_data_len = pInfo->w_length;
      status =
          process_ext_rsp(nci_hal_ctrl.p_rx_data, &nci_hal_ctrl.rx_data_len);
    }

    print_res_status(pInfo->p_buff, &pInfo->w_length);
    /* Check if response should go to hal module only */
    if (nci_hal_ctrl.hal_ext_enabled == TRUE &&
        (nci_hal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_RSP) {
      if (status == EMVCO_STATUS_FAILED) {
        nci_hal_ctrl.ext_cb_data.status = status;
      }
      /* Unlock semaphore only for responses*/
      if ((nci_hal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_RSP ||
          ((icode_detected == true) && (icode_send_eof == 3))) {
        /* Unlock semaphore */
        SEM_POST(&(nci_hal_ctrl.ext_cb_data));
      }
    } // Notification Checking
    else if ((nci_hal_ctrl.hal_ext_enabled == TRUE) &&
             ((nci_hal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_NTF) &&
             (nci_hal_ctrl.nci_info.wait_for_ntf == TRUE)) {
      /* Unlock semaphore waiting for only  ntf*/
      SEM_POST(&(nci_hal_ctrl.ext_cb_data));
      nci_hal_ctrl.nci_info.wait_for_ntf = FALSE;
    }
    /* Read successful send the event to higher layer */
    else if ((nci_hal_ctrl.p_nfc_stack_data_cback != NULL) &&
             (status == EMVCO_STATUS_SUCCESS)) {
      (*nci_hal_ctrl.p_nfc_stack_data_cback)(nci_hal_ctrl.rx_data_len,
                                             nci_hal_ctrl.p_rx_data);
    }
  } else {
    LOG_EMVCOHAL_E("read error status = 0x%x", pInfo->w_status);
  }

  if (nci_hal_ctrl.halStatus == HAL_STATUS_CLOSE &&
      nci_hal_ctrl.nci_info.wait_for_ntf == FALSE) {
    return;
  }
  /* Read again because read must be pending always.*/
  status = tml_read(
      // Rx_data, NCI_MAX_DATA_LEN,
      nci_hal_ctrl.p_rsp_data, NCI_MAX_DATA_LEN,
      (transact_completion_callback_t)&read_app_data_complete, NULL);
  if (status != EMVCO_STATUS_PENDING) {
    LOG_EMVCOHAL_E("read status error status = %x", status);
    /* TODO: Not sure how to handle this ? */
  }

  return;
}

int close_app_data_channel(bool bShutdown) {
  EMVCO_STATUS status;
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x00};

  static uint8_t cmd_ven_disable_nci[] = {0x20, 0x02, 0x05, 0x01,
                                          0xA0, 0x07, 0x01, 0x02};

  // AutoThreadMutex a(sHalFnLock);
  if (nci_hal_ctrl.halStatus == HAL_STATUS_CLOSE) {
    LOG_EMVCOHAL_D("close_app_data_channel is already closed, ignoring close");
    return EMVCO_STATUS_FAILED;
  }

  CONCURRENCY_LOCK();

  int sem_val;
  osal_sem_getvalue(&(nci_hal_ctrl.sync_spi_nfc), &sem_val);
  if (sem_val == 0) {
    osal_sem_post(&(nci_hal_ctrl.sync_spi_nfc));
  }
  if (!bShutdown) {
    status = send_ext_cmd(sizeof(cmd_ven_disable_nci), cmd_ven_disable_nci);
    if (status != EMVCO_STATUS_SUCCESS) {
      LOG_EMVCOHAL_E("CMD_VEN_DISABLE_NCI: Failed");
    }
  }
  nci_hal_ctrl.halStatus = HAL_STATUS_CLOSE;
  status = send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_E("NCI_CORE_RESET: Failed");
  }
  osal_sem_destroy(&nci_hal_ctrl.sync_spi_nfc);

  if (NULL != gptml_emvco_context->p_dev_handle) {
    close_app_data_channel_complete(EMVCO_STATUS_SUCCESS);
    /* Abort any pending read and write */
    status = tml_read_abort();
    status = tml_write_abort();

    osal_timer_cleanup();

    status = tml_shutdown();

    if (0 !=
        osal_thread_join(nci_hal_ctrl.emvco_hal_client_thread, (void **)NULL)) {
      LOG_EMVCO_TML_E("Fail to kill client thread!");
    }

    tml_cleanup();

    osal_msg_release(nci_hal_ctrl.gDrvCfg.n_client_id);

    osal_memset(&nci_hal_ctrl, 0x00, sizeof(nci_hal_ctrl));

    LOG_EMVCOHAL_D("close_app_data_channel - phOsalNfc_DeInit completed");
  }
  CONCURRENCY_UNLOCK();
  cleanup_monitor();
  /* Return success always */
  return EMVCO_STATUS_SUCCESS;
}
/******************************************************************************
 * Function         min_close_app_data_channel
 *
 * Description      This function close the NFCC interface and free all
 *                  resources.This is called by libnfc-nci on NFC service stop.
 *
 * Returns          Always return EMVCO_STATUS_SUCCESS (0).
 *
 ******************************************************************************/
int min_close_app_data_channel(void) {
  EMVCO_STATUS status;
  /*NCI_RESET_CMD*/
  uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x00};
  CONCURRENCY_LOCK();
  nci_hal_ctrl.halStatus = HAL_STATUS_CLOSE;
  status = send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if (status != EMVCO_STATUS_SUCCESS) {
    LOG_EMVCOHAL_E("NCI_CORE_RESET: Failed");
  }
  sem_destroy(&nci_hal_ctrl.sync_spi_nfc);
  if (NULL != gptml_emvco_context->p_dev_handle) {
    close_app_data_channel_complete(EMVCO_STATUS_SUCCESS);
    /* Abort any pending read and write */
    status = tml_read_abort();
    status = tml_write_abort();

    osal_timer_cleanup();

    status = tml_shutdown();

    if (0 !=
        pthread_join(nci_hal_ctrl.emvco_hal_client_thread, (void **)NULL)) {
      LOG_EMVCO_TML_E("Fail to kill client thread!");
    }

    tml_cleanup();

    osal_msg_release(nci_hal_ctrl.gDrvCfg.n_client_id);

    osal_memset(&nci_hal_ctrl, 0x00, sizeof(nci_hal_ctrl));

    LOG_EMVCOHAL_D("close_app_data_channel - phOsalNfc_DeInit completed");
  }

  CONCURRENCY_UNLOCK();

  cleanup_monitor();

  return EMVCO_STATUS_SUCCESS;
}
/******************************************************************************
 * Function         close_app_data_channel_complete
 *
 * Description      This function inform libnfc-nci about result of
 *                  close_app_data_channel.
 *
 * Returns          void.
 *
 ******************************************************************************/
void close_app_data_channel_complete(EMVCO_STATUS status) {
  static lib_emvco_message_t msg;

  if (status == EMVCO_STATUS_SUCCESS) {
    msg.e_msgType = EMVCO_CLOSE_CHNL_CPLT_MSG;
  } else {
    msg.e_msgType = EMVCO_OPEN_CHNL_ERROR_MSG;
  }
  msg.p_msg_data = NULL;
  msg.size = 0;

  tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);

  return;
}

/******************************************************************************
 * Function         handle_power_cycle
 *
 * Description      This function is called by libnfc-nci when power cycling is
 *                  performed. When processing is complete it is notified to
 *                  libnfc-nci through power_cycle_complete.
 *
 * Returns          Always return EMVCO_STATUS_SUCCESS (0).
 *
 ******************************************************************************/
int handle_power_cycle(void) {
  LOG_EMVCOHAL_D("Power Cycle");
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  if (nci_hal_ctrl.halStatus != HAL_STATUS_OPEN) {
    LOG_EMVCOHAL_D("Power Cycle failed due to hal status not open");
    return EMVCO_STATUS_FAILED;
  }
  status = tml_ioctl(ResetDevice);

  if (EMVCO_STATUS_SUCCESS == status) {
    LOG_EMVCOHAL_D("Controller Reset - SUCCESS\n");
  } else {
    LOG_EMVCOHAL_D("Controller Reset - FAILED\n");
  }
  power_cycle_complete(EMVCO_STATUS_SUCCESS);
  return EMVCO_STATUS_SUCCESS;
}

/******************************************************************************
 * Function         power_cycle_complete
 *
 * Description      This function is called to provide the status of
 *                  handle_power_cycle to libnfc-nci through callback.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void power_cycle_complete(EMVCO_STATUS status) {
  static lib_emvco_message_t msg;

  if (status == EMVCO_STATUS_SUCCESS) {
    msg.e_msgType = EMVCO_OPEN_CHNL_CPLT_MSG;
  } else {
    msg.e_msgType = EMVCO_OPEN_CHNL_ERROR_MSG;
  }
  msg.p_msg_data = NULL;
  msg.size = 0;

  tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &msg);

  return;
}
/******************************************************************************
 * Function         check_ncicmd_write_window
 *
 * Description      This function is called to check the write synchroniztion
 *                  status if write already aquired then wait for corresponding
                    read to complete.
 *
 * Returns          return 0 on success and -1 on fail.
 *
 ******************************************************************************/

int check_ncicmd_write_window(uint16_t cmd_len, uint8_t *p_cmd) {
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  int sem_timedout = 2, s;
  struct timespec ts;

  if (cmd_len < 1) {
    android_errorWriteLog(0x534e4554, "153880357");
    return EMVCO_STATUS_FAILED;
  }

  if ((p_cmd[0] & 0xF0) == 0x20) {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec += sem_timedout;

    while ((s = osal_sem_timedwait_monotonic_np(&nci_hal_ctrl.sync_spi_nfc,
                                                &ts)) == -1 &&
           errno == EINTR)
      continue; /* Restart if interrupted by handler */

    if (s != -1) {
      status = EMVCO_STATUS_SUCCESS;
    }
  } else {
    /* cmd window check not required for writing data packet */
    status = EMVCO_STATUS_SUCCESS;
  }
  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_do_factory_reset
 *
 * Description      This function is called during factory reset to clear/reset
 *                  nfc sub-system persistant data.
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_do_factory_reset(void) {
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  if (nci_hal_ctrl.halStatus == HAL_STATUS_CLOSE) {
    status = min_open_app_data_channel();
    if (status != EMVCO_STATUS_SUCCESS) {
      LOG_EMVCOHAL_E("%s: NXP Nfc Open failed", __func__);
      return;
    }
  }
}

/******************************************************************************
 * Function         print_res_status
 *
 * Description      This function is called to process the response status
 *                  and print the status byte.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void print_res_status(uint8_t *p_rx_data, uint16_t *p_len) {
  static uint8_t response_buf[][30] = {"STATUS_OK",
                                       "STATUS_REJECTED",
                                       "STATUS_RF_FRAME_CORRUPTED",
                                       "STATUS_FAILED",
                                       "STATUS_NOT_INITIALIZED",
                                       "STATUS_SYNTAX_ERROR",
                                       "STATUS_SEMANTIC_ERROR",
                                       "RFU",
                                       "RFU",
                                       "STATUS_INVALID_PARAM",
                                       "STATUS_MESSAGE_SIZE_EXCEEDED",
                                       "STATUS_UNDEFINED"};
  int status_byte;
  if (p_rx_data[0] == 0x40 && (p_rx_data[1] == 0x02 || p_rx_data[1] == 0x03)) {
    if (p_rx_data[2] && p_rx_data[3] <= 10) {
      status_byte = p_rx_data[CORE_RES_STATUS_BYTE];
      LOG_EMVCOHAL_D("%s: response status =%s", __func__,
                     response_buf[status_byte]);
    } else {
      LOG_EMVCOHAL_D("%s: response status =%s", __func__, response_buf[11]);
    }
    if (nci_clock.is_clock_set) {
      int i, len = sizeof(nci_clock.p_rx_data);
      if (*p_len > len) {
        __osal_log_error_write(0x534e4554, "169257710");
      } else {
        len = *p_len;
      }
      for (i = 0; i < len; i++) {
        nci_clock.p_rx_data[i] = p_rx_data[i];
      }
    }

    else if (phNxpNciRfSet.is_get_config) {
      int i, len = sizeof(phNxpNciRfSet.p_rx_data);
      if (*p_len > len) {
        __osal_log_error_write(0x534e4554, "169258733");
      } else {
        len = *p_len;
      }
      for (i = 0; i < len; i++) {
        phNxpNciRfSet.p_rx_data[i] = p_rx_data[i];
        // LOG_EMVCOHAL_D("%s: response status =0x%x",__func__,p_rx_data[i]);
      }
    } else if (eeprom_area.is_get_eeprom_area) {
      int i, len = sizeof(eeprom_area.p_rx_data) + 8;
      if (*p_len > len) {
        __osal_log_error_write(0x534e4554, "169258884");
      } else {
        len = *p_len;
      }
      for (i = 8; i < len; i++) {
        eeprom_area.p_rx_data[i - 8] = p_rx_data[i];
      }
    } else if (nci_hal_ctrl.gpio_info.state == GPIO_STORE) {
      LOG_EMVCOHAL_D("%s: Storing GPIO Values...", __func__);
      nci_hal_ctrl.gpio_info.values[0] = p_rx_data[9];
      nci_hal_ctrl.gpio_info.values[1] = p_rx_data[8];
    } else if (nci_hal_ctrl.gpio_info.state == GPIO_RESTORE) {
      LOG_EMVCOHAL_D("%s: Restoring GPIO Values...", __func__);
      nci_hal_ctrl.gpio_info.values[0] = p_rx_data[9];
      nci_hal_ctrl.gpio_info.values[1] = p_rx_data[8];
    }
  }

  if (p_rx_data[2] && (config_access == true)) {
    if (p_rx_data[3] != EMVCO_STATUS_SUCCESS) {
      LOG_EMVCOHAL_W("Invalid Data from config file.");
      config_success = false;
    }
  }
}
