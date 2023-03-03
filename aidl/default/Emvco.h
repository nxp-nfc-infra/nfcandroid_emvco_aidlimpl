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
#ifndef EMVCO_HAL_DEBUG
#define EMVCO_HAL_DEBUG true
#endif
/** @defgroup EMVCO_HAL_API_INTERFACE.
 * The android application calls this interface to get the EMVCo mode switch and
 * Contact less Transaction functionality
 */

/** \addtogroup EMVCO_STACK_HAL_API_INTERFACE
 *  @brief interface to EMVCo mode switch, contact and contact interfaces.
 *           The android device, which supports EMVCo feature, implements
 * IEmvcoProfileDiscovery, IEmvcoContactlessCard, IEmvcoContactCard interface as
 * @ref EMVCo_POLLER_LIB to provide functionalities to Android application.
 *  @{
 */

#include "EmvcoContactCard.h"
#include "EmvcoContactlessCard.h"
#include "EmvcoProfileDiscovery.h"
#include <aidl/android/hardware/emvco/BnEmvco.h>
#include <aidl/android/hardware/emvco/DiscoveryMode.h>
#include <aidl/android/hardware/emvco/IEmvcoClientCallback.h>
#include <aidl/android/hardware/emvco/INfcStateChangeRequestCallback.h>
#include <android-base/logging.h>
#include <log/log.h>

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

using ::ndk::ICInterface;
class LinkedCallback;

class Emvco : public BnEmvco {
public:
  Emvco();

  /**
   * @brief Provides the EMVCo mode switch HAL interface.
   *
   * @param  none
   *
   * @return IEmvcoProfileDiscovery EMVCo mode switch HAL interface
   *
   */
  ::ndk::ScopedAStatus getEmvcoProfileDiscoveryInterface(
      std::shared_ptr<::aidl::android::hardware::emvco::IEmvcoProfileDiscovery>
          *_aidl_return) override;

  /**
   * @brief Provides the EMVCo contact less HAL interface.
   *
   * @param  none
   *
   * @return IEmvcoContactlessCard EMVCo contact less HAL interface
   *
   */
  ::ndk::ScopedAStatus getEmvcoContactlessCard(
      std::shared_ptr<::aidl::android::hardware::emvco::IEmvcoContactlessCard>
          *_aidl_return) override;

  /**
   * @brief Provides the EMVCo contact HAL interface.
   *
   * @param  none
   *
   * @return IEmvcoContactCard EMVCo contact HAL interface
   *
   */
  ::ndk::ScopedAStatus getEmvcoContactCard(
      std::shared_ptr<::aidl::android::hardware::emvco::IEmvcoContactCard>
          *_aidl_return) override;
  ::ndk::ScopedAStatus onNfcStateChange(NfcState in_nfcState);

  ::ndk::ScopedAStatus registerEMVCoEventListener(
      const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback,
      bool *_aidl_return);
  ::ndk::ScopedAStatus setEMVCoMode(int8_t in_config, bool in_isStartEMVCo);
  ::ndk::ScopedAStatus transceive(const std::vector<uint8_t> &in_data,
                                  int32_t *_aidl_return);
  binder_status_t dump(int fd, const char **args, uint32_t num_args) override;
  ::ndk::ScopedAStatus open();
  ::ndk::ScopedAStatus
  close(const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback);
  ::ndk::ScopedAStatus registerNFCStateChangeCallback(
      const std::shared_ptr<
          ::aidl::android::hardware::emvco::INfcStateChangeRequestCallback>
          &in_nfcStateChangeCallback,
      bool *_aidl_return);
  ::ndk::ScopedAStatus getCurrentDiscoveryMode(
      ::aidl::android::hardware::emvco::DiscoveryMode *_aidl_return);
  ::ndk::ScopedAStatus stopRFDisovery(
      ::aidl::android::hardware::emvco::DeactivationType in_deactivationType,
      ::aidl::android::hardware::emvco::EmvcoStatus *emvco_status);
  ::ndk::ScopedAStatus
  setLed(::aidl::android::hardware::emvco::LedControl in_ledControl,
         ::aidl::android::hardware::emvco::EmvcoStatus *emvco_status);

  static void eventCallback(uint8_t event, uint8_t status);
  static void dataCallback(uint16_t data_len, uint8_t *p_data);
  static std::shared_ptr<Emvco> getInstance();
  static void setNfcState(bool enableNfc);
  static std::shared_ptr<INfcStateChangeRequestCallback>
      nfc_State_change_callback;

private:
  friend LinkedCallback;
  static std::shared_ptr<Emvco> emvco_service_;
  static std::vector<std::unique_ptr<LinkedCallback>> callbacks_;
  static std::mutex callbacks_lock_;
  ndk::ScopedAIBinder_DeathRecipient death_recipient_;
  std::shared_ptr<IEmvcoProfileDiscovery> nxp_emvco_profile_discovery_;
  std::shared_ptr<IEmvcoContactCard> nxp_emvco_contact_card_;
  std::shared_ptr<IEmvcoContactlessCard> nxp_emvco_contactless_card;

  void registerCallback(const std::shared_ptr<IEmvcoClientCallback> &callback);
  void
  unregisterCallback(const std::shared_ptr<IEmvcoClientCallback> &callback);
  void setEMVCoModeImpl(int8_t in_config);
};

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl

/** @}*/