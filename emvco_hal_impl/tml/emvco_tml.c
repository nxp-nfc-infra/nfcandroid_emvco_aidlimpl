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

/*
 * TML Implementation.
 */

#include <emvco_config.h>
#include <emvco_log.h>
#include <emvco_tml.h>
#include <emvco_tml_i2c.h>
#include <emvco_util.h>
#include <osal_message_queue_lib.h>
#include <osal_thread.h>
#include <osal_timer.h>

/*
 * Duration of Timer to wait after sending an Nci packet
 */
#define PHTMLNFC_MAXTIME_RETRANSMIT (200U)
#define MAX_WRITE_RETRY_COUNT 0x03
#define MAX_READ_RETRY_DELAY_IN_MILLISEC (150U)
/* Retry Count = Standby Recovery time of NFCC / Retransmission time + 1 */
static uint8_t bCurrentRetryCount = (2000 / PHTMLNFC_MAXTIME_RETRANSMIT) + 1;

/* Value to reset variables of TML  */
#define TMLNFC_RESET_VALUE (0x00)

/* Indicates a Initial or offset value */
#define TMLNFC_VALUE_ONE (0x01)

/* Initialize Context structure pointer used to access context structure */
tml_emvco_context_t *gptml_emvco_context = NULL;
/* Local Function prototypes */
static EMVCO_STATUS tml_emvco_start_thread(void);
static void tml_readDeferredCb(void *pParams);
static void tml_writeDeferredCb(void *pParams);
static void *emvco_tml_thread(void *pParam);
static void *tml_writer_thread(void *pParam);
static void tml_emvco_re_trx_timer_cb(uint32_t dw_timer_id, void *p_context);
static EMVCO_STATUS tml_initiateTimer(void);

/* Function definitions */

/*******************************************************************************
**
** Function         tml_init
**
** Description      Provides initialization of TML layer and hardware interface
**                  Configures given hardware interface and sends handle to the
**                  caller
**
** Parameters       pConfig - TML configuration details as provided by the upper
**                            layer
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS - initialization successful
**                  EMVCO_STATUS_INVALID_PARAMETER - at least one parameter is
**                                                invalid
**                  EMVCO_STATUS_FAILED - initialization failed (for example,
**                                     unable to open hardware interface)
**                  EMVCO_STATUS_INVALID_DEVICE - device has not been opened or
*has
**                                             been disconnected
**
*******************************************************************************/
EMVCO_STATUS tml_init(ptml_emvco_Config_t pConfig) {
  EMVCO_STATUS wInitStatus = EMVCO_STATUS_SUCCESS;

  /* Check if TML layer is already Initialized */
  if (NULL != gptml_emvco_context) {
    /* TML initialization is already completed */
    wInitStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_ALREADY_INITIALISED);
  }
  /* Validate Input parameters */
  else if ((NULL == pConfig) ||
           (TMLNFC_RESET_VALUE == pConfig->dw_get_msg_thread_id)) {
    /*Parameters passed to TML init are wrong */
    wInitStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_INVALID_PARAMETER);
  } else {
    /* Allocate memory for TML context */
    gptml_emvco_context =
        (tml_emvco_context_t *)malloc(sizeof(tml_emvco_context_t));

    if (NULL == gptml_emvco_context) {
      wInitStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_FAILED);
    } else {
      /* Initialise all the internal TML variables */
      memset(gptml_emvco_context, TMLNFC_RESET_VALUE,
             sizeof(tml_emvco_context_t));
      /* Make sure that the thread runs once it is created */
      gptml_emvco_context->b_thread_done = 1;

      /* Open the device file to which data is read/written */
      wInitStatus =
          i2c_open_and_configure(pConfig, &(gptml_emvco_context->p_dev_handle));

      if (EMVCO_STATUS_SUCCESS != wInitStatus) {
        wInitStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_INVALID_DEVICE);
        gptml_emvco_context->p_dev_handle = NULL;
      } else {
        gptml_emvco_context->t_read_info.b_enable = 0;
        gptml_emvco_context->t_write_info.b_enable = 0;
        gptml_emvco_context->t_read_info.b_thread_busy = false;
        gptml_emvco_context->t_write_info.b_thread_busy = false;
        if (osal_mutex_init(&gptml_emvco_context->read_info_update_mutex) !=
            0) {
          wInitStatus = EMVCO_STATUS_FAILED;
        } else if (0 != sem_init(&gptml_emvco_context->rx_semaphore, 0, 0)) {
          wInitStatus = EMVCO_STATUS_FAILED;
        } else if (0 != sem_init(&gptml_emvco_context->tx_semaphore, 0, 0)) {
          wInitStatus = EMVCO_STATUS_FAILED;
        } else if (0 !=
                   sem_init(&gptml_emvco_context->post_msg_semaphore, 0, 0)) {
          wInitStatus = EMVCO_STATUS_FAILED;
        } else {
          sem_post(&gptml_emvco_context->post_msg_semaphore);
          /* Start TML thread (to handle write and read operations) */
          if (EMVCO_STATUS_SUCCESS != tml_emvco_start_thread()) {
            wInitStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_FAILED);
          } else {
            /* Create Timer used for Retransmission of NCI packets */
            gptml_emvco_context->dw_timer_id = osal_timer_create();
            if (PH_OSALNFC_TIMER_ID_INVALID !=
                gptml_emvco_context->dw_timer_id) {
              /* Store the Thread Identifier to which Message is to be posted */
              gptml_emvco_context->dw_callback_thread_id =
                  pConfig->dw_get_msg_thread_id;
              /* Enable retransmission of Nci packet & set retry count to
               * default */
              gptml_emvco_context->e_config = disable_retrans;
              /* Retry Count = Standby Recovery time of NFCC / Retransmission
               * time + 1 */
              gptml_emvco_context->b_retry_count =
                  (2000 / PHTMLNFC_MAXTIME_RETRANSMIT) + 1;
              gptml_emvco_context->bWriteCbInvoked = false;
            } else {
              wInitStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_FAILED);
            }
          }
        }
      }
    }
  }
  /* Clean up all the TML resources if any error */
  if (EMVCO_STATUS_SUCCESS != wInitStatus) {
    /* Clear all handles and memory locations initialized during init */
    tml_shutdown_cleanup();
  }

  return wInitStatus;
}

