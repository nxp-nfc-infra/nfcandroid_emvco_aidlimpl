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
#pragma once
#include "Emvco.h"
#include <aidl/android/hardware/emvco/BnEmvcoProfileDiscovery.h>
#include <aidl/android/hardware/emvco/IEmvcoClientCallback.h>
#include <aidl/android/hardware/emvco/INfcStateChangeRequestCallback.h>

/** \addtogroup EMVCO_STACK_HAL_MODE_SWITCH_API_INTERFACE
 *  @brief   The android application calls this interface to utilize EMVCo
 *           mode switch functionality of the underlying device.
 *  @{
 */

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

class EmvcoProfileDiscovery : public BnEmvcoProfileDiscovery {

public:
  /**
   *
   * @brief Register EMVCo callback function to receive the events from a
   * listener device.
   *
   * @note This function is must to call before invoking any other api.
   *
   * @param[in]  *in_clientCallback has EMVCo client HAL callback
   * @param[in]  *in_aidl_return indicates register status in return to caller
   *
   * @return boolean returns true, if success and returns false, if failed to
   * register
   */
  ::ndk::ScopedAStatus registerEMVCoEventListener(
      const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback,
      bool *in_aidl_return) override;

  /**
   * @brief starts the EMVCo mode with the Device-Controller.
   *
   * Once the @ref open "Application Data Channel is "
   * established, the Application may send start the EMVCo mode with the
   * Device-Controller.
   *
   * @param[in] in_disc_mask EMVCo polling technologies are configured through
   * this parameter
   * @param[in] in_isStartEMVCo specifies to start or stop the EMVCo mode
   *
   * @return void
   *
   */
  ::ndk::ScopedAStatus setEMVCoMode(int8_t in_disc_mask,
                                    bool in_isStartEMVCo) override;

  /**
   *
   * @brief Register NFC callback function to receive the events from a listener
   * device.
   *
   * @note This function is must to call before invoking any other api.
   *
   * @param[in] in_nfcStateChangeCallback INfcStateChangeRequestCallback the
   * event callback function to be passed by caller. It should implement to turn
   * ON/OFF NFC based on the request received.
   *
   * @return boolean returns true, if success and returns false, if failed to
   * register
   */
  ::ndk::ScopedAStatus registerNFCStateChangeCallback(
      const std::shared_ptr<
          ::aidl::android::hardware::emvco::INfcStateChangeRequestCallback>
          &in_nfcStateChangeRequestCallback,
      bool *_aidl_return) override;

  /**
   *
   * @brief updates NFC state to EMVCo HAL.
   *
   *
   * @param[in] in_nfcState specifies the NFC state
   *
   * @return void
   */
  ::ndk::ScopedAStatus onNfcStateChange(NfcState in_nfcState) override;

  /**
   * @brief returns the current active profile type.
   *
   * @return DiscoveryMode - NFC/EMVCo/Unknown
   *
   */
  ::ndk::ScopedAStatus getCurrentDiscoveryMode(
      ::aidl::android::hardware::emvco::DiscoveryMode *_aidl_return) override;
};

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl
  /** @}*/