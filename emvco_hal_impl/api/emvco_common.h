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

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMVCO_STATUS_SUCCESS (0x0000)
#define EMVCO_STATUS_FAILED (0x00FF)

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

typedef enum { NFC = 1, EMVCO, UNKNOWN } discovery_mode_t;

discovery_mode_t discovery_mode;

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
#endif /* _EMVCO_COMMON_H_ */
