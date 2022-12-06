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
#include "EmvcoProfileDiscovery.h"

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

::ndk::ScopedAStatus EmvcoProfileDiscovery::registerEMVCoEventListener(
    const std::shared_ptr<
        ::aidl::android::hardware::emvco::IEmvcoClientCallback>
        &in_clientCallback,
    bool *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  return Emvco::getInstance()->registerEMVCoEventListener(in_clientCallback,
                                                          _aidl_return);
}

::ndk::ScopedAStatus EmvcoProfileDiscovery::setEMVCoMode(int8_t in_config,
                                                         bool in_isStartEMVCo) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  return Emvco::getInstance()->setEMVCoMode(in_config, in_isStartEMVCo);
}

::ndk::ScopedAStatus EmvcoProfileDiscovery::registerNFCStateChangeCallback(
    const std::shared_ptr<
        ::aidl::android::hardware::emvco::INfcStateChangeRequestCallback>
        &in_nfcStateChangeCallback,
    bool *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  return Emvco::getInstance()->registerNFCStateChangeCallback(
      in_nfcStateChangeCallback, _aidl_return);
}
::ndk::ScopedAStatus
EmvcoProfileDiscovery::onNfcStateChange(NfcState in_nfcState) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  return Emvco::getInstance()->onNfcStateChange(in_nfcState);
}
} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl