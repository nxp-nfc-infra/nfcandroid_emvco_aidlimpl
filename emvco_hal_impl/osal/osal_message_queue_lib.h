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

#ifndef _OSAL_MESSAGE_QUEUE_LIB_H_
#define _OSAL_MESSAGE_QUEUE_LIB_H_
/** \addtogroup EMVCO_STACK_OSAL_API_INTERFACE
 *  @brief      Interface for message queue, log, dynamic memory management,
 * thread, timer and transport mapping layer implementation in Android
 */
#include <emvco_types.h>
#include <linux/ipc.h>

/**
 *
 * @brief            Allocates message queue
 *
 * @param[in]       Ignored, included only for Linux queue API compatibility
 *
 * @return           (int) value of pQueue if successful
 *                  -1, if failed to allocate memory or to init mutex
 *
 */
intptr_t osal_msg_get(key_t key, int msgflg);

/**
 *
 * @brief       Releases message queue
 *
 * @param[in]        msqid - message queue handle
 *
 * @return           None
 *
 */
void osal_msg_release(intptr_t msqid);

/**
 *
 * @brief      Destroys message queue
 *
 * @param[in]       msqid - message queue handle
 *                  cmd, buf - ignored, included only for Linux queue API
 *                  compatibility
 *
 * @return          0,  if successful
 *                  -1, if invalid handle is passed
 *
 */
int osal_msg_ctl(intptr_t msqid, int cmd, void *buf);

/**
 *
 * @brief      Sends a message to the queue. The message will be added at
 *                  the end of the queue as appropriate for FIFO policy
 *
 * @param[in]       msqid  - message queue handle
 *                  msgp   - message to be sent
 *                  msgsz  - message size
 *                  msgflg - ignored
 *
 * @return          0,  if successful
 *                  -1, if invalid parameter passed or failed to allocate memory
 *
 */
intptr_t osal_msg_snd(intptr_t msqid, lib_emvco_message_t *msg, int msgflg);

/**
 *
 * @brief       Gets the oldest message from the queue.
 *                  If the queue is empty the function waits (blocks on a mutex)
 *                  until a message is posted to the queue with
 * phDal4EMVCo_msgsnd
 *
 * @param[in]        msqid  - message queue handle
 *                  msgp   - message to be received
 *                  msgsz  - message size
 *                  msgtyp - ignored
 *                  msgflg - ignored
 *
 * @return           0,  if successful
 *                  -1, if invalid parameter passed
 *
 */
int osal_msg_rcv(intptr_t msqid, lib_emvco_message_t *msg, long msgtyp,
                 int msgflg);

/** @}*/
#endif /*  _OSAL_MESSAGE_QUEUE_LIB_H_  */
