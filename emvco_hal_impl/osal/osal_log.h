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
#ifndef _OSAL_LOG_H_
#define _OSAL_LOG_H_

#include <log/log.h>

/**
 * @brief Prints logging information
 *
 * @param[in] prio priority of the log level
 * @param[in] tag TAG name
 * @param[in] ... additional information of the log
 */
#define OSAL_LOG_PRI(prio, tag, ...)                                           \
  { LOG_PRI(prio, tag, __VA_ARGS__); }

/**
 * @brief writes error logging information
 *
 * @param[in] tag TAG name
 * @param[in] subTag asub tag name
 */
#define __osal_log_error_write(tag, subTag)                                    \
  __android_log_error_write(tag, subTag, -1, NULL, 0)
#endif /* _OSAL_LOG_H_*/