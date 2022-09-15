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
#include "NxpEmvco.h"
#include "LinkedCallback.h"
#include "phNfcStatus.h"
#include <phNxpNciHal_Adaptation.h>
#define CHK_STATUS(x)                                                          \
  ((x) == NFCSTATUS_SUCCESS)                                                   \
      ? (::ndk::ScopedAStatus::ok())                                           \
      : (::ndk::ScopedAStatus::fromStatus(STATUS_FAILED_TRANSACTION))

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

using ::aidl::android::hardware::emvco::EmvcoEvent;
using ::aidl::android::hardware::emvco::EmvcoStatus;

std::vector<std::unique_ptr<LinkedCallback>> NxpEmvco::callbacks_;
std::mutex NxpEmvco::callbacks_lock_;
std::shared_ptr<NxpEmvco> NxpEmvco::emvco_service_;

void NxpEmvco::eventCallback(uint8_t event, uint8_t status) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  std::lock_guard<std::mutex> lock(callbacks_lock_);
  for (auto &it : callbacks_) {
    it->callback()->sendEvent((EmvcoEvent)event, (EmvcoStatus)status);
  }
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Exit", __func__);
}

void NxpEmvco::dataCallback(uint16_t data_len, uint8_t *p_data) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  std::lock_guard<std::mutex> lock(callbacks_lock_);
  std::vector<uint8_t> data(p_data, p_data + data_len);
  for (auto &it : callbacks_) {
    it->callback()->sendData(data);
  }
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Exit", __func__);
}

void OnCallbackDiedWrapped(void *cookie) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  LinkedCallback *linked = reinterpret_cast<LinkedCallback *>(cookie);
  linked->OnCallbackDied();
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Exit", __func__);
}

std::shared_ptr<NxpEmvco> NxpEmvco::getInstance() {
  if (emvco_service_ == nullptr) {
    emvco_service_ = ::ndk::SharedRefBase::make<NxpEmvco>();
    ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  }
  return emvco_service_;
}

NxpEmvco::NxpEmvco()
    : death_recipient_(AIBinder_DeathRecipient_new(&OnCallbackDiedWrapped)) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
}

::ndk::ScopedAStatus NxpEmvco::getEmvcoProfileDiscoveryInterface(
    std::shared_ptr<::aidl::android::hardware::emvco::INxpEmvcoProfileDiscovery>
        *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (nxp_emvco_profile_discovery_ == nullptr) {
    nxp_emvco_profile_discovery_ =
        ::ndk::SharedRefBase::make<NxpEmvcoProfileDiscovery>();
  }
  *_aidl_return = nxp_emvco_profile_discovery_;
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus NxpEmvco::getNxpEmvcoContactlessCard(
    std::shared_ptr<::aidl::android::hardware::emvco::INxpEmvcoContactlessCard>
        *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (nxp_emvco_contactless_card == nullptr) {
    nxp_emvco_contactless_card =
        ::ndk::SharedRefBase::make<NxpEmvcoContactlessCard>();
  }
  *_aidl_return = nxp_emvco_contactless_card;
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus NxpEmvco::getNxpEmvcoContactCard(
    std::shared_ptr<::aidl::android::hardware::emvco::INxpEmvcoContactCard>
        *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (nxp_emvco_contact_card_ == nullptr) {
    nxp_emvco_contact_card_ = ::ndk::SharedRefBase::make<NxpEmvcoContactCard>();
  }
  *_aidl_return = nxp_emvco_contact_card_;
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus NxpEmvco::doRegisterEMVCoEventListener(
    const std::shared_ptr<
        ::aidl::android::hardware::emvco::IEmvcoClientCallback>
        &in_clientCallback,
    bool *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  registerCallback(in_clientCallback);
  *_aidl_return = true;
  return ndk::ScopedAStatus::ok();
}

binder_status_t NxpEmvco::dump(int fd, const char **p, uint32_t q) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s BALA dump method called", __func__);
  (void)fd;
  (void)p;
  (void)q;
  return STATUS_OK;
}

::ndk::ScopedAStatus NxpEmvco::handleNfcStateChanged(int32_t in_nfcState) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s BALA handleNfcStateChanged nfcState:%d",
           __func__, in_nfcState);
  phNxpNciHal_handleNfcStateChanged(in_nfcState);
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus NxpEmvco::doSetEMVCoMode(int8_t in_config,
                                              bool in_isStartEMVCo) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter in_config:%d", __func__, in_config);
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: phNxpNciHal_open called check return",
           __func__);

  phNxpNciHal_open(eventCallback, dataCallback);

  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter in_isStartEMVCo:%d", __func__,
           in_isStartEMVCo);
  phNxpNciHal_doSetEMVCoMode(in_config, in_isStartEMVCo);
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: phNxpNciHal_doSetEMVCoMode returned",
           __func__);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus NxpEmvco::open() {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  NFCSTATUS status = phNxpNciHal_open(eventCallback, dataCallback);

  return CHK_STATUS(status);
}
::ndk::ScopedAStatus NxpEmvco::transceive(const std::vector<uint8_t> &in_data,
                                          int32_t *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  std::vector<uint8_t> data(in_data.begin(), in_data.end());
  *_aidl_return = phNxpNciHal_write(data.size(), (uint8_t *)data.data());
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus NxpEmvco::close(
    const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (in_clientCallback == nullptr) {
    LOG(ERROR) << __func__ << "Client callback is NULL";
  } else {
    unregisterCallback(in_clientCallback);
  }
  NFCSTATUS status = phNxpNciHal_close(false);
  return CHK_STATUS(status);
}

void NxpEmvco::registerCallback(
    const std::shared_ptr<IEmvcoClientCallback> &callback) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (callback == nullptr) {
    LOG(ERROR) << __func__ << "Client callback is NULL";
    return;
  }
  {
    std::lock_guard<std::mutex> lock(callbacks_lock_);
    callbacks_.emplace_back(LinkedCallback::Make(ref<NxpEmvco>(), callback));
    ALOGD_IF(EMVCO_HAL_DEBUG, "Total clients=%d", (int)callbacks_.size());
  }
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Exit", __func__);
}

void NxpEmvco::unregisterCallback(
    const std::shared_ptr<IEmvcoClientCallback> &callback) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (callback == nullptr) {
    LOG(ERROR) << __func__ << "Client callback is NULL";
    return;
  }
  std::lock_guard<std::mutex> lock(callbacks_lock_);
  auto matches = [callback](const auto &linked) {
    return linked->callback()->asBinder() == callback->asBinder();
  };
  auto it1 = std::remove_if(callbacks_.begin(), callbacks_.end(), matches);
  bool removed = (it1 != callbacks_.end());
  if (removed) {
    callbacks_.erase(it1, callbacks_.end());
  } else {
    LOG(ERROR) << __func__ << "unregisterCallback failed";
  }
  ALOGD_IF(EMVCO_HAL_DEBUG, "Total clients = %d", (int)callbacks_.size());
}

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl
