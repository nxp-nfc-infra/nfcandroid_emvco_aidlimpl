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
 * @brief config types allowed to be set through EMVCO HAL
 *
 * The EMVCo Hal uses this config type as key to set the config value.
 */
@VintfStability
@Backing(type="int")
enum ConfigType {
   /**
    * POLL_PROFILE_SEL - Discovery profile selection in poll mode
    */
   POLL_PROFILE_SEL = 0,
}

/** @} */