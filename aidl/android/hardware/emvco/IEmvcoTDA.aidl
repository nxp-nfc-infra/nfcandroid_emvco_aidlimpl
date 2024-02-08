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

import android.hardware.emvco.IEmvcoClientCallback;
import android.hardware.emvco.EmvcoTDAInfo;
import android.hardware.emvco.EmvcoStatus;
import android.hardware.emvco.IEmvcoTDACallback;

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

/**
  * @brief Interface to EMVCo contact card HAL.
  *
  * The android application calls this interface to utilize EMVCo
  * Contact card functionality of the underlying device.
  *
  */
 
@VintfStability
interface IEmvcoTDA {
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
	* @throws EX_UNSUPPORTED_OPERATION when the contact card feature is not supported.
    */
    boolean registerEMVCoEventListener(in IEmvcoClientCallback clientCallback);
    
    /**
    *
    * @brief discoverTDA provides all the details of smart card connected over TDA
    *
    * @param[in]  *in_clientCallback has EMVCo contact card client HAL callback
    *
    * @throws ServiceSpecificException with code
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return EmvcoTDAInfo[] returns all the smart card connected over TDA.
    *         valid emvcoTDAInfo received only when status is EMVCO_STATUS_OK
    */
    EmvcoTDAInfo[] discoverTDA(in IEmvcoTDACallback in_clientCallback);

    /**
    *
    * @brief opens the smart card connected over TDA
    * @param[in]  tdaID tda id of the smard card received through discoverTDA
    * 
    *
    * @throws ServiceSpecificException with codes
    *   - EMVCO_STATUS_INVALID_PARAMETER, if provided tdaID is in-valid
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return byte returns connection id of the smard card.
    *         valid connection id received only when status is EMVCO_STATUS_OK
    */
    byte openTDA(in byte tdaID);

    /**
    * @brief sends application data with the Device-Controller and 
    *        receives response data from controller
    *
    * @note connection id of the TDA should be added as part of NCI header.
    *
    * @param[in] in_cmd_data Application command data buffer
    *
    * @throws ServiceSpecificException with codes
    *   - EMVCO_STATUS_INVALID_PARAMETER, if provided connection id is in-valid
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return Response APDU received from controller.
    *         valid Response APDU received only when status is EMVCO_STATUS_OK
    */
    byte[] transceive(in byte[] in_cmd_data);

    /**
    *
    * @brief closes the smart card connected over TDA
    * @param[in]  tdaID id of the tda slot to be closed
    *
    * @throws ServiceSpecificException with codes
    *   - EMVCO_STATUS_INVALID_PARAMETER, if provided tdaID is in-valid
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return void
    */
    void closeTDA(in byte tdaID);

}
