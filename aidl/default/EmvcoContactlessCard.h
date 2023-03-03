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

/** \addtogroup EMVCO_STACK_HAL_CL_API_INTERFACE
 *  @brief The android application calls this interface to use the EMVCo
 *   Contactless card functionality.
 *  @{
 */

#include <aidl/android/hardware/emvco/BnEmvcoContactlessCard.h>
#include <aidl/android/hardware/emvco/IEmvcoClientCallback.h>

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

class EmvcoContactlessCard : public BnEmvcoContactlessCard {

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
      bool *_aidl_return) override;

  /**
   * @brief send application data with the Device-Controller.
   *
   * @note In case if send data is failed, Application shall again invoke
   *          @ref open "open()" before
   *          invoking @ref open "this" API.
   *
   * @param[in] in_data Application data buffer
   *
   * @return EmvcoStatus indicating execution status
   *
   */
  ::ndk::ScopedAStatus transceive(const std::vector<uint8_t> &in_data,
                                  int32_t *_aidl_return) override;

  /**
   * @brief starts the EMVCo mode with the Device-Controller.
   *
   * Once the @ref open "Application Data Channel is "
   * established, the Application may send start the EMVCo mode with the
   * Device-Controller.
   *
   * @param[in] in_config EMVCo polling technologies are configured through this
   * parameter
   * @param[in] in_isStartEMVCo specifies to start or stop the EMVCo mode
   *
   * @return void
   *
   */
  ::ndk::ScopedAStatus setEMVCoMode(int8_t in_config, bool in_isStartEMVCo);

  /**
   * @brief stops the RF field and moves in to the specified deactivation state.
   *
   * @param[in] in_deactivationType specifies the state to be in after RF
   * deactivation
   *
   * @return EmvcoStatus returns EMVCO_STATUS_OK, if command processed
   * successfully and returns EMVCO_STATUS_FAILED, if command is not processed
   * due to in-valid state. EMVCo mode should be ON to call this API
   *
   */
  ::ndk::ScopedAStatus stopRFDisovery(
      ::aidl::android::hardware::emvco::DeactivationType in_deactivationType,
      ::aidl::android::hardware::emvco::EmvcoStatus *emvco_status) override;
};

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl
  /** @}*/