/*******************************************************************************
**
** Function         tml_writer_thread
**
** Description      Writes the requested data onto the lower layer driver
**
** Parameters       pParam  - context provided by upper layer
**
** Returns          None
**
*******************************************************************************/
static void *tml_writer_thread(void *pParam) {
  EMVCO_STATUS w_status = EMVCO_STATUS_SUCCESS;
  int32_t dwNoBytesWrRd = TMLNFC_RESET_VALUE;
  /* Transaction info buffer to be passed to Callback Thread */
  static osal_transact_info_t tTransactionInfo;
  /* Structure containing Tml callback function and parameters to be invoked
     by the callback thread */
  static lib_emvco_deferred_call_t tDeferredInfo;
  /* Initialize Message structure to post message onto Callback Thread */
  static lib_emvco_message_t tMsg;
  /* In case of I2C Write Retry */
  static uint16_t retry_cnt;
  UNUSED(pParam);
  LOG_EMVCO_TML_D("PN54X - Tml Writer Thread Started................\n");

  /* Writer thread loop shall be running till shutdown is invoked */
  while (gptml_emvco_context->b_thread_done) {
    LOG_EMVCO_TML_D("PN54X - Tml Writer Thread Running................\n");
    sem_wait(&gptml_emvco_context->tx_semaphore);
    /* If Tml write is requested */
    if (1 == gptml_emvco_context->t_write_info.b_enable) {
      LOG_EMVCO_TML_D("PN54X - Write requested.....\n");
      /* Set the variable to success initially */
      w_status = EMVCO_STATUS_SUCCESS;
      if (NULL != gptml_emvco_context->p_dev_handle) {
        gptml_emvco_context->t_write_info.b_enable = 0;
        dwNoBytesWrRd = TMLNFC_RESET_VALUE;
        LOG_EMVCO_TML_D("PN54X - Invoking I2C Write.....\n");
        dwNoBytesWrRd = i2c_write(gptml_emvco_context->p_dev_handle,
                                  gptml_emvco_context->t_write_info.p_buffer,
                                  gptml_emvco_context->t_write_info.w_length);
        if (-1 == dwNoBytesWrRd) {
          LOG_EMVCO_TML_D("PN54X - Error in I2C Write.....\n");
          w_status = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_FAILED);
        } else {
          print_packet("SEND", gptml_emvco_context->t_write_info.p_buffer,
                       gptml_emvco_context->t_write_info.w_length);
        }

        retry_cnt = 0;
        if (EMVCO_STATUS_SUCCESS == w_status) {
          LOG_EMVCO_TML_D("PN54X - I2C Write successful.....\n");
          dwNoBytesWrRd = TMLNFC_VALUE_ONE;
        }
        /* Fill the Transaction info structure to be passed to Callback Function
         */
        tTransactionInfo.w_status = w_status;
        tTransactionInfo.p_buff = gptml_emvco_context->t_write_info.p_buffer;
        /* Actual number of bytes written is filled in the structure */
        tTransactionInfo.w_length = (uint16_t)dwNoBytesWrRd;

        /* Prepare the message to be posted on the User thread */
        tDeferredInfo.p_callback = &tml_writeDeferredCb;
        tDeferredInfo.p_parameter = &tTransactionInfo;
        /* Write operation completed successfully. Post a Message onto Callback
         * Thread*/
        tMsg.e_msgType = LIB_EMVCO_DEFERREDCALL_MSG;
        tMsg.p_msg_data = &tDeferredInfo;
        tMsg.size = sizeof(tDeferredInfo);

        /* Check whether Retransmission needs to be started,
         * If yes, Post message only if
         * case 1. Message is not posted &&
         * case 11. Write status is success ||
         * case 12. Last retry of write is also failure
         */
        if ((enable_retrans == gptml_emvco_context->e_config) &&
            (0x00 != (gptml_emvco_context->t_write_info.p_buffer[0] & 0xE0))) {
          if (gptml_emvco_context->bWriteCbInvoked == false) {
            if ((EMVCO_STATUS_SUCCESS == w_status) ||
                (bCurrentRetryCount == 0)) {
              LOG_EMVCO_TML_D("PN54X - Posting Write message.....\n");
              tml_deferred_call(gptml_emvco_context->dw_callback_thread_id,
                                &tMsg);
              gptml_emvco_context->bWriteCbInvoked = true;
            }
          }
        } else {
          LOG_EMVCO_TML_D("PN54X - Posting Fresh Write message.....\n");
          tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &tMsg);
        }
      } else {
        LOG_EMVCO_TML_D("PN54X - gptml_emvco_context->p_dev_handle is NULL");
      }

      /* If Data packet is sent, then NO retransmission */
      if ((enable_retrans == gptml_emvco_context->e_config) &&
          (0x00 != (gptml_emvco_context->t_write_info.p_buffer[0] & 0xE0))) {
        LOG_EMVCO_TML_D("PN54X - Starting timer for Retransmission case");
        w_status = tml_initiateTimer();
        if (EMVCO_STATUS_SUCCESS != w_status) {
          /* Reset Variables used for Retransmission */
          LOG_EMVCO_TML_D("PN54X - Retransmission timer initiate failed");
          gptml_emvco_context->t_write_info.b_enable = 0;
          bCurrentRetryCount = 0;
        }
      }
    } else {
      LOG_EMVCO_TML_D("PN54X - Write request NOT enabled");
      usleep(10000);
    }

  } /* End of While loop */

  return NULL;
}

/*******************************************************************************
**
** Function         tml_emvco_start_thread
**
** Description      Initializes comport, reader and writer threads
**
** Parameters       None
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS - threads initialized successfully
**                  EMVCO_STATUS_FAILED - initialization failed due to system
*error
**
*******************************************************************************/
static EMVCO_STATUS tml_emvco_start_thread(void) {
  EMVCO_STATUS wStartStatus = EMVCO_STATUS_SUCCESS;
  void *h_threadsEvent = 0x00;
  int pthread_create_status = 0;

  /* Create Reader and Writer threads */
  pthread_create_status =
      osal_thread_create(&gptml_emvco_context->reader_thread, NULL,
                         &emvco_tml_thread, (void *)h_threadsEvent);
  if (0 != pthread_create_status) {
    wStartStatus = EMVCO_STATUS_FAILED;
  } else {
    /*Start Writer Thread*/
    pthread_create_status =
        pthread_create(&gptml_emvco_context->writer_thread, NULL,
                       &tml_writer_thread, (void *)h_threadsEvent);
    if (0 != pthread_create_status) {
      wStartStatus = EMVCO_STATUS_FAILED;
    }
  }

  return wStartStatus;
}

/*******************************************************************************
**
** Function         tml_emvco_re_trx_timer_cb
**
** Description      This is the timer callback function after timer expiration.
**
** Parameters       dwThreadId  - id of the thread posting message
**                  p_context    - context provided by upper layer
**
** Returns          None
**
*******************************************************************************/
static void tml_emvco_re_trx_timer_cb(uint32_t dw_timer_id, void *p_context) {
  if ((gptml_emvco_context->dw_timer_id == dw_timer_id) &&
      (NULL == p_context)) {
    /* If Retry Count has reached its limit,Retransmit Nci
       packet */
    if (0 == bCurrentRetryCount) {
      /* Since the count has reached its limit,return from timer callback
         Upper layer Timeout would have happened */
    } else {
      bCurrentRetryCount--;
      gptml_emvco_context->t_write_info.b_thread_busy = true;
      gptml_emvco_context->t_write_info.b_enable = 1;
    }
    sem_post(&gptml_emvco_context->tx_semaphore);
  }

  return;
}

