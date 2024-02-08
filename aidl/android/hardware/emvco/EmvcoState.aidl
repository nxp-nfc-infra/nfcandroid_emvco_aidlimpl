/******************************************************************************
 *
 *  Copyright 2023 NXP
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
 * @brief This provides the status of the EMVCo mode in CT Mode
 *
 */
@VintfStability
@Backing(type="int")
enum EmvcoState {
    /**
     * ON event to notify that EMVCo mode is turned ON
     *
     */
    ON                       = 0,
    /**
     * CL_TYPE_A_CARD_DETECTED event to notify that TYPE A/B/T3T Card detected over CL
     */
    CL_TYPE_A_B_T3T_CARD_DETECTED         = 1,
    /**
     * CL_TYPE_A_B_T3T_CARD_REMOVED event to notify that TYPE A/B/T3T removed from RF field
     */
    CL_TYPE_A_B_T3T_CARD_REMOVED         = 2,
    /**
     * OFF event to notify that EMVCo mode is turned ON
     */
    OFF                      = 3,

}
/** @}*/