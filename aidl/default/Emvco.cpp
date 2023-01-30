/******************************************************************************
 *
 *  Copyright 2022,2023 NXP
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
#include "Emvco.h"
#include "LinkedCallback.h"
#include <emvco_hal.h>
#define CHK_STATUS(x)                                                          \
  ((x) == EMVCO_STATUS_SUCCESS)                                                \
      ? (::ndk::ScopedAStatus::ok())                                           \
      : (::ndk::ScopedAStatus::fromStatus(STATUS_FAILED_TRANSACTION))

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

using ::aidl::android::hardware::emvco::DiscoveryMode;
using ::aidl::android::hardware::emvco::EmvcoEvent;
using ::aidl::android::hardware::emvco::EmvcoStatus;

std::vector<std::unique_ptr<LinkedCallback>> Emvco::callbacks_;
std::mutex Emvco::callbacks_lock_;
std::shared_ptr<Emvco> Emvco::emvco_service_;
std::shared_ptr<INfcStateChangeRequestCallback>
    Emvco::nfc_State_change_callback = nullptr;

void Emvco::setNfcState(bool enableNfc) {
  if (Emvco::nfc_State_change_callback != nullptr) {
    auto ret = Emvco::nfc_State_change_callback->enableNfc(enableNfc);
    if (!ret.isOk()) {
      LOG(ERROR) << "Failed to send event!";
    }
  }
}

void Emvco::eventCallback(uint8_t event, uint8_t status) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  std::lock_guard<std::mutex> lock(callbacks_lock_);
  for (auto &it : callbacks_) {
    it->callback()->sendEvent((EmvcoEvent)event, (EmvcoStatus)status);
  }
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Exit", __func__);
}

void Emvco::dataCallback(uint16_t data_len, uint8_t *p_data) {
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

std::shared_ptr<Emvco> Emvco::getInstance() {
  if (emvco_service_ == nullptr) {
    emvco_service_ = ::ndk::SharedRefBase::make<Emvco>();
    ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  }
  return emvco_service_;
}

Emvco::Emvco()
    : death_recipient_(AIBinder_DeathRecipient_new(&OnCallbackDiedWrapped)) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  open();
}

::ndk::ScopedAStatus Emvco::getEmvcoProfileDiscoveryInterface(
    std::shared_ptr<::aidl::android::hardware::emvco::IEmvcoProfileDiscovery>
        *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (nxp_emvco_profile_discovery_ == nullptr) {
    nxp_emvco_profile_discovery_ =
        ::ndk::SharedRefBase::make<EmvcoProfileDiscovery>();
  }
  *_aidl_return = nxp_emvco_profile_discovery_;
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus Emvco::getEmvcoContactlessCard(
    std::shared_ptr<::aidl::android::hardware::emvco::IEmvcoContactlessCard>
        *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (nxp_emvco_contactless_card == nullptr) {
    nxp_emvco_contactless_card =
        ::ndk::SharedRefBase::make<EmvcoContactlessCard>();
  }
  *_aidl_return = nxp_emvco_contactless_card;
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus Emvco::getEmvcoContactCard(
    std::shared_ptr<::aidl::android::hardware::emvco::IEmvcoContactCard>
        *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (nxp_emvco_contact_card_ == nullptr) {
    nxp_emvco_contact_card_ = ::ndk::SharedRefBase::make<EmvcoContactCard>();
  }
  *_aidl_return = nxp_emvco_contact_card_;
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Emvco::registerEMVCoEventListener(
    const std::shared_ptr<
        ::aidl::android::hardware::emvco::IEmvcoClientCallback>
        &in_clientCallback,
    bool *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  registerCallback(in_clientCallback);
  *_aidl_return = true;
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Emvco::getCurrentDiscoveryMode(
    ::aidl::android::hardware::emvco::DiscoveryMode *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  *_aidl_return = (DiscoveryMode)get_current_discovery_mode();
  return ndk::ScopedAStatus::ok();
}

binder_status_t Emvco::dump(int fd, const char **p, uint32_t q) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s", __func__);
  (void)fd;
  (void)p;
  (void)q;
  return STATUS_OK;
}

::ndk::ScopedAStatus Emvco::onNfcStateChange(NfcState in_nfcState) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s onNfcStateChange nfcState:%d", __func__,
           (int)in_nfcState);
  on_nfc_state_change((int)in_nfcState);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Emvco::registerNFCStateChangeCallback(
    const std::shared_ptr<
        ::aidl::android::hardware::emvco::INfcStateChangeRequestCallback>
        &in_nfcStateChangeCallback,
    bool *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s", __func__);
  Emvco::nfc_State_change_callback = in_nfcStateChangeCallback;
  *_aidl_return = true;
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus Emvco::setEMVCoMode(int8_t in_config,
                                         bool in_isStartEMVCo) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter in_config:%d", __func__, in_config);
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: phNxpNciHal_open called check return",
           __func__);

  open_emvco_app_data_channel(eventCallback, dataCallback, setNfcState);

  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter in_isStartEMVCo:%d", __func__,
           in_isStartEMVCo);
  set_emvco_mode(in_config, in_isStartEMVCo);
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: phNxpNciHal_doSetEMVCoMode returned",
           __func__);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Emvco::open() {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  EMVCO_STATUS status =
      open_emvco_app_data_channel(eventCallback, dataCallback, setNfcState);

  return CHK_STATUS(status);
}
::ndk::ScopedAStatus Emvco::transceive(const std::vector<uint8_t> &in_data,
                                       int32_t *_aidl_return) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  std::vector<uint8_t> data(in_data.begin(), in_data.end());
  *_aidl_return = send_emvco_app_data(data.size(), (uint8_t *)data.data());
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus
Emvco::close(const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (in_clientCallback == nullptr) {
    LOG(ERROR) << __func__ << "Client callback is NULL";
  } else {
    unregisterCallback(in_clientCallback);
  }
  EMVCO_STATUS status = close_emvco_app_data_channel(false);
  return CHK_STATUS(status);
}

void Emvco::registerCallback(
    const std::shared_ptr<IEmvcoClientCallback> &callback) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (callback == nullptr) {
    LOG(ERROR) << __func__ << "Client callback is NULL";
    return;
  }
  {
    std::lock_guard<std::mutex> lock(callbacks_lock_);
    callbacks_.emplace_back(LinkedCallback::Make(ref<Emvco>(), callback));
    ALOGD_IF(EMVCO_HAL_DEBUG, "Total clients=%d", (int)callbacks_.size());
  }
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Exit", __func__);
}

void Emvco::unregisterCallback(
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