/*******************************************************************************
**
** Function         tml_initiateTimer
**
** Description      Start a timer for Tx and Rx thread.
**
** Parameters       void
**
** Returns          NFC status
**
*******************************************************************************/
static EMVCO_STATUS tml_initiateTimer(void) {
  EMVCO_STATUS w_status = EMVCO_STATUS_SUCCESS;

  /* Start Timer once Nci packet is sent */
  w_status = osal_timer_start(gptml_emvco_context->dw_timer_id,
                              (uint32_t)PHTMLNFC_MAXTIME_RETRANSMIT,
                              tml_emvco_re_trx_timer_cb, NULL);

  return w_status;
}

/*******************************************************************************
**
** Function         emvco_tml_thread
**
** Description      Read the data from the lower layer driver
**
** Parameters       pParam  - parameters for Writer thread function
**
** Returns          None
**
*******************************************************************************/
static void *emvco_tml_thread(void *pParam) {
  EMVCO_STATUS w_status = EMVCO_STATUS_SUCCESS;
  int32_t dwNoBytesWrRd = TMLNFC_RESET_VALUE;
  uint8_t temp[260];
  uint8_t readRetryDelay = 0;
  /* Transaction info buffer to be passed to Callback Thread */
  static osal_transact_info_t tTransactionInfo;
  /* Structure containing Tml callback function and parameters to be invoked
     by the callback thread */
  static lib_emvco_deferred_call_t tDeferredInfo;
  /* Initialize Message structure to post message onto Callback Thread */
  static lib_emvco_message_t tMsg;
  UNUSED(pParam);
  LOG_EMVCO_TML_D("PN54X - Tml Reader Thread Started................\n");

  /* Writer thread loop shall be running till shutdown is invoked */
  while (gptml_emvco_context->b_thread_done) {
    /* If Tml write is requested */
    /* Set the variable to success initially */
    w_status = EMVCO_STATUS_SUCCESS;
    sem_wait(&gptml_emvco_context->rx_semaphore);

    /* If Tml read is requested */
    if (1 == gptml_emvco_context->t_read_info.b_enable) {
      LOG_EMVCO_TML_D("PN54X - Read requested.....\n");
      /* Set the variable to success initially */
      w_status = EMVCO_STATUS_SUCCESS;

      /* Variable to fetch the actual number of bytes read */
      dwNoBytesWrRd = TMLNFC_RESET_VALUE;

      /* Read the data from the file onto the buffer */
      if (NULL != gptml_emvco_context->p_dev_handle) {
        LOG_EMVCO_TML_D("PN54X - Invoking I2C Read.....\n");
        dwNoBytesWrRd = i2c_read(gptml_emvco_context->p_dev_handle, temp, 260);
        if (-1 == dwNoBytesWrRd) {
          LOG_EMVCO_TML_E("PN54X - Error in I2C Read.....\n");
          if (readRetryDelay < MAX_READ_RETRY_DELAY_IN_MILLISEC) {
            /*sleep for 30/60/90/120/150 msec between each read trial incase of
             * read error*/
            readRetryDelay += 30;
          }
          usleep(readRetryDelay * 1000);
          sem_post(&gptml_emvco_context->rx_semaphore);
        } else if (dwNoBytesWrRd > 260) {
          LOG_EMVCO_TML_E("Numer of bytes read exceeds the limit 260.....\n");
          readRetryDelay = 0;
          sem_post(&gptml_emvco_context->rx_semaphore);
        } else {
          osal_mutex_lock(&gptml_emvco_context->read_info_update_mutex);
          memcpy(gptml_emvco_context->t_read_info.p_buffer, temp,
                 dwNoBytesWrRd);
          readRetryDelay = 0;

          LOG_EMVCO_TML_D("PN54X - I2C Read successful.....\n");
          /* This has to be reset only after a successful read */
          gptml_emvco_context->t_read_info.b_enable = 0;
          if ((enable_retrans == gptml_emvco_context->e_config) &&
              (0x00 != (gptml_emvco_context->t_read_info.p_buffer[0] & 0xE0))) {
            LOG_EMVCO_TML_D("PN54X - Retransmission timer stopped.....\n");
            /* Stop Timer to prevent Retransmission */
            uint32_t timerStatus =
                osal_timer_stop(gptml_emvco_context->dw_timer_id);
            if (EMVCO_STATUS_SUCCESS != timerStatus) {
              LOG_EMVCO_TML_E("PN54X - timer stopped returned failure.....\n");
            } else {
              gptml_emvco_context->bWriteCbInvoked = false;
            }
          }
          if (gptml_emvco_context->t_write_info.b_thread_busy) {
            LOG_EMVCO_TML_D("Delay Read if write thread is busy");
            usleep(2000); /*2ms delay to give prio to write complete */
          }
          /* Update the actual number of bytes read including header */
          gptml_emvco_context->t_read_info.w_length = (uint16_t)(dwNoBytesWrRd);
          print_packet("RECV", gptml_emvco_context->t_read_info.p_buffer,
                       gptml_emvco_context->t_read_info.w_length);

          dwNoBytesWrRd = TMLNFC_RESET_VALUE;

          /* Fill the Transaction info structure to be passed to Callback
           * Function */
          tTransactionInfo.w_status = w_status;
          tTransactionInfo.p_buff = gptml_emvco_context->t_read_info.p_buffer;
          /* Actual number of bytes read is filled in the structure */
          tTransactionInfo.w_length = gptml_emvco_context->t_read_info.w_length;

          /* Read operation completed successfully. Post a Message onto Callback
           * Thread*/
          /* Prepare the message to be posted on User thread */
          tDeferredInfo.p_callback = &tml_readDeferredCb;
          tDeferredInfo.p_parameter = &tTransactionInfo;
          tMsg.e_msgType = LIB_EMVCO_DEFERREDCALL_MSG;
          tMsg.p_msg_data = &tDeferredInfo;
          tMsg.size = sizeof(tDeferredInfo);
          // pthread_mutex_unlock(&gptml_emvco_context->read_info_update_mutex);
          osal_mutex_unlock(&gptml_emvco_context->read_info_update_mutex);
          LOG_EMVCO_TML_D("PN54X - Posting read message.....\n");
          tml_deferred_call(gptml_emvco_context->dw_callback_thread_id, &tMsg);
        }
      } else {
        LOG_EMVCO_TML_D("PN54X -gptml_emvco_context->p_dev_handle is NULL");
      }
    } else {
      LOG_EMVCO_TML_D("PN54X - read request NOT enabled");
      usleep(10 * 1000);
    }
  } /* End of While loop */

  return NULL;
}

