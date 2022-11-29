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
#ifndef _EMVCO_UTILS_H_
#define _EMVCO_UTILS_H_

#include "osal_thread.h"
#include <assert.h>
#include <emvco_status.h>
#include <pthread.h>
#include <semaphore.h>
/********************* Definitions and structures *****************************/

/* List structures */
struct list_node {
  void *p_data;
  struct list_node *p_next;
};

struct list_head {
  struct list_node *p_first;
  pthread_mutex_t mutex;
};

/* Semaphore handling structure */
typedef struct nci_hal_sem_t {
  /* Semaphore used to wait for callback */
  sem_t sem;

  /* Used to store the status sent by the callback */
  EMVCO_STATUS status;

  /* Used to provide a local context to the callback */
  void *p_context;

} nci_hal_sem;

/* Semaphore helper macros */
#define SEM_WAIT(cb_data) osal_sem_wait(&((cb_data).sem))
#define SEM_POST(p_cb_data) osal_sem_post(&((p_cb_data)->sem))

/* Semaphore and mutex monitor */
typedef struct phNxpNciHal_Monitor {
  /* Mutex protecting native library against reentrance */
  pthread_mutex_t reentrance_mutex;

  /* Mutex protecting native library against concurrency */
  pthread_mutex_t concurrency_mutex;

  /* List used to track pending semaphores waiting for callback */
  struct list_head sem_list;

} phNxpNciHal_Monitor_t;

/************************ Exposed functions ***********************************/
/* List functions */
int listInit(struct list_head *pList);
int listDestroy(struct list_head *pList);
int listAdd(struct list_head *pList, void *p_data);
int listRemove(struct list_head *pList, void *p_data);
int listGetAndRemoveNext(struct list_head *pList, void **pp_data);
void listDump(struct list_head *pList);

/* NXP NCI HAL utility functions */
phNxpNciHal_Monitor_t *init_monitor(void);
void cleanup_monitor(void);
phNxpNciHal_Monitor_t *get_monitor(void);
EMVCO_STATUS init_cb_data(nci_hal_sem *pCallbackData, void *p_context);
void cleanup_cb_data(nci_hal_sem *pCallbackData);
void releaseall_cb_data(void);
void print_packet(const char *pString, const uint8_t *p_data, uint16_t len);
void emergency_recovery(void);

/* Lock unlock helper macros */
/* Lock unlock helper macros */
#define REENTRANCE_LOCK()                                                      \
  if (get_monitor())                                                           \
  pthread_mutex_lock(&get_monitor()->reentrance_mutex)
#define REENTRANCE_UNLOCK()                                                    \
  if (get_monitor())                                                           \
  pthread_mutex_unlock(&get_monitor()->reentrance_mutex)
#define CONCURRENCY_LOCK()                                                     \
  if (get_monitor())                                                           \
  pthread_mutex_lock(&get_monitor()->concurrency_mutex)
#define CONCURRENCY_UNLOCK()                                                   \
  if (get_monitor())                                                           \
  pthread_mutex_unlock(&get_monitor()->concurrency_mutex)

#endif /* _EMVCO_UTILS_H_ */
