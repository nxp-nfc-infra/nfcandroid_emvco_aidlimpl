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

  ::ndk::ScopedAStatus
  setLed(::aidl::android::hardware::emvco::LedControl in_ledControl,
         ::aidl::android::hardware::emvco::EmvcoStatus *emvco_status) override;
  ::ndk::ScopedAStatus setByteConfig(
      ::aidl::android::hardware::emvco::ConfigType in_type, int32_t in_length,
      int8_t in_value,
      ::aidl::android::hardware::emvco::EmvcoStatus *_aidl_return) override;
  ::ndk::ScopedAStatus setByteArrayConfig(
      ::aidl::android::hardware::emvco::ConfigType in_type, int32_t in_length,
      const std::vector<uint8_t> &in_value,
      ::aidl::android::hardware::emvco::EmvcoStatus *_aidl_return) override;
  ::ndk::ScopedAStatus setStringConfig(
      ::aidl::android::hardware::emvco::ConfigType in_type, int32_t in_length,
      const std::string &in_value,
      ::aidl::android::hardware::emvco::EmvcoStatus *_aidl_return) override;
};

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl
  /** @}*/