/*******************************************************************************
**
** Function         tml_cleanup
**
** Description      Clears all handles opened during TML initialization
**
** Parameters       None
**
** Returns          None
**
*******************************************************************************/
void tml_cleanup(void) {
  if (NULL == gptml_emvco_context) {
    return;
  }
  if (NULL != gptml_emvco_context->p_dev_handle) {
    (void)i2c_nfcc_reset(gptml_emvco_context->p_dev_handle, MODE_POWER_OFF);
    gptml_emvco_context->b_thread_done = 0;
  }
  sem_destroy(&gptml_emvco_context->rx_semaphore);
  sem_destroy(&gptml_emvco_context->tx_semaphore);
  sem_destroy(&gptml_emvco_context->post_msg_semaphore);
  i2c_close(gptml_emvco_context->p_dev_handle);
  gptml_emvco_context->p_dev_handle = NULL;
  /* Clear memory allocated for storing Context variables */
  free((void *)gptml_emvco_context);
  /* Set the pointer to NULL to indicate De-Initialization */
  gptml_emvco_context = NULL;

  return;
}

/*******************************************************************************
**
** Function         tml_shutdown
**
** Description      Uninitializes TML layer and hardware interface
**
** Parameters       None
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS - TML configuration released
*successfully
**                  EMVCO_STATUS_INVALID_PARAMETER - at least one parameter is
**                                                invalid
**                  EMVCO_STATUS_FAILED - un-initialization failed (example:
*unable
**                                     to close interface)
**
*******************************************************************************/
EMVCO_STATUS tml_shutdown(void) {
  EMVCO_STATUS wShutdownStatus = EMVCO_STATUS_SUCCESS;

  /* Check whether TML is Initialized */
  if (NULL != gptml_emvco_context) {
    /* Reset thread variable to terminate the thread */
    gptml_emvco_context->b_thread_done = 0;
    usleep(1000);
    /* Clear All the resources allocated during initialization */
    sem_post(&gptml_emvco_context->rx_semaphore);
    usleep(1000);
    sem_post(&gptml_emvco_context->tx_semaphore);
    usleep(1000);
    sem_post(&gptml_emvco_context->post_msg_semaphore);
    usleep(1000);
    sem_post(&gptml_emvco_context->post_msg_semaphore);
    usleep(1000);
    osal_mutex_destroy(&gptml_emvco_context->read_info_update_mutex);
    if (0 !=
        osal_thread_join(gptml_emvco_context->reader_thread, (void **)NULL)) {
      LOG_EMVCO_TML_E("Fail to kill reader thread!");
    }
    if (0 !=
        osal_thread_join(gptml_emvco_context->writer_thread, (void **)NULL)) {
      LOG_EMVCO_TML_E("Fail to kill writer thread!");
    }
    LOG_EMVCO_TML_D("b_thread_done == 0");

  } else {
    wShutdownStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_NOT_INITIALISED);
  }

  return wShutdownStatus;
}

