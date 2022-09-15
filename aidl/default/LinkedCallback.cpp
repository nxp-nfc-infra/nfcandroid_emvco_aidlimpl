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
#include "LinkedCallback.h"
#include <android/binder_ibinder.h>
#include <utils/Errors.h>

namespace aidl::android::hardware::emvco {

LinkedCallback::LinkedCallback() = default;

LinkedCallback::~LinkedCallback() {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  if (callback_ == nullptr) {
    return;
  }
  auto status = AIBinder_unlinkToDeath(callback_->asBinder().get(),
                                       service()->death_recipient_.get(),
                                       reinterpret_cast<void *>(this));
  if (status != STATUS_OK && status != STATUS_DEAD_OBJECT) {
    LOG(WARNING) << __func__ << "Cannot unlink to death: "
                 << ::android::statusToString(status);
  }
}

std::unique_ptr<LinkedCallback>
LinkedCallback::Make(std::shared_ptr<NxpEmvco> service,
                     std::shared_ptr<IEmvcoClientCallback> callback) {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  std::unique_ptr<LinkedCallback> ret(new LinkedCallback());
  binder_status_t linkRet = AIBinder_linkToDeath(
      callback->asBinder().get(), service->death_recipient_.get(),
      reinterpret_cast<void *>(ret.get()));
  if (linkRet != ::STATUS_OK) {
    LOG(WARNING) << __func__ << "Cannot link to death: " << linkRet;
    return nullptr;
  }
  ret->service_ = service;
  ret->callback_ = std::move(callback);
  return ret;
}

std::shared_ptr<NxpEmvco> LinkedCallback::service() {
  auto service_sp = service_.lock();
  CHECK_NE(nullptr, service_sp);
  return service_sp;
}

void LinkedCallback::OnCallbackDied() {
  ALOGD_IF(EMVCO_HAL_DEBUG, "%s: Enter", __func__);
  service()->unregisterCallback(callback_);
}

} // namespace aidl::android::hardware::emvco
