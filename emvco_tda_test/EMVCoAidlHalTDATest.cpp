/******************************************************************************
 *
 *  Copyright 2023 NXP
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

#define LOG_TAG "emvco_tda_test"

#include <aidl/Gtest.h>
#include <aidl/android/hardware/emvco/BnEmvco.h>
#include <aidl/android/hardware/emvco/BnEmvcoClientCallback.h>
#include <aidl/android/hardware/emvco/CardTLVInfo.h>
#include <aidl/android/hardware/emvco/ConfigType.h>
#include <aidl/android/hardware/emvco/IEmvco.h>
#include <aidl/android/hardware/emvco/IEmvcoContactlessCard.h>
#include <aidl/android/hardware/emvco/IEmvcoProfileDiscovery.h>
#include <aidl/android/hardware/emvco/IEmvcoTDA.h>
#include <aidl/android/hardware/emvco/Protocols.h>
#include <android-base/stringprintf.h>
#include <android/binder_auto_utils.h>
#include <android/binder_enums.h>
#include <android/binder_interface_utils.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <chrono>
#include <future>
#include <log/log.h>

using aidl::android::hardware::emvco::CardTLVInfo;
using aidl::android::hardware::emvco::ConfigType;
using aidl::android::hardware::emvco::DeactivationType;
using aidl::android::hardware::emvco::EmvcoEvent;
using aidl::android::hardware::emvco::EmvcoStatus;
using aidl::android::hardware::emvco::IEmvco;
using aidl::android::hardware::emvco::IEmvcoClientCallback;
using aidl::android::hardware::emvco::IEmvcoContactlessCard;
using aidl::android::hardware::emvco::IEmvcoProfileDiscovery;
using aidl::android::hardware::emvco::IEmvcoTDA;
using aidl::android::hardware::emvco::LedControl;
using aidl::android::hardware::emvco::Protocols;
using ndk::SpAIBinder;

#define NCI_SEND_PPSE                                                          \
  {                                                                            \
    0x00, 0xA4, 0x04, 0x00, 0x0E, 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59,    \
        0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00                         \
  }
#define EMVCO_POLLING_STARTED_EVT 4
#define MIN_VALID_DATA_SIZE 9
#define NCI_HEADER_SIZE 3

#define CT_CARD_TDA_ID 0x20
int8_t channel_id = -1;

const std::vector<uint8_t> nci_send_ppse = NCI_SEND_PPSE;
::ndk::ScopedAIBinder_DeathRecipient mDeathRecipient;
static std::promise<void> psse_cb_promise_;
static std::future<void> psse_cb_future_ = psse_cb_promise_.get_future();
unsigned long long start_ts_, end_ts_;

static std::vector<uint8_t> nci_send_loopback_;
static volatile bool is_aborted_ = false;
static volatile uint8_t pollingConfiguration = 0;
const int NFC_A_PASSIVE_POLL_MODE = 0;
const int NFC_B_PASSIVE_POLL_MODE = 1;
const int NFC_F_PASSIVE_POLL_MODE = 2;
const int NFC_VAS_PASSIVE_POLL_MODE = 3;

const int NFC_AB_PASSIVE_POLL_MODE_SUPPORTED = 3;
const int NFC_F_PASSIVE_POLL_MODE_SUPPORTED = 4;
const int NFC_ABF_PASSIVE_POLL_MODE_SUPPORTED = 7;
const int NFC_ABVAS_PASSIVE_POLL_MODE_SUPPORTED = 11;
const int NFC_ABFVAS_PASSIVE_POLL_MODE_SUPPORTED = 15;
const int pollProfileSelectionVal = 0b00000010;
int config_len = 0;

std::mutex data_mutex_;
std::mutex led_mutex_;
int32_t aidl_return;
std::shared_ptr<IEmvco> nxp_emvco_service_;
std::shared_ptr<IEmvcoTDA> nxp_emvco_ct_service_;
std::shared_ptr<IEmvcoContactlessCard> nxp_emvco_cl_service_;
std::shared_ptr<IEmvcoProfileDiscovery> nxp_emvco_prof_disc_service_;

class EmvcoClientCallback
    : public aidl::android::hardware::emvco::BnEmvcoClientCallback {
public:
  EmvcoClientCallback(
      const std::function<void(EmvcoEvent, EmvcoStatus)> &on_hal_event_cb,
      const std::function<void(const std::vector<uint8_t> &)> &on_nci_data_cb)
      : on_nci_data_cb_(on_nci_data_cb), on_hal_event_cb_(on_hal_event_cb) {}
  virtual ~EmvcoClientCallback() = default;

  ::ndk::ScopedAStatus sendEvent(EmvcoEvent event,
                                 EmvcoStatus event_status) override {
    ALOGI("%s ", __func__);
    if (!is_aborted_) {
      on_hal_event_cb_(event, event_status);
    }
    return ::ndk::ScopedAStatus::ok();
  };
  ::ndk::ScopedAStatus sendData(const std::vector<uint8_t> &data) override {
    ALOGI("%s before mutex ", __func__);
    if (!is_aborted_) {
      on_nci_data_cb_(data);
    }
    return ::ndk::ScopedAStatus::ok();
  };

private:
  std::function<void(const std::vector<uint8_t> &)> on_nci_data_cb_;
  std::function<void(EmvcoEvent, EmvcoStatus)> on_hal_event_cb_;
};

void signal_callback_handler(int signum) {
  ALOGI("%s Self test App abort requested, signum:%d", __func__, signum);
  is_aborted_ = true;
  if (nxp_emvco_cl_service_ != nullptr) {
    nxp_emvco_cl_service_->setEMVCoMode(pollingConfiguration, false);
  }
  psse_cb_promise_.set_value();
  exit(signum);
  ALOGI("%s Self test App aborted", __func__);
}
void setRFTechnologyMode(int modeType, bool isSet) {
  ALOGI("%s Before set pollingConfiguration:%d\n", __func__,
        pollingConfiguration);
  if (isSet) {
    pollingConfiguration = 1 << modeType | pollingConfiguration;
  } else {
    pollingConfiguration = ~(1 << modeType) & pollingConfiguration;
  }
  ALOGI("%s after set pollingConfiguration:%d\n", __func__,
        pollingConfiguration);
}

void EmvcoHalBinderDied(void *cookie) {
  ALOGI("EmvcoHalBinderDied");
  (void)cookie;
  AIBinder_unlinkToDeath(nxp_emvco_service_->asBinder().get(),
                         mDeathRecipient.get(), 0);
  exit(1);
  ALOGI("Self test App aborted due to EMVCo HAL crash");
}

int main(int argc, char **argv) {
  ABinderProcess_startThreadPool();
  try {
    ALOGI("%s Entered %d arguments", __func__, argc);
    for (int i = 0; i < argc; ++i) {
      ALOGI("%s argv:", argv[i]);
    }
    if (argc > 2) {
      if (strstr(argv[2], "A") != 0 || strstr(argv[2], "a") != 0) {
        setRFTechnologyMode(NFC_A_PASSIVE_POLL_MODE, true);
      }
      if (strstr(argv[2], "B") != 0 || strstr(argv[2], "b") != 0) {
        setRFTechnologyMode(NFC_B_PASSIVE_POLL_MODE, true);
      }
      if (strstr(argv[2], "F") != 0 || strstr(argv[2], "f") != 0) {
        setRFTechnologyMode(NFC_F_PASSIVE_POLL_MODE, true);
      }
      if (strstr(argv[2], "V") != 0 || strstr(argv[2], "v") != 0) {
        setRFTechnologyMode(NFC_VAS_PASSIVE_POLL_MODE, true);
      }
    } else {
      printf(
          "\n Select atleast one polling technolgy to enable EMVCo mode\n "
          "Example#1: \"./EMVCoAidlHalComplianceTest Type/interop A 600\" "
          "will enable Type A "
          "for polling with LED glow duration of 600ms \n Example#2: "
          "\"./EMVCoAidlHalComplianceTest Type/interop AB 600\" "
          "will enable Type/interop AB for polling with LED glow duration of "
          "600ms\n \n ");
      return 0;
    }

    if (pollingConfiguration == NFC_AB_PASSIVE_POLL_MODE_SUPPORTED ||
        pollingConfiguration == NFC_F_PASSIVE_POLL_MODE_SUPPORTED ||
        pollingConfiguration == NFC_ABF_PASSIVE_POLL_MODE_SUPPORTED ||
        pollingConfiguration == NFC_ABVAS_PASSIVE_POLL_MODE_SUPPORTED ||
        pollingConfiguration == NFC_ABFVAS_PASSIVE_POLL_MODE_SUPPORTED) {
      printf("\n Valid Technology selected for polling\n ");
    } else {
      printf(
          "\n Select supported polling technolgy (AB) to enable EMVCo mode\n "
          "Example: \"./EMVCoAidlHalComplianceTest Type/interop "
          "AB 600 \" will enable Type/interop AB for polling with LED glow "
          "duration of 600ms\n \n ");
      return 0;
    }

    signal(SIGINT, signal_callback_handler);

    const std::string instance =
        std::string() + IEmvco::descriptor + "/default";
    SpAIBinder binder(AServiceManager_waitForService(instance.c_str()));
    nxp_emvco_service_ = IEmvco::fromBinder(binder);

    mDeathRecipient = ::ndk::ScopedAIBinder_DeathRecipient(
        AIBinder_DeathRecipient_new(EmvcoHalBinderDied));
    AIBinder_linkToDeath(nxp_emvco_service_->asBinder().get(),
                         mDeathRecipient.get(), 0);

    nxp_emvco_service_->getEmvcoTDA(&nxp_emvco_ct_service_);
    nxp_emvco_service_->getEmvcoContactlessCard(&nxp_emvco_cl_service_);
    nxp_emvco_service_->getEmvcoProfileDiscoveryInterface(
        &nxp_emvco_prof_disc_service_);
    aidl::android::hardware::emvco::EmvcoStatus _aidl_return;
    int tempPollProfileSelection = pollProfileSelectionVal;
    while (tempPollProfileSelection != 0) {
      tempPollProfileSelection /= 10;
      ++config_len;
    }
    ALOGI("setByteConfig called with pollProfileSelectionVal:%d, config_len:%d",
          pollProfileSelectionVal, config_len);

    nxp_emvco_prof_disc_service_->setByteConfig(
        ConfigType::POLL_PROFILE_SEL, config_len, pollProfileSelectionVal,
        &_aidl_return);

    auto mCallback = ::ndk::SharedRefBase::make<EmvcoClientCallback>(
        [](auto event, auto status) {
          ALOGI("Event callback event:%d", event);
          (void)status;
          if (EMVCO_POLLING_STARTED_EVT == (int)event) {
            std::vector<aidl::android::hardware::emvco::EmvcoTDAInfo>
                contactCards;
            auto res =
                nxp_emvco_ct_service_->discoverTDA(nullptr, &contactCards);
            if (res.isOk() != 1 || res.getExceptionCode() != 0 ||
                res.getServiceSpecificError() != 0) {
              ALOGI("CT Feature not supported");
              psse_cb_promise_.set_value();
            } else {

              ALOGI("TDA size:%d", (int)contactCards.size());
              for (aidl::android::hardware::emvco::EmvcoTDAInfo contactCard :
                   contactCards) {
                ALOGI("TDA ID:%d", contactCard.id);
                ALOGI("TDA status:%d", contactCard.status);
                ALOGI("TDA numberOfProtocols:%d",
                      contactCard.numberOfProtocols);
                std::vector<Protocols> protocols = contactCard.protocols;
                for (Protocols protocol : protocols) {
                  ALOGI("TDA protocol:%d", protocol);
                }
                ALOGI("TDA numberOfCardInfo:%d", contactCard.numberOfCardInfo);
                std::vector<CardTLVInfo> cardTLVInfos = contactCard.cardTLVInfo;
                for (CardTLVInfo cardTLVInfo : cardTLVInfos) {
                  ALOGI("TDA cardTLVInfo.type:%d", cardTLVInfo.type);
                  ALOGI("TDA cardTLVInfo.length:%d", cardTLVInfo.length);
                  std::vector<uint8_t> values = cardTLVInfo.value;
                  for (uint8_t value : values) {
                    ALOGI("TDA cardTLVInfo.value:%d", value);
                  }
                }
              }
            }
          }
        },
        [](auto &in_data) {
          ALOGI("Data callback");
          (void)in_data;
        });
    bool register_status;
    EXPECT_TRUE((*(nxp_emvco_ct_service_))
                    .registerEMVCoEventListener(mCallback, &register_status)
                    .isOk());
    nxp_emvco_cl_service_->setEMVCoMode(pollingConfiguration, true);
    psse_cb_future_.wait();

  } catch (const std::length_error &e) {
    ALOGE("%s std::length_error", e.what());
  }
  ALOGI("TEST APP EXITED");
}