/*******************************************************************************
**
** Function         tml_write
**
** Description      Asynchronously writes given data block to hardware
**                  interface/driver. Enables writer thread if there are no
**                  write requests pending. Returns successfully once writer
**                  thread completes write operation. Notifies upper layer using
**                  callback mechanism.
**
**                  NOTE:
**                  * it is important to post a message with id
**                    TMLNFC_WRITE_MESSAGE to IntegrationThread after data
**                    has been written to PN54X
**                  * if CRC needs to be computed, then input buffer should be
**                    capable to store two more bytes apart from length of
**                    packet
**
** Parameters       p_buffer - data to be sent
**                  w_length - length of data buffer
**                  pTmlWriteComplete - pointer to the function to be invoked
**                                      upon completion
**                  p_context - context provided by upper layer
**
** Returns          NFC status:
**                  EMVCO_STATUS_PENDING - command is yet to be processed
**                  EMVCO_STATUS_INVALID_PARAMETER - at least one parameter is
**                                                invalid
**                  EMVCO_STATUS_BUSY - write request is already in progress
**
*******************************************************************************/
EMVCO_STATUS tml_write(uint8_t *p_buffer, uint16_t w_length,
                       transact_completion_callback_t pTmlWriteComplete,
                       void *p_context) {
  EMVCO_STATUS wWriteStatus;

  /* Check whether TML is Initialized */

  if (NULL != gptml_emvco_context) {
    if ((NULL != gptml_emvco_context->p_dev_handle) && (NULL != p_buffer) &&
        (TMLNFC_RESET_VALUE != w_length) && (NULL != pTmlWriteComplete)) {
      if (!gptml_emvco_context->t_write_info.b_thread_busy) {
        /* Setting the flag marks beginning of a Write Operation */
        gptml_emvco_context->t_write_info.b_thread_busy = true;
        /* Copy the buffer, length and Callback function,
           This shall be utilized while invoking the Callback function in thread
           */
        gptml_emvco_context->t_write_info.p_buffer = p_buffer;
        gptml_emvco_context->t_write_info.w_length = w_length;
        gptml_emvco_context->t_write_info.p_thread_callback = pTmlWriteComplete;
        gptml_emvco_context->t_write_info.p_context = p_context;

        wWriteStatus = EMVCO_STATUS_PENDING;
        // FIXME: If retry is going on. Stop the retry thread/timer
        if (enable_retrans == gptml_emvco_context->e_config) {
          /* Set retry count to default value */
          // FIXME: If the timer expired there, and meanwhile we have created
          // a new request. The expired timer will think that retry is still
          // ongoing.
          bCurrentRetryCount = gptml_emvco_context->b_retry_count;
          gptml_emvco_context->bWriteCbInvoked = false;
        }
        /* Set event to invoke Writer Thread */
        gptml_emvco_context->t_write_info.b_enable = 1;
        sem_post(&gptml_emvco_context->tx_semaphore);
      } else {
        wWriteStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_BUSY);
      }
    } else {
      wWriteStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_INVALID_PARAMETER);
    }
  } else {
    wWriteStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_NOT_INITIALISED);
  }

  return wWriteStatus;
}

/*******************************************************************************
**
** Function         tml_update_read_complete_callback
**
** Description      Updates the callback to be invoked after read completed
**
** Parameters       pTmlReadComplete - pointer to the function to be invoked
**                                     upon completion of read operation
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS - if TmlNfc context available
**                  EMVCO_STATUS_FAILED - otherwise
**
*******************************************************************************/
EMVCO_STATUS tml_update_read_complete_callback(
    transact_completion_callback_t pTmlReadComplete) {
  EMVCO_STATUS w_status = EMVCO_STATUS_FAILED;
  if ((NULL != gptml_emvco_context) && (NULL != pTmlReadComplete)) {
    gptml_emvco_context->t_read_info.p_thread_callback = pTmlReadComplete;
    w_status = EMVCO_STATUS_SUCCESS;
  }
  return w_status;
}

