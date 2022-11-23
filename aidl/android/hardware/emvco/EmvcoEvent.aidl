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

package android.hardware.emvco;

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

/**
 * @brief Event codes to be handled by Application
 *
 * The EMVCo Hal notifies application with below event codes.
 */
@VintfStability
@Backing(type="int")
enum EmvcoEvent {
    /** Event to indicate EMVCo HAL open complete state*/
    EMVCO_HAL_OPEN_CPLT_EVT          = 0,
    /** Event to indicate EMVCo HAL close complete state*/
    EMVCO_HAL_CLOSE_CPLT_EVT         = 1,
    /** Event to indicate the start of EMVCo mode*/
    EMVCO_MODE_START_EVT             = 2,
    /** Event to indicate EMVCo polling activated state*/
    EMVCO_POLL_ACTIVATE_EVT          = 3,
    /** Event to indicate the stop of EMVCo mode*/
    EMVCO_MODE_STOP_EVT              = 4,
}
/** @}*/