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

#ifndef _OSAL_THREAD_H_
#define _OSAL_THREAD_H_
/** \addtogroup EMVCO_STACK_OSAL_API_INTERFACE
 *  @{
 */
#include <emvco_types.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

/**
 * \brief Interface to Thread and syncronization.
 *
 * Thread and syncronization implementation for Android
 *
 */
typedef struct osal_semaphore {
  sem_t object_handle; /**<Handle of the semaphore object */
} osal_semaphore_t;    /**< Variable for Structure Instance*/

/**
 *
 * @brief                   This function creates a thread in the underlying
 system.
 *                          To delete the created thread use the
 phOsalEMVCo_Thread_Delete
 *                          function.
 *
 *
 * @param[in] hThread    The Thread handle: The caller has to prepare a void
 *                           pointer
 *                           that need not to be initialized.
 *                           The value (content) of the pointer is set by the
 *                           function.
 * @param[in] attr            It points to a pthread_attr_t structure whose
                             contents are used at thread creation time to
 *                           determine attributes for the new thread
 * @param[in] pThreadFunction Pointer to a function within the
 *                           implementation that shall be called by the Thread
 *                           procedure. This represents the Thread main
 *                           function.
 *                           When this function exits the thread exits.
 * @param[in] pParam          A pointer to a user-defined location the thread
 *                           function receives.
 *
 * @return #EMVCOSTATUS_SUCCESS                    The operation was successful.
 * @return #EMVCOSTATUS_INSUFFICIENT_RESOURCES     At least one parameter value
 is
 *                                               invalid.
 * @return #PH_OSALEMVCO_THREAD_CREATION_ERROR     A new Thread could not be
 *                                               created due to system error.
 * @return #EMVCOSTATUS_NOT_INITIALISED            Osal Module is not
 Initialized.
 *
 */
EMVCO_STATUS osal_thread_create(pthread_t *hThread, const pthread_attr_t *attr,
                                void *(*pThreadFunction)(void *), void *pParam);
/**
 *
 * @brief                               This function locks a mutex used for
 * handling critical section of code. However the function blocks till the Mutex
 * is available to be occupied.
 *
 * @param[in] hMutex                    The handle of the Mutex.
 *
 * @return #EMVCOSTATUS_SUCCESS            The operation was successful.
 * @return #EMVCOSTATUS_INVALID_PARAMETER  Parameter passed to the function is
 * not Correct.
 * @return #PH_OSALEMVCO_MUTEX_LOCK_ERROR  The mutex cannot be locked due
 *                                       to a system error or invalid handle.
 * @return #EMVCOSTATUS_NOT_INITIALISED    OsalModule is not Initialized.
 *
 */
EMVCO_STATUS osal_mutex_lock(void *hMutex);
/**
 *
 * @brief                              This function unlocks a mutex after
 * updating critical section of code.
 *
 * @param[in] hMutex        The handle of the Mutex.
 *
 * @return #EMVCOSTATUS_SUCCESS                The operation was successful.
 * @return #EMVCOSTATUS_INVALID_PARAMETER      Parameter passed to the function
 * is not Correct.
 * @return #PH_OSALEMVCO_MUTEX_UNLOCK_ERROR    The mutex cannot be
 *                                           locked due to a system error or
 *                                           invalid handle.
 * @return #EMVCOSTATUS_NOT_INITIALISED        Osal Module is not Initialized.
 *
 */
EMVCO_STATUS osal_mutex_unlock(void *hMutex);
/**
 *
 * @brief                    This function creates a Mutex in the underlying
 * system.
 *
 * @param[in] hMutex        The handle: The caller has to prepare a void
 *                           pointer where the handle of mutex shall be
 *                           returned.
 *
 * @return #EMVCOSTATUS_SUCCESS                    The operation was successful.
 * @return #EMVCOSTATUS_INVALID_PARAMETER          Parameter passed to the
 *                                               function is not Correct.
 * @return #EMVCOSTATUS_INSUFFICIENT_RESOURCES     All Mutexes are occupied by
 * other threads.
 * @return #PH_OSALEMVCO_MUTEX_CREATION_ERROR      A new mutex could not be
 * created due to system error.
 * @return #EMVCOSTATUS_NOT_INITIALISED            Osal Module is not
 * Initialized.
 *
 */
