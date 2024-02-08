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

import android.hardware.emvco.EmvcoTDAStatus;
/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

/**
 * @brief Provides a contact card callback functionality to App.
 *        dynamic insertion and removal card status will be received
 *        as part of this callback
 */
@VintfStability
interface IEmvcoTDACallback {
    /**
     * Used to inform the client about changes in the state of the TDA
     * 
     * @param tdaID unique ID of the TDA
     * @param tdaStaus indicates the current status of the smart card over TDA
     * @param debugReason provides additional data why there was a change in state
     *               ex. initialization error, smart card removed etc
     *               This is used only for debugging purpose to understand
     *               in-field issues.
     */
    oneway void onStateChange(in byte tdaID, in EmvcoTDAStatus tdaStaus, in String debugReason);
}
/** @}*/