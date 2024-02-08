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
 * @brief This provides the supported protocol information of the smart card connected over TDA
 *
 */
@VintfStability
@Backing(type="int")
enum Protocols {
    /**
     * APDU event to notify that smart card connected over TDA supports APDU protocol
     *
     */
    APDU                       = 0,
    /**
     * RFU_01 event reserved for future purpose
     */
    RFU_01                = 1,
    /**
     * T3T event to notify that smart card connected over TDA supports T3T protocol
     */
    T3T                       = 2,
    /**
     * TRANPARENT event to notify that smart card connected over TDA supports Transparent protocol
     *
     */
    TRANSPARENT                       = 3,
    /**
     * PROPRIETARY event to notify that smart card connected over TDA supports proprietary protocol
     */
    PROPRIETARY                = 4,
    /**
     * RFU_FF event reserved for future purpose
     */
    RFU_FF                = 5,

}
/** @}*/