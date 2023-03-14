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

package android.hardware.emvco;

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

/**
 * @brief EMVCo Hal return status code as below
 */
@VintfStability
@Backing(type="int")
enum EmvcoStatus {
    /**
     * Indicates success status.
     */
    EMVCO_STATUS_OK = 0,

    /**
     * Indicates invalid input parameter status.
     */
    EMVCO_STATUS_INVALID_PARAMETER = 1,

    /**
     * Indicates action requested on invalid state
     */
    EMVCO_STATUS_INVALID_STATE = 11,

    /**
     * Indicates feature not supported yet.
     */
    EMVCO_STATUS_FEATURE_NOT_SUPPORTED = 33,

    /**
     * Indicates failure status.
     */
    EMVCO_STATUS_FAILED = 255,
}

/** @}*/