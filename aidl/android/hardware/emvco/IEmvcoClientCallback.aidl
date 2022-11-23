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

import android.hardware.emvco.EmvcoEvent;
import android.hardware.emvco.EmvcoStatus;

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

/**
 * @brief Provides a callback functionality to App.
 */
@VintfStability
interface IEmvcoClientCallback {
   /**
    * EMVCo Data Callback Function
    */
    oneway void sendData(in byte[] data);
    /**
    * EMVCo Event Callback Function
    */
    oneway void sendEvent(in EmvcoEvent event, in EmvcoStatus status);
}
/** @}*/