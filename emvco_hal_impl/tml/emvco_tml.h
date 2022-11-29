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
 * Transport Mapping Layer header files containing APIs related to initializing,
 * reading
 * and writing data into files provided by the driver interface.
 *
 * API listed here encompasses Transport Mapping Layer interfaces required to be
 * mapped
 * to different Interfaces and Platforms.
 *
 */

#ifndef _EMVCO_TML_H_
#define _EMVCO_TML_H_

#include <emvco_osal_common.h>

/*
 * Message posted by Reader thread upon
 * completion of requested operation
 */
#define TMLNFC_READ_MESSAGE (0xAA)

/*
 * Message posted by Writer thread upon
 * completion of requested operation
 */
#define TMLNFC_WRITE_MESSAGE (0x55)

/*
 * Value indicates to reset device
 */
#define TMLNFC_RESETDEVICE (0x00008001)

/*
***************************Globals,Structure and Enumeration ******************
*/

/*
 * Transaction (Tx/Rx) completion information structure of TML
 *
 * This structure holds the completion callback information of the
 * transaction passed from the TML layer to the Upper layer
 * along with the completion callback.
 *
 * The value of field w_status can be interpreted as:
 *
 *     - EMVCO_STATUS_SUCCESS                    Transaction performed
 * successfully.
 *     - EMVCO_STATUS_FAILED                     Failed to wait on Read/Write
 * operation.
 *     - EMVCO_STATUS_INSUFFICIENT_STORAGE       Not enough memory to store data
 * in case of read.
 *     - EMVCO_STATUS_BOARD_COMMUNICATION_ERROR  Failure to Read/Write from the
 * file or timeout.
 */

typedef struct osal_transact_info {
  EMVCO_STATUS w_status; /* Status of the Transaction Completion*/
  uint8_t *p_buff;       /* Response Data of the Transaction*/
  uint16_t w_length;     /* Data size of the Transaction*/
} osal_transact_info_t;  /* Instance of Transaction structure */

/*nfc state flags*/
enum emvco_state_flags {
  /*nfc in unknown state */
  EMVCO_STATE_UNKNOWN = 0,
  /*nfc booted in download mode */
  EMVCO_STATE_FW_DWL = 0x1,
  /*nfc booted in NCI mode */
  EMVCO_STATE_NCI = 0x2,
};
/*
 * TML transreceive completion callback to Upper Layer
 *
 * p_context - Context provided by upper layer
 * pInfo    - Transaction info. See osal_transact_info
 */
typedef void (*transact_completion_callback_t)(void *p_context,
                                               osal_transact_info_t *pInfo);

/*
 * TML Deferred callback interface structure invoked by upper layer
 *
 * This could be used for read/write operations
 *
 * dw_msg_posted_thread Message source identifier
 * pParams Parameters for the deferred call processing
 */
typedef void (*defer_func_pointer_t)(uint32_t dw_msg_posted_thread,
                                     void *pParams);

/*
 * Enum definition contains  supported ioctl control codes.
 *
 * tml_ioctl
 */
typedef enum {
  Invalid = 0,
  ResetDevice = TMLNFC_RESETDEVICE, /* Reset the device */
  EnableDownloadMode,               /* Do the hardware setting to enter into
                                                  download mode */
  EnableNormalMode, /* Hardware setting for normal mode of operation
                     */
  RedLedOff,        /* Turns off red led */
  RedLedOn,         /* Turns on red led */
  GreenLedOff,      /* Turns off green led */
  GreenLedOn,       /* Turns on green led */
  NFCCModeSwitchOn,
  NFCCModeSwitchOff,
  SMCUModeSwitchOn,
  SMCUModeSwitchOff,
} emvco_control_code_t; /* Control code for IOCTL call */

/*
 * Enable / Disable Re-Transmission of Packets
 *
 * tml_emvco_ConfigNciPktReTx
 */
typedef enum {
  enable_retrans = 0x00, /*Enable retransmission of Nci packet */
  disable_retrans = 0x01 /*Disable retransmission of Nci packet */
} config_retrans_t;      /* Configuration for Retransmission */

/*
 * Structure containing details related to read and write operations
 *
 */
typedef struct tml_read_write_info {
  volatile uint8_t b_enable; /*This flag shall decide whether to perform
                               Write/Read operation */
  uint8_t b_thread_busy;     /*Flag to indicate thread is busy on respective
                                operation */
  /* Transaction completion Callback function */
  transact_completion_callback_t p_thread_callback;
  void *p_context;          /*Context passed while invocation of operation */
  uint8_t *p_buffer;        /*Buffer passed while invocation of operation */
  uint16_t w_length;        /*Length of data read/written */
  EMVCO_STATUS work_status; /*Status of the transaction performed */
} tml_read_write_info_t;

/*
 *Base Context Structure containing members required for entire session
 */
typedef struct tml_emvco_context {
  pthread_t reader_thread; /*Handle to the thread which handles write and read
                             operations */
  pthread_t writer_thread;
  volatile uint8_t
      b_thread_done; /*Flag to decide whether to run or abort the thread */
  config_retrans_t e_config; /*Retransmission of Nci Packet during timeout */
  uint8_t b_retry_count;     /*Number of times retransmission shall happen */
  uint8_t bWriteCbInvoked; /* Indicates whether write callback is invoked during
                              retransmission */
  uint32_t dw_timer_id;    /* Timer used to retransmit nci packet */
  tml_read_write_info_t t_read_info;  /*Pointer to Reader Thread Structure */
  tml_read_write_info_t t_write_info; /*Pointer to Writer Thread Structure */
  void *p_dev_handle;                 /* Pointer to Device Handle */
  uintptr_t dw_callback_thread_id; /* Thread ID to which message to be posted */
  uint8_t b_enable_crc; /*Flag to validate/not CRC for input buffer */
  sem_t rx_semaphore;
  sem_t tx_semaphore;       /* Lock/Aquire txRx Semaphore */
  sem_t post_msg_semaphore; /* Semaphore to post message atomically by Reader &
                             writer thread */
  pthread_mutex_t
      read_info_update_mutex; /*Mutex to synchronize read Info update*/
} tml_emvco_context_t;

/*
 * TML Configuration exposed to upper layer.
 */
typedef struct tml_emvco_Config {
  /* Port name connected to PN54X
   *
   * Platform specific canonical device name to which PN54X is connected.
   *
   * e.g. On Linux based systems this would be /dev/PN54X
   */
  int8_t *p_dev_name;
  /* Callback Thread ID
   *
   * This is the thread ID on which the Reader & Writer thread posts message. */
  uintptr_t dw_get_msg_thread_id;
  /* Communication speed between DH and PN54X
   *
   * This is the baudrate of the bus for communication between DH and PN54X */
  uint32_t dw_baud_rate;
} tml_emvco_Config_t, *ptml_emvco_Config_t; /* pointer to tml_emvco_Config_t */

/*
 * TML Deferred Callback structure used to invoke Upper layer Callback function.
 */
typedef struct {
  /* Deferred callback function to be invoked */
  defer_func_pointer_t p_def_call;
  /* Source identifier
   *
   * Identifier of the source which posted the message
   */
  uint32_t dw_msg_posted_thread;
  /** Actual Message
   *
   * This is passed as a parameter passed to the deferred callback function
   * pDef_call. */
  void *pParams;
} tml_emvco_defer_msg_t; /* DeferMsg structure passed to User Thread */

typedef enum {
  I2C_FRAGMENATATION_DISABLED, /*i2c fragmentation_disabled           */
  I2C_FRAGMENTATION_ENABLED    /*i2c_fragmentation_enabled          */
} i2c_fragmentation_t;
/* Function declarations */
EMVCO_STATUS tml_init(ptml_emvco_Config_t pConfig);
EMVCO_STATUS tml_shutdown(void);
EMVCO_STATUS tml_shutdown_cleanup();
void tml_cleanup(void);
EMVCO_STATUS tml_write(uint8_t *p_buffer, uint16_t w_length,
                       transact_completion_callback_t pTmlWriteComplete,
                       void *p_context);
EMVCO_STATUS tml_read(uint8_t *p_buffer, uint16_t w_length,
                      transact_completion_callback_t pTmlReadComplete,
                      void *p_context);
EMVCO_STATUS tml_write_abort(void);
EMVCO_STATUS tml_read_abort(void);
EMVCO_STATUS tml_ioctl(emvco_control_code_t eControlCode);
EMVCO_STATUS tml_update_read_complete_callback(
    transact_completion_callback_t pTmlReadComplete);
void tml_deferred_call(uintptr_t dwThreadId, lib_emvco_message_t *ptWorkerMsg);
void set_fragmentation_enabled(i2c_fragmentation_t enable);
i2c_fragmentation_t get_fragmentation_enabled();

#endif /*  _EMVCO_TML_H_  */
