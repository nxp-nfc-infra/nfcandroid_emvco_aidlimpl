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

#include "osal_thread.h"
#include <emvco_log.h>
#include <semaphore.h>

EMVCO_STATUS osal_thread_create(pthread_t *thread, const pthread_attr_t *attr,
                                void *(*start_routine)(void *), void *arg) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return pthread_create(thread, attr, start_routine, arg);
}

EMVCO_STATUS osal_mutex_lock(void *hMutex) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return pthread_mutex_lock(hMutex);
}
EMVCO_STATUS osal_mutex_unlock(void *hMutex) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return pthread_mutex_unlock(hMutex);
}
EMVCO_STATUS osal_mutex_init(void *hMutex) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return pthread_mutex_init(hMutex, NULL);
}
EMVCO_STATUS osal_mutex_destroy(void *hMutex) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return pthread_mutex_destroy(hMutex);
}
EMVCO_STATUS osal_thread_join(pthread_t thread, void *retval) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return pthread_join(thread, retval);
}

int osal_sem_wait(sem_t *sem) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_wait(sem);
}

int osal_sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_timedwait(sem, abs_timeout);
}

int osal_sem_post(sem_t *sem) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_post(sem);
}

int osal_sem_destroy(sem_t *sem) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_destroy(sem);
}

int osal_sem_init(sem_t *sem, int pshared, unsigned int value) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_init(sem, pshared, value);
}

int osal_sem_getvalue(sem_t *sem, int *sval) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_getvalue(sem, sval);
}

int osal_sem_timedwait_monotonic_np(sem_t *__sem, const struct timespec *__ts) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return sem_timedwait_monotonic_np(__sem, __ts);
}