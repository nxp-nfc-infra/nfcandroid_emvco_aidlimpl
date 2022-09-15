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
#pragma once
#ifndef EMVCO_HAL_DEBUG
#define EMVCO_HAL_DEBUG true
#endif

#include "NxpEmvcoContactCard.h"
#include "NxpEmvcoContactlessCard.h"
#include "NxpEmvcoProfileDiscovery.h"
#include <aidl/android/hardware/emvco/BnNxpEmvco.h>
#include <aidl/android/hardware/emvco/IEmvcoClientCallback.h>
#include <android-base/logging.h>
#include <log/log.h>

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

using ::ndk::ICInterface;
class LinkedCallback;

class NxpEmvco : public BnNxpEmvco {
public:
  NxpEmvco();
  ::ndk::ScopedAStatus getEmvcoProfileDiscoveryInterface(
      std::shared_ptr<
          ::aidl::android::hardware::emvco::INxpEmvcoProfileDiscovery>
          *_aidl_return) override;
  ::ndk::ScopedAStatus getNxpEmvcoContactlessCard(
      std::shared_ptr<
          ::aidl::android::hardware::emvco::INxpEmvcoContactlessCard>
          *_aidl_return) override;
  ::ndk::ScopedAStatus getNxpEmvcoContactCard(
      std::shared_ptr<::aidl::android::hardware::emvco::INxpEmvcoContactCard>
          *_aidl_return) override;
  ::ndk::ScopedAStatus handleNfcStateChanged(int32_t in_nfcState) override;

  ::ndk::ScopedAStatus doRegisterEMVCoEventListener(
      const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback,
      bool *_aidl_return);
  ::ndk::ScopedAStatus doSetEMVCoMode(int8_t in_config, bool in_isStartEMVCo);
  ::ndk::ScopedAStatus transceive(const std::vector<uint8_t> &in_data,
                                  int32_t *_aidl_return);
  binder_status_t dump(int fd, const char **args, uint32_t num_args) override;
  ::ndk::ScopedAStatus open();
  ::ndk::ScopedAStatus
  close(const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback);

  static void eventCallback(uint8_t event, uint8_t status);
  static void dataCallback(uint16_t data_len, uint8_t *p_data);
  static std::shared_ptr<NxpEmvco> getInstance();

private:
  friend LinkedCallback;
  static std::shared_ptr<NxpEmvco> emvco_service_;
  static std::vector<std::unique_ptr<LinkedCallback>> callbacks_;
  static std::mutex callbacks_lock_;
  ndk::ScopedAIBinder_DeathRecipient death_recipient_;
  std::shared_ptr<INxpEmvcoProfileDiscovery> nxp_emvco_profile_discovery_;
  std::shared_ptr<INxpEmvcoContactCard> nxp_emvco_contact_card_;
  std::shared_ptr<INxpEmvcoContactlessCard> nxp_emvco_contactless_card;
  void registerCallback(const std::shared_ptr<IEmvcoClientCallback> &callback);
  void
  unregisterCallback(const std::shared_ptr<IEmvcoClientCallback> &callback);
  void doSetEMVCoModeImpl(int8_t in_config);
};

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl
