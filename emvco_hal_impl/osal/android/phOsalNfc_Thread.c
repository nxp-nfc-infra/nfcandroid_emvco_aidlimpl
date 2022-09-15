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

#include "phOsalNfc_Thread.h"
#include <phNxpLog.h>
#include <semaphore.h>

NFCSTATUS phOsalNfc_Thread_create(pthread_t *thread, const pthread_attr_t *attr,
                                  void *(*start_routine)(void *), void *arg) {
  NXPLOG_TML_D("%s\n", __func__);
  return pthread_create(thread, attr, start_routine, arg);
}

NFCSTATUS phOsalNfc_Thread_lock_mutex(void *hMutex) {
  NXPLOG_TML_D("%s\n", __func__);
  return pthread_mutex_lock(hMutex);
}
NFCSTATUS phOsalNfc_Thread_unlock_mutex(void *hMutex) {
  NXPLOG_TML_D("%s\n", __func__);
  return pthread_mutex_unlock(hMutex);
}
NFCSTATUS phOsalNfc_create_mutex(void *hMutex) {
  NXPLOG_TML_D("%s\n", __func__);
  return pthread_mutex_init(hMutex, NULL);
}
NFCSTATUS phOsalNfc_Thread_delete_mutex(void *hMutex) {
  NXPLOG_TML_D("%s\n", __func__);
  return pthread_mutex_destroy(hMutex);
}
NFCSTATUS phOsalNfc_Thread_join(pthread_t thread, void *retval) {
  NXPLOG_TML_D("%s\n", __func__);
  return pthread_join(thread, retval);
}

int phOsalNfc_Thread_sem_wait(sem_t *sem) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_wait(sem);
}

int phOsalNfc_Thread_sem_timedwait(sem_t *sem,
                                   const struct timespec *abs_timeout) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_timedwait(sem, abs_timeout);
}

int phOsalNfc_Thread_sem_post(sem_t *sem) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_post(sem);
}

int phOsalNfc_Thread_sem_destroy(sem_t *sem) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_destroy(sem);
}

int phOsalNfc_Thread_sem_init(sem_t *sem, int pshared, unsigned int value) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_init(sem, pshared, value);
}

int phOsalNfc_Thread_sem_getvalue(sem_t *sem, int *sval) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_getvalue(sem, sval);
}

int phOsalNfc_Thread_sem_timedwait_monotonic_np(sem_t *__sem,
                                                const struct timespec *__ts) {
  NXPLOG_TML_D("%s\n", __func__);
  return sem_timedwait_monotonic_np(__sem, __ts);
}