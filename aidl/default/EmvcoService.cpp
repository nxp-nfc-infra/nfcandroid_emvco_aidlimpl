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

#include "Emvco.h"
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
//#include <cstring>

using ::aidl::android::hardware::emvco::Emvco;

int main() {
  LOG(INFO) << "EMVCO HAL starting up";
  if (!ABinderProcess_setThreadPoolMaxThreadCount(1)) {
    LOG(INFO) << "failed to set thread pool max thread count";
    return 1;
  }
  std::shared_ptr<Emvco> emvco_service = ndk::SharedRefBase::make<Emvco>();

  const std::string instance = std::string() + Emvco::descriptor + "/default";
  ALOGD_IF(EMVCO_HAL_DEBUG, "EMVCo Registering service: %s", instance.c_str());
  binder_status_t status = AServiceManager_addService(
      emvco_service->asBinder().get(), instance.c_str());
  ALOGD_IF(EMVCO_HAL_DEBUG, "EMVCo Registered IEMVCo service status: %d",
           status);
  CHECK(status == STATUS_OK);
  ABinderProcess_joinThreadPool();

  return EXIT_FAILURE; // should not reach
}
