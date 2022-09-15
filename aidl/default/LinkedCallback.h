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

#include <NxpEmvco.h>
#include <aidl/android/hardware/emvco/IEmvcoClientCallback.h>
#include <android-base/macros.h>
#include <android/binder_auto_utils.h>
#include <memory>

namespace aidl::android::hardware::emvco {

class LinkedCallback {
public:
  static std::unique_ptr<LinkedCallback>
  Make(std::shared_ptr<NxpEmvco> service,
       std::shared_ptr<IEmvcoClientCallback> callback);
  ~LinkedCallback();
  const std::shared_ptr<IEmvcoClientCallback> &callback() const {
    return callback_;
  }
  void OnCallbackDied();

private:
  LinkedCallback();
  DISALLOW_COPY_AND_ASSIGN(LinkedCallback);
  std::shared_ptr<NxpEmvco> service();
  std::weak_ptr<NxpEmvco> service_;
  std::shared_ptr<IEmvcoClientCallback> callback_;
};

} // namespace aidl::android::hardware::emvco
