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
 * DAL independent message queue implementation for Android
 */

#ifndef _OSAL_MESSAGE_QUEUE_LIB_H_
#define _OSAL_MESSAGE_QUEUE_LIB_H_
#include <emvco_types.h>
#include <linux/ipc.h>

intptr_t osal_msg_get(key_t key, int msgflg);
void osal_msg_release(intptr_t msqid);
int osal_msg_ctl(intptr_t msqid, int cmd, void *buf);
intptr_t osal_msg_snd(intptr_t msqid, lib_emvco_message_t *msg, int msgflg);
int osal_msg_rcv(intptr_t msqid, lib_emvco_message_t *msg, long msgtyp,
                 int msgflg);
#endif /*  _OSAL_MESSAGE_QUEUE_LIB_H_  */
