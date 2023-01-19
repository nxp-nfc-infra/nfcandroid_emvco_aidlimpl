/******************************************************************************
 *
 *  Copyright 2022,2023 NXP
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
import android.hardware.emvco.INfcStateChangeRequestCallback;
import android.hardware.emvco.NfcState;

@VintfStability
interface IEmvcoProfileDiscovery {
    /**
    *
    * @brief Register EMVCo callback function to receive the events from a listener device.
    *
    * @note This function is must to call before invoking any other api.
    *
    * @param[in]  *in_clientCallback has EMVCo client HAL callback
    * @param[in]  *in_aidl_return indicates register status in return to caller
    *
    * @return boolean returns true, if success and returns false, if failed to register
    */
    boolean registerEMVCoEventListener(in IEmvcoClientCallback emvcoClientCallback);

    /**
    * @brief starts the EMVCo mode with the Device-Controller.
    *
    * Once the @ref open "Application Data Channel is "
    * established, the Application may send start the EMVCo mode with the
    * Device-Controller.
    *
    * @param[in] in_config EMVCo polling technologies are configured through this parameter
    * @param[in] in_isStartEMVCo specifies to start or stop the EMVCo mode
    *
    * @return void
    *
    */
    void setEMVCoMode(in byte in_config, boolean in_isStartEMVCo);

    /**
    *
    * @brief updates NFC state to EMVCo HAL.
    *
    *
    * @param[in] in_nfcState specifies the NFC state
    *
    * @return void
    */
    void onNfcStateChange(NfcState in_nfcState);

    /**
    *
    * @brief Register NFC callback function to receive the events from a listener device.
    *
    * @note This function is must to call before invoking any other api.
    *
    * @param[in] in_nfcStateChangeCallback INfcStateChangeRequestCallback the event callback function to be passed by caller
    *
    * @return boolean returns true, if success and returns false, if failed to register
    */
    boolean registerNFCStateChangeCallback(in INfcStateChangeRequestCallback in_nfcStateChangeCallback);
}
