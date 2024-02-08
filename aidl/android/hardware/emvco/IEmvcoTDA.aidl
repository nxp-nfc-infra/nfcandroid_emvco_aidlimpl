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
    * @brief Register callback function to receive the events needed for CT functionality.
    *
    * @note This function is must to call before invoking any other api.
    *
    * @param[in]  *in_clientCallback has EMVCo contact card client HAL callback
    *
    * @return boolean returns true, if success and returns false, if failed to register
    */
    boolean registerEMVCoCTListener(in IEmvcoTDACallback in_clientCallback);

    /**
    *
    * @brief discoverTDA provides all the details of smart card connected over TDA
    *
    *
    * @throws ServiceSpecificException with code
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return EmvcoTDAInfo[] returns all the smart card connected over TDA.
    *         valid emvcoTDAInfo received only when status is EMVCO_STATUS_OK
    */
    EmvcoTDAInfo[] discoverTDA();

    /**
    *
    * @brief opens the smart card connected over TDA. It internally calls
    *        mode set enable and core connection create. mode set enable is 
    *        called based on standBy flag.
    *
    * @note  Please gothrough closeTDA API to understand the usage of standBy flag
    *
    * @param[in]  tdaID tda id of the smard card received through discoverTDA
    * @param[in]  standBy false, opens the communication with TDA freshely and mode set enable command is sent to controller.
    *         standBy true, resumes the communication from partial close and does not send mode set enable command to controller
    * 
    * @note       use standby false, if you are opening the TDA for first time.
    *             use standby true, if you are opening the TDA followed by partial close of another TDA
    *
    *
    * @throws ServiceSpecificException with codes
    *   - EMVCO_STATUS_INVALID_PARAMETER, if provided tdaID is in-valid
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return byte returns connection id of the smard card.
    *         valid connection id received only when status is EMVCO_STATUS_OK
    */
    byte openTDA(in byte tdaID, boolean standBy);

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
    * @brief closes the smart card connected over TDA.It internally calls
    *        core connection close and mode set disable. mode set disable is 
    *        called based on standBy flag.
    *
    * @param[in]  tdaID tda ID of the tda slot to be closed
    * @param[in]  standBy true, closes the communication with TDA fully and allows the system to go in standbymode 
    *         standBy false, closes the communication partially and does not allow the system to go in standbymode.
    * 
    * @note       use standby false, If you are closing the current TDA to open another TDA for communication then use false to get better performance
    *             use standby true, If you are closing the current TDA to stop the communication with it fully and allow system to enter standby mode
    *
    * @throws ServiceSpecificException with codes
    *   - EMVCO_STATUS_INVALID_PARAMETER, if provided tdaID is in-valid
    *   - EMVCO_STATUS_FEATURE_NOT_SUPPORTED when the contact card feature is not supported.
    *
    * @return void
    */
    void closeTDA(in byte tdaID, boolean standBy);

}