/*******************************************************************************
**
** Function         tml_read
**
** Description      Asynchronously reads data from the driver
**                  Number of bytes to be read and buffer are passed by upper
**                  layer.
**                  Enables reader thread if there are no read requests pending
**                  Returns successfully once read operation is completed
**                  Notifies upper layer using callback mechanism
**
** Parameters       p_buffer - location to send read data to the upper layer via
**                            callback
**                  w_length - length of read data buffer passed by upper layer
**                  pTmlReadComplete - pointer to the function to be invoked
**                                     upon completion of read operation
**                  p_context - context provided by upper layer
**
** Returns          NFC status:
**                  EMVCO_STATUS_PENDING - command is yet to be processed
**                  EMVCO_STATUS_INVALID_PARAMETER - at least one parameter is
**                                                invalid
**                  EMVCO_STATUS_BUSY - read request is already in progress
**
*******************************************************************************/
EMVCO_STATUS tml_read(uint8_t *p_buffer, uint16_t w_length,
                      transact_completion_callback_t pTmlReadComplete,
                      void *p_context) {
  EMVCO_STATUS wReadStatus;

  /* Check whether TML is Initialized */
  if (NULL != gptml_emvco_context) {
    if ((gptml_emvco_context->p_dev_handle != NULL) && (NULL != p_buffer) &&
        (TMLNFC_RESET_VALUE != w_length) && (NULL != pTmlReadComplete)) {
      if (!gptml_emvco_context->t_read_info.b_thread_busy) {
        osal_mutex_lock(&gptml_emvco_context->read_info_update_mutex);
        /* Setting the flag marks beginning of a Read Operation */
        gptml_emvco_context->t_read_info.b_thread_busy = true;
        /* Copy the buffer, length and Callback function,
           This shall be utilized while invoking the Callback function in thread
           */
        gptml_emvco_context->t_read_info.p_buffer = p_buffer;
        gptml_emvco_context->t_read_info.w_length = w_length;
        gptml_emvco_context->t_read_info.p_thread_callback = pTmlReadComplete;
        gptml_emvco_context->t_read_info.p_context = p_context;
        wReadStatus = EMVCO_STATUS_PENDING;

        /* Set event to invoke Reader Thread */
        gptml_emvco_context->t_read_info.b_enable = 1;
        osal_mutex_unlock(&gptml_emvco_context->read_info_update_mutex);
        sem_post(&gptml_emvco_context->rx_semaphore);
      } else {
        wReadStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_BUSY);
      }
    } else {
      wReadStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_INVALID_PARAMETER);
    }
  } else {
    wReadStatus = EMVCOSTVAL(CID_EMVCO_TML, EMVCO_STATUS_NOT_INITIALISED);
  }

  return wReadStatus;
}

/*******************************************************************************
**
** Function         tml_read_abort
**
** Description      Aborts pending read request (if any)
**
** Parameters       None
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS - ongoing read operation aborted
**                  EMVCO_STATUS_INVALID_PARAMETER - at least one parameter is
**                                                invalid
**                  EMVCO_STATUS_NOT_INITIALIZED - TML layer is not initialized
**                  EMVCO_STATUS_BOARD_COMMUNICATION_ERROR - unable to cancel
*read
**                                                        operation
**
*******************************************************************************/
EMVCO_STATUS tml_read_abort(void) {
  EMVCO_STATUS w_status = EMVCO_STATUS_INVALID_PARAMETER;
  gptml_emvco_context->t_read_info.b_enable = 0;

  /*Reset the flag to accept another Read Request */
  gptml_emvco_context->t_read_info.b_thread_busy = false;
  w_status = EMVCO_STATUS_SUCCESS;

  return w_status;
}

/*******************************************************************************
**
** Function         tml_write_abort
**
** Description      Aborts pending write request (if any)
**
** Parameters       None
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS - ongoing write operation aborted
**                  EMVCO_STATUS_INVALID_PARAMETER - at least one parameter is
**                                                invalid
**                  EMVCO_STATUS_NOT_INITIALIZED - TML layer is not initialized
**                  EMVCO_STATUS_BOARD_COMMUNICATION_ERROR - unable to cancel
*write
**                                                        operation
**
*******************************************************************************/
EMVCO_STATUS tml_write_abort(void) {
  EMVCO_STATUS w_status = EMVCO_STATUS_INVALID_PARAMETER;

  gptml_emvco_context->t_write_info.b_enable = 0;
  /* Stop if any retransmission is in progress */
  bCurrentRetryCount = 0;

  /* Reset the flag to accept another Write Request */
  gptml_emvco_context->t_write_info.b_thread_busy = false;
  w_status = EMVCO_STATUS_SUCCESS;

  return w_status;
}

