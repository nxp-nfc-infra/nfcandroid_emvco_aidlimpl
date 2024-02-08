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
 * @brief This provides the status of the smart card connected over TDA
 *
 */
@VintfStability
@Backing(type="int")
enum EmvcoTDAStatus {
    /**
     * ENABLED event to notify that smart card connected over TDA is in enabled state
     *
     */
    CONNECTED_ENABLED                       = 0,
    /**
     * DISABLED event to notify that smart card connected over TDA is in disabled state
     */
    CONNECTED_DISABLED                = 1,
    /**
     * UN_RESPONSIVE event to notify that smart card connected over TDA is in un responsive state
     */
    UN_RESPONSIVE_CARD_NOT_INSERTED                       = 2,
    /**
     * CC_STATUS_RFU Reserved for future purpose
     */
    CC_STATUS_RFU              = 3,

}
/** @}*/