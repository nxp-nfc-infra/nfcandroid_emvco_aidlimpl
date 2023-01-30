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
 * @brief Pofile types to be handled by EMVCo HAL
 *
 * The EMVCo Hal uses this event to determine the current active profile.
 */
@VintfStability
@Backing(type="int")
enum DiscoveryMode {
    /**
     * NFC event to notify NFC as current active state to EMVCo HAL
     *
     */
    NFC                       = 1,
    /**
     * EMVCO event to notify NFC as current active state to EMVCo HAL
     */
    EMVCO                = 2,
    /**
     * UN_KNOWN event to notify UN_KNOWN as current active state to EMVCo HAL
     */
    UN_KNOWN                       = 3,

}
/** @}*/
