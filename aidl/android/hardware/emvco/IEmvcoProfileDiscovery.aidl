/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of NXP nor the names of its contributors may be used
 * to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

package android.hardware.emvco;

import android.hardware.emvco.IEmvcoClientCallback;
import android.hardware.emvco.INfcStateChangeRequestCallback;
import android.hardware.emvco.NfcState;
import android.hardware.emvco.DiscoveryMode;
import android.hardware.emvco.LedControl;
import android.hardware.emvco.EmvcoStatus;
import android.hardware.emvco.ConfigType;

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
    void setEMVCoMode(in byte config, boolean isStartEMVCo);

    /**
    *
    * @brief updates NFC state to EMVCo HAL.
    *
    *
    * @param[in] in_nfcState specifies the NFC state
    *
    * @return void
    */
    void onNfcStateChange(NfcState nfcState);

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
    boolean registerNFCStateChangeCallback(in INfcStateChangeRequestCallback nfcStateChangeCallback);

    /**
    * @brief returns the current active profile type.
    *
    * @return DiscoveryMode - NFC/EMVCo/Unknown
    *
    */
    DiscoveryMode getCurrentDiscoveryMode();

    /**
    * @brief allows to turn ON/OFF the specified LED.
    *
    * @param[in] ledControl specifies the LED to be turned ON or OFF
    *
    * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed successfully and returns EMVCO_STATUS_FAILED, if command
    *         is not processed due to in-valid state. EMVCo mode should be ON to call this API
    *
    */
    EmvcoStatus setLed(in LedControl ledControl);

    /**
    * @brief allows to set the single byte value.
    *
    * @param[in] type - name of the config
    * @param[in] length - length of the config value
    * @param[in] value - actual byte value to be set
    *
    * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed successfully and returns EMVCO_STATUS_FAILED, if command
    *         is not processed successfully.
    *
    */
    EmvcoStatus setByteConfig(in ConfigType type, in int length, in byte value);

    /**
    * @brief allows to set the byte array value.
    *
    * @param[in] type - name of the config
    * @param[in] length - length of the config value
    * @param[in] value - actual byte array value to be set
    *
    * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed successfully and returns EMVCO_STATUS_FAILED, if command
    *         is not processed successfully
    *
    */
    EmvcoStatus setByteArrayConfig(in ConfigType type, in int length, in byte[] value);

    /**
    * @brief allows to set the string value.
    *
    * @param[in] type - name of the config
    * @param[in] length - length of the config value
    * @param[in] value - actual string value to be set
    *
    * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed successfully and returns EMVCO_STATUS_FAILED, if command
    *         is not processed successfully
    *
    */
    EmvcoStatus setStringConfig(in ConfigType type, in int length, in String value);
}
