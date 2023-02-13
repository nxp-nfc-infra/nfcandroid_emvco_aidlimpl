/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
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

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

 /**
  * @brief Interface to EMVCo Contactless card HAL.
  *
  * The android application calls this interface to utilize EMVCo
  * Contact less functionality of the underlying device.
  * The android device, which supports Wireless Charging, implements INxpEmvcoContactlessCard
  * interface as @ref EMVCo_POLLER_LIB "EMVCo HAL" to provide functionalities to
  * Android application.
  *
  */

@VintfStability
interface IEmvcoContactlessCard {
    /**
    *
    * @brief Register callback function to receive the events from a listener device.
    *
    * @note This function is must to call before invoking any other api.
    *
    * @param cb IEmvcoClientCallback the event callback function to be
             passed by caller
    *
    * @return boolean returns true, if success and returns false, if failed to register
    */
    boolean registerEMVCoEventListener(in IEmvcoClientCallback clientCallback);

    /**
    * @brief send application data with the Device-Controller.
    *
    * @note In case if send data is failed, Application shall again invoke
    *          @ref open "open()" before
    *          invoking @ref open "this" API.
    *
    * @param[in] in_data Application data buffer
    *
    * @return int indicating execution status
    *
    */
    int transceive(in byte[] in_data);

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
    void setEMVCoMode(in byte in_config,boolean in_isStartEMVCo);
}

/** @}*/