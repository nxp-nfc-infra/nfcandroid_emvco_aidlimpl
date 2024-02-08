/******************************************************************************
 *
 *  Copyright 2022 NXP
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

/** \addtogroup EMVCO_STACK_HAL_CT_API_INTERFACE
 *  @brief The android application calls this interface to use the EMVCo Contact
 *   card functionality.
 *  @{
 */
#include <aidl/android/hardware/emvco/BnEmvcoTDA.h>
#include <aidl/android/hardware/emvco/IEmvcoClientCallback.h>
#include <aidl/android/hardware/emvco/IEmvcoTDACallback.h>
#include <android-base/logging.h>
#include <log/log.h>

namespace aidl {
namespace android {
namespace hardware {
namespace emvco {

class EmvcoTDA : public BnEmvcoTDA {

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
   * @return ::ndk::ScopedAStatus indicates registerEMVCoEventListener request
   * processed by EMVCo HAL successfully or not
   */
  ::ndk::ScopedAStatus registerEMVCoEventListener(
      const std::shared_ptr<IEmvcoClientCallback> &in_clientCallback,
      bool *_aidl_return) override;
  ::ndk::ScopedAStatus
  discoverTDA(const std::shared_ptr<IEmvcoTDACallback> &in_clientCallback,
              std::vector<::aidl::android::hardware::emvco::EmvcoTDAInfo>
                  *emvcoTDAInfo) override;
  ::ndk::ScopedAStatus openTDA(int8_t in_tdaID, int8_t *out_connID) override;
  ::ndk::ScopedAStatus transceive(const std::vector<uint8_t> &in_cmd_data,
                                  std::vector<uint8_t> *out_rsp_data) override;
  ::ndk::ScopedAStatus closeTDA(int8_t in_tdaID) override;
};

} // namespace emvco
} // namespace hardware
} // namespace android
} // namespace aidl
  /** @}*/