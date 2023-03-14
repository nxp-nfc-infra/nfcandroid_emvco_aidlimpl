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
 * @brief LED types to be handled by EMVCo HAL
 *
 * The EMVCo Hal uses this event to control the Red and Green LED state
 */
@VintfStability
@Backing(type="int")
enum LedControl {
    /**
     * Turns off GREEN LED
     */
    RED_LED_OFF                  = 0,

    /**
     * Turns on RED LED
     *
     */
    RED_LED_ON                   = 1,
    /**
     * Turns off GREEN LED
     */
    GREEN_LED_OFF                = 2,
    /**
     * Turns on GREEN LED
     *
     */
    GREEN_LED_ON                 = 3,
}
/** @}*/