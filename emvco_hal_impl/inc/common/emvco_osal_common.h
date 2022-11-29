/*
 * Copyright 2010-2021,2022 NXP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  OSAL header files related to memory, debug, random, semaphore and mutex
 * functions.
 */

#ifndef _EMVCO_OSAL_COMMON_H_
#define _EMVCO_OSAL_COMMON_H_

/*
************************* Include Files ****************************************
*/

#include <emvco_comp_id.h>
#include <emvco_status.h>
#include <emvco_types.h>
#include <osal_timer.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef FW_LIB_ROOT_DIR
#if (defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64))
#define FW_LIB_ROOT_DIR "/vendor/lib64/"
#else
#define FW_LIB_ROOT_DIR "/vendor/lib/"
#endif
#endif

/*
 *  information to configure OSAL
 */
typedef struct osal_emvco_config {
  uint8_t *p_log_file;             /* Log File Name*/
  uintptr_t dw_callback_thread_id; /* Client ID to which message is posted */
} osal_emvco_config_t,
    *posal_emvco_config_t /* Pointer to #osal_emvco_config_t */;

/*
 * Deferred call declaration.
 * This type of API is called from ClientApplication (main thread) to notify
 * specific callback.
 */
typedef void (*osal_emvco_defer_func_pointer)(void *);

/*
 * Deferred message specific info declaration.
 */
typedef struct osal_emvco_defer_call_info {
  osal_emvco_defer_func_pointer pDeferedCall; /* pointer to Deferred callback */
  void *pParam; /* contains timer message specific details*/
} osal_emvco_defer_call_info_t;

/*
 * States in which a OSAL timer exist.
 */
typedef enum {
  eTimerIdle = 0,            /* Indicates Initial state of timer */
  eTimerRunning = 1,         /* Indicate timer state when started */
  eTimerStopped = 2          /* Indicates timer state when stopped */
} osal_emvco_timer_states_t; /* Variable representing State of timer */

/*
 **Timer Handle structure containing details of a timer.
 */
typedef struct osal_emvco_timer_handle {
  uint32_t timer_id;    /* ID of the timer */
  timer_t timer_handle; /* Handle of the timer */
  /* Timer callback function to be invoked */
  p_osal_emvco_timer_callback_t Application_callback;
  void *p_context; /* Parameter to be passed to the callback function */
  osal_emvco_timer_states_t eState; /* Timer states */
  /* Osal Timer message posted on User Thread */
  lib_emvco_message_t tOsalMessage;
  /* Deferred Call structure to Invoke Callback function */
  osal_emvco_defer_call_info_t tDeferedCallInfo;
  /* Variables for Structure Instance and Structure Ptr */
} osal_emvco_timer_handle_t, *posal_emvco_timer_handle_t;

#endif /*  _EMVCO_OSAL_COMMON_H_  */
