/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
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

#ifndef _EMVCO_COMMON_H_
#define _EMVCO_COMMON_H_

#include "emvco_status.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  boolean data type
 */
typedef uint8_t bool_t;

/**
 * @brief  EMVCo status specifies EMVCO_STATUS_SUCCESS or EMVCO_STATUS_FAILED
 */
typedef uint16_t EMVCO_STATUS; /* Return values */

/**
 * @brief emvco_event_t EMVCo data event
 *
 */
typedef uint8_t emvco_event_t;
/**
 * @brief emvco_event_t EMVCo statuc event
 *
 */
typedef uint8_t emvco_status_t;
/**
 * @brief discovery_mode_t stores the current active profile. NFC/EMVCO/UNKNOWN
 *
 */
typedef enum { NFC = 1, EMVCO, UNKNOWN } discovery_mode_t;

discovery_mode_t discovery_mode;
/**
 * @brief config_type_t defines the supported config key types
 *        can be configured through setConfig API
 *
 */
typedef enum { POLL_PROFILE_SEL = 0 } config_type_t;
/**
 * @brief deactivation_type_t defines the supported deactivation
 *        types which will be used along with deactivation command.
 *
 */
typedef enum { IDLE = 0, DISCOVER = 3 } deactivation_type_t;

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
#endif /* _EMVCO_COMMON_H_ */
