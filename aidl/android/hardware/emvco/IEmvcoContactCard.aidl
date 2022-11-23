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

import android.hardware.emvco.IEmvcoClientCallback;

@VintfStability
interface IEmvcoContactCard {
    /**
    *
    * @brief Register EMVCo callback function to receive the events from a listener device.
    *
    * @note This function is must to call before invoking any other api.
    *
    * @param[in]  *in_clientCallback has EMVCo client HAL callback
    * @param[in]  *in_aidl_return indicates register status in return to caller
    *
    * @return ::ndk::ScopedAStatus indicates doRegisterEMVCoEventListener request processed by EMVCo HAL successfully or not
    */
    boolean registerEMVCoEventListener(in IEmvcoClientCallback clientCallback);
}