EMVCO_STATUS osal_mutex_init(void *hMutex);
/**
 *
 * @brief                                    This function deletes a Mutex in
 * the underlying system.
 *
 * @param[in] hMutex                         The handle of the Mutex.
 *
 * @return #EMVCOSTATUS_SUCCESS                The operation was successful.
 * @return #PH_OSALEMVCO_MUTEX_DELETE_ERROR    The mutex cannot be deleted due
 * to a system error or invalid handle.
 * @return #EMVCOSTATUS_NOT_INITIALISED         Osal Module is not Initialized.
 *
 */
EMVCO_STATUS osal_mutex_destroy(void *hMutex);
/**
 *
 * @brief                                    The pthread_join() function waits
 * for the thread specified by thread to terminate
 * @param[in] hMutex                         The handle of the Mutex.
 *
 * @return #EMVCOSTATUS_SUCCESS                The operation was successful.
 * @return #EDEADLK                          A deadlock was detected.
 * @return #EINVAL                           Thread is not a joinable thread.
 * @return #EINVAL                           Another thread is alredy waiting to
 *                                           join with this thread.
 * @return #ESRCH                            No thread with the ID thread
 *                                           could be found.
 */
EMVCO_STATUS osal_thread_join(pthread_t thread, void *retval);
/**
 *
 * @brief  lock a semaphore
 *
 * @param[in] sem_t               Semaphore.
 *
 * @return #0                     on success
 * @return #EAGAIN                The operation could not be performed
 *                                without blocking
 * @return #EINTR                 The call was interrupted by
 *                                a signal handler.
 * @return #EINVAL                sem is not a valid semaphore..
 */
int osal_sem_wait(sem_t *sem);
/**
 *
 * @brief lock a semaphore with timeout
 *
 * @param[in] sem_t                          Semaphore.
 *
 * @return #0                on success
 * @return #EAGAIN           The operation could not be performed
 *                           without blocking
 * @return #EINTR            The call was interrupted by a signal handler.
 * @return #EINVAL           sem is not a valid semaphore..
 * @return #EINVAL           The value of abs_timeout.tv_nsecs is
 *                           less than 0, or greater than or
 *                           equal to 1000 million
 * @return #ETIMEDOUT        The call timed out before the semaphore
 *                           could be locked.
 */
int osal_sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

/**
 *
 * @brief unlock a semaphore
 *
 * @param[in] sem_t                          Semaphore.
 *
 * @return #0                on success
 * @return #EINVAL           sem is not a valid semaphore..
 * @return #EOVERFLOW        The maximum allowable value for a semaphore would
 *                           be exceeded
 */
int osal_sem_post(sem_t *sem);

/**
 *
 * @brief  destroys an semaphore
 *
 * @param[in] sem_t          Semaphore.
 *
 * @return #0                on success
 * @return #EINVAL           sem is not a valid semaphore..
 *
 */
int osal_sem_destroy(sem_t *sem);

/**
 *
 * @brief initialize an unnamed semaphore
 *
 * @param[in] sem_t                          Semaphore.
 *
 * @return #0                on success
 * @return #EINVAL           value exceeds SEM_VALUE_MAX
 * @return #ENOSYS           pshared is nonzero, but the system does not support
 *                           process-shared semaphores
 *
 */
int osal_sem_init(sem_t *sem, int pshared, unsigned int value);

/**
 *
 * @brief get the value of a semaphore
 *
 * @param[in] sem_t          Semaphore.
 *
 * @return #0                on success
 * @return #EINVAL           sem is not a valid semaphore
 *
 */
int osal_sem_getvalue(sem_t *sem, int *sval);

/**
 *
 * @brief lock a semaphore with monotonic timeout
 *
 * @param[in] sem_t                          Semaphore.
 *
 * @return #0             on success
 * @return #EAGAIN        The operation could not be performed
 *                        without blocking
 *
 * @return #EINTR         The call was interrupted by
 *                        a signal handler.
 * @return #EINVAL        sem is not a valid semaphore..
 * @return #EINVAL        The value of abs_timeout.tv_nsecs is
 *                        less than 0, or greater than or
 *                        equal to 1000 million
 * @return #ETIMEDOUT     The call timed out before the semaphore
 *                        could be locked.
 */
int osal_sem_timedwait_monotonic_np(sem_t *__sem, const struct timespec *__ts)
    __INTRODUCED_IN(28);

/** @}*/
#endif /* _OSAL_THREAD_H_*/