/*******************************************************************************
**
** Function         tml_ioctl
**
** Description      Resets device when insisted by upper layer
**                  Number of bytes to be read and buffer are passed by upper
**                  layer
**                  Enables reader thread if there are no read requests pending
**                  Returns successfully once read operation is completed
**                  Notifies upper layer using callback mechanism
**
** Parameters       eControlCode       - control code for a specific operation
**
** Returns          NFC status:
**                  EMVCO_STATUS_SUCCESS  - ioctl command completed successfully
**                  EMVCO_STATUS_FAILED   - ioctl command request failed
**
*******************************************************************************/
EMVCO_STATUS tml_ioctl(emvco_control_code_t eControlCode) {
  EMVCO_STATUS w_status = EMVCO_STATUS_SUCCESS;

  if (NULL == gptml_emvco_context) {
    w_status = EMVCO_STATUS_FAILED;
  } else {
    switch (eControlCode) {
    case ResetDevice: {
      /*Reset PN54X*/
      i2c_nfcc_reset(gptml_emvco_context->p_dev_handle, MODE_POWER_ON);
      usleep(100 * 1000);
      i2c_nfcc_reset(gptml_emvco_context->p_dev_handle, MODE_POWER_OFF);
      usleep(100 * 1000);
      i2c_nfcc_reset(gptml_emvco_context->p_dev_handle, MODE_POWER_ON);
      break;
    }
    case EnableNormalMode: {
      /*Reset PN54X*/
      i2c_nfcc_reset(gptml_emvco_context->p_dev_handle, MODE_POWER_OFF);
      usleep(10 * 1000);
      i2c_nfcc_reset(gptml_emvco_context->p_dev_handle, MODE_POWER_ON);
      usleep(100 * 1000);
      break;
    }
    case RedLedOff: {
      i2c_led_control(gptml_emvco_context->p_dev_handle, RED_LED_OFF);
      break;
    }
    case RedLedOn: {
      i2c_led_control(gptml_emvco_context->p_dev_handle, RED_LED_ON);
      break;
    }
    case GreenLedOff: {
      i2c_led_control(gptml_emvco_context->p_dev_handle, GREEN_LED_OFF);
      break;
    }
    case GreenLedOn: {
      i2c_led_control(gptml_emvco_context->p_dev_handle, GREEN_LED_ON);
      break;
    }
    case NFCCModeSwitchOn: {
      i2c_nfcc_profile_switch(gptml_emvco_context->p_dev_handle, EMVCO_MODE);
      break;
    }
    case NFCCModeSwitchOff: {
      i2c_nfcc_profile_switch(gptml_emvco_context->p_dev_handle, NCI_MODE);
      break;
    }
    case SMCUModeSwitchOn: {
      i2c_smcu_profile_switch(gptml_emvco_context->p_dev_handle, EMVCO_MODE);
      break;
    }
    case SMCUModeSwitchOff: {
      i2c_smcu_profile_switch(gptml_emvco_context->p_dev_handle, NCI_MODE);
      break;
    }
    default: {
      w_status = EMVCO_STATUS_INVALID_PARAMETER;
      break;
    }
    }
  }

  return w_status;
}

/*******************************************************************************
**
** Function         tml_deferred_call
**
** Description      Posts message on upper layer thread
**                  upon successful read or write operation
**
** Parameters       dwThreadId  - id of the thread posting message
**                  ptWorkerMsg - message to be posted
**
** Returns          None
**
*******************************************************************************/
void tml_deferred_call(uintptr_t dwThreadId, lib_emvco_message_t *ptWorkerMsg) {
  intptr_t bPostStatus;
  UNUSED(dwThreadId);
  /* Post message on the user thread to invoke the callback function */
  sem_wait(&gptml_emvco_context->post_msg_semaphore);
  bPostStatus =
      osal_msg_snd(gptml_emvco_context->dw_callback_thread_id, ptWorkerMsg, 0);
  sem_post(&gptml_emvco_context->post_msg_semaphore);
}

/*******************************************************************************
**
** Function         tml_readDeferredCb
**
** Description      Read thread call back function
**
** Parameters       pParams - context provided by upper layer
**
** Returns          None
**
*******************************************************************************/
static void tml_readDeferredCb(void *pParams) {
  /* Transaction info buffer to be passed to Callback Function */
  osal_transact_info_t *pTransactionInfo = (osal_transact_info_t *)pParams;

  /* Reset the flag to accept another Read Request */
  gptml_emvco_context->t_read_info.b_thread_busy = false;
  gptml_emvco_context->t_read_info.p_thread_callback(
      gptml_emvco_context->t_read_info.p_context, pTransactionInfo);

  return;
}

/*******************************************************************************
**
** Function         tml_writeDeferredCb
**
** Description      Write thread call back function
**
** Parameters       pParams - context provided by upper layer
**
** Returns          None
**
*******************************************************************************/
static void tml_writeDeferredCb(void *pParams) {
  /* Transaction info buffer to be passed to Callback Function */
  osal_transact_info_t *pTransactionInfo = (osal_transact_info_t *)pParams;

  /* Reset the flag to accept another Write Request */
  gptml_emvco_context->t_write_info.b_thread_busy = false;
  gptml_emvco_context->t_write_info.p_thread_callback(
      gptml_emvco_context->t_write_info.p_context, pTransactionInfo);

  return;
}

/*******************************************************************************
**
** Function         tml_shutdown_cleanup
**
** Description      wrapper function  for shutdown  and cleanup of resources
**
** Parameters       None
**
** Returns          EMVCO_STATUS
**
*******************************************************************************/
EMVCO_STATUS tml_shutdown_cleanup() {
  EMVCO_STATUS wShutdownStatus = tml_shutdown();
  tml_cleanup();
  return wShutdownStatus;
}
