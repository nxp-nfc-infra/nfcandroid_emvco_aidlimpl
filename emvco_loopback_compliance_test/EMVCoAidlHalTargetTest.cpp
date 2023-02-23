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

#define LOG_TAG "emvco_aidl_hal_test"

#include <aidl/Gtest.h>
#include <aidl/android/hardware/emvco/BnEmvco.h>
#include <aidl/android/hardware/emvco/BnEmvcoClientCallback.h>
#include <aidl/android/hardware/emvco/IEmvco.h>
#include <aidl/android/hardware/emvco/IEmvcoContactlessCard.h>
#include <android-base/stringprintf.h>
#include <android/binder_auto_utils.h>
#include <android/binder_enums.h>
#include <android/binder_interface_utils.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <chrono>
#include <future>
#include <log/log.h>

using aidl::android::hardware::emvco::EmvcoEvent;
using aidl::android::hardware::emvco::EmvcoStatus;
using aidl::android::hardware::emvco::IEmvco;
using aidl::android::hardware::emvco::IEmvcoClientCallback;
using aidl::android::hardware::emvco::IEmvcoContactlessCard;
using ndk::SpAIBinder;


/* NCI Commands */
#define NCI_START_DISCOVERY                                                    \
  { 0x21, 0x03, 0x05, 0x02, 0x00, 0x01, 0x01, 0x01 }
#define NCI_STOP_DISCOVERY_IDLE                                                \
  { 0x21, 0x06, 0x01, 0x00 }
#define NCI_STOP_DISCOVERY_DISCOVER                                            \
  { 0x21, 0x06, 0x01, 0x03 }
#define NCI_SEND_PPSE                                                          \
  {                                                                            \
    0x00, 0xA4, 0x04, 0x00, 0x0E, 0x32, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59,    \
        0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00                         \
  }

const std::vector<uint8_t> nci_start_discovery = NCI_START_DISCOVERY;
const std::vector<uint8_t> nci_stop_discovery_idle = NCI_STOP_DISCOVERY_IDLE;
const std::vector<uint8_t> nci_stop_discovery_discover =
    NCI_STOP_DISCOVERY_DISCOVER;
const std::vector<uint8_t> nci_send_ppse = NCI_SEND_PPSE;

::ndk::ScopedAIBinder_DeathRecipient mDeathRecipient;

// 638 test case *7 APDU including start discovery + 638 discovery = 5104
#define PPSE_SEND_MAX_TIMES 5104

#define PRINT_CAPDU(y, z)                                                      \
  {                                                                            \
    int loop;                                                                  \
    ALOGI("PCD  > ");                                                          \
    for (loop = 3; loop < z; loop++)                                           \
      ALOGI("%02.02X ", y[loop]);                                              \
  }

constexpr static int CALLBACK_TIMEOUT = 100;
volatile static int APDU_COUNT = 0;
static std::vector<std::promise<void>> psse_cb_promise_;
unsigned long long start_ts_, end_ts_;

static std::vector<uint8_t> nci_send_loopback_;
static volatile bool is_end_of_test_ = false;
static volatile bool is_aborted_ = false;
static volatile bool is_removal_procedure = false;
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

std::mutex data_mutex_;
int32_t aidl_return;

std::shared_ptr<IEmvco> nxp_emvco_service_;
std::shared_ptr<IEmvcoContactlessCard> nxp_emvco_cl_service_;

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

static bool isEndOfTest(std::vector<unsigned char> &data, int received_size) {
  ALOGI("%s\n", __func__);
  bool isEOT = false;
  if ((data.at(0) == 0x61) && (data.at(1) == 0x06)) {
    ALOGI("RF Deactivated received_size:%d\n", received_size);
    isEOT = true;
  } else if ((received_size >= 5) &&
             (data.at(0) == 0x00 &&
              (data.at(1) == 0x70 || data.at(1) == 0x72) &&
              data.at(2) == 0x04 && data.at(3) == 0x04 && data.at(4) == 0x00)) {
    ALOGI("Device lost APDU received with NCI header\n");
    if (data.at(1) == 0x70) {
      is_removal_procedure = true;
    }
    isEOT = true;
  }
  return isEOT;
}

static void send(std::shared_ptr<IEmvcoContactlessCard> nxp_emvco_cl_service_,
                 const std::vector<uint8_t> data, int32_t aidl_return,
                 std::string dataTag) {
  ALOGI("%s\n transceive data:%s", __func__, dataTag.c_str());
  nxp_emvco_cl_service_->transceive(data, &aidl_return);
}


void signal_callback_handler(int signum) {
  ALOGI("%s Self test App abort requested, signum:%d", __func__, signum);
  is_aborted_ = true;
  if (nxp_emvco_cl_service_ != nullptr) {
    nxp_emvco_cl_service_->setEMVCoMode(pollingConfiguration, false);
  }
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

  ALOGI("%s Entered %d arguments", __func__, argc);
  for (int i = 0; i < argc; ++i) {
    ALOGI("%s argv:", argv[i]);
  }
  if (argc == 3) {
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
    printf("\n Select atleast one polling technolgy to enable EMVCo mode\n "
           "Example#1: \"./EMVCoAidlHalComplianceTest Type A\" will enable Type A "
           "for polling \n Example#2: \"./EMVCoAidlHalComplianceTest Type AB\" "
           "will enable Type AB for polling \n \n ");
    return 0;
  }

  if (pollingConfiguration == NFC_AB_PASSIVE_POLL_MODE_SUPPORTED ||
      pollingConfiguration == NFC_F_PASSIVE_POLL_MODE_SUPPORTED ||
      pollingConfiguration == NFC_ABF_PASSIVE_POLL_MODE_SUPPORTED ||
      pollingConfiguration == NFC_ABVAS_PASSIVE_POLL_MODE_SUPPORTED ||
      pollingConfiguration == NFC_ABFVAS_PASSIVE_POLL_MODE_SUPPORTED) {
    printf("\n Valid Technology selected for polling\n ");
  } else {
    printf("\n Select supported polling technolgy (AB) to enable EMVCo mode\n "
           "Example: \"./EMVCoAidlHalComplianceTest Type "
           "AB\" will enable Type AB for polling \n \n ");
    return 0;
  }
  signal(SIGINT, signal_callback_handler);

  std::vector<std::future<void>> psse_cb_future;

  std::chrono::milliseconds timeout{CALLBACK_TIMEOUT};

  for (int i = 0; i < PPSE_SEND_MAX_TIMES; i++) {
    std::promise<void> ppse_promise;
    psse_cb_future.push_back(ppse_promise.get_future());
    psse_cb_promise_.push_back(std::move(ppse_promise));
  }

  const std::string instance = std::string() + IEmvco::descriptor + "/default";
  SpAIBinder binder(AServiceManager_waitForService(instance.c_str()));
  nxp_emvco_service_ = IEmvco::fromBinder(binder);

  mDeathRecipient = ::ndk::ScopedAIBinder_DeathRecipient(
      AIBinder_DeathRecipient_new(EmvcoHalBinderDied));
  AIBinder_linkToDeath(nxp_emvco_service_->asBinder().get(),
                       mDeathRecipient.get(), 0);

  nxp_emvco_service_->getEmvcoContactlessCard(&nxp_emvco_cl_service_);

  auto mCallback = ::ndk::SharedRefBase::make<EmvcoClientCallback>(
      [](auto event, auto status) {
        ALOGI("Event callback");
        (void)event;
        (void)status;
      },
      [](auto &in_data) {
        ALOGI("Data callback");
        const std::lock_guard<std::mutex> lock(data_mutex_);
        ALOGI("Data callback after mutex lock");
        try {
          std::vector<uint8_t> data(in_data.begin(), in_data.end());
          int received_size = data.size();
          if (isEndOfTest(data, received_size)) {
            ALOGI("End of Test\n");
            is_end_of_test_ = true;
            psse_cb_promise_.at(APDU_COUNT).set_value();
          } else if ((data.at(0) == 0x00) || (data.at(0) == 0x10)) {
            send(nxp_emvco_cl_service_, data, aidl_return, "LOOPBACK_APDU");
          } else if (data.at(0) == 0x61 &&
                     data.at(1) == 0x05) { // RF_ACTIVATION_NTF - 0x61 && 0x05
            ALOGI("RF_ACTIVATION_NTF VERIFIED");
            psse_cb_promise_.at(APDU_COUNT).set_value();
          }
        } catch (const std::future_error &e) {
          ALOGE("%s data future_error", e.what());
        }
      });
  bool register_status;
  EXPECT_TRUE((*(nxp_emvco_cl_service_))
                  .registerEMVCoEventListener(mCallback, &register_status)
                  .isOk());
  nxp_emvco_cl_service_->setEMVCoMode(pollingConfiguration, true);

  psse_cb_future.at(APDU_COUNT).wait();
  APDU_COUNT++;

  send(nxp_emvco_cl_service_, nci_send_ppse, aidl_return, "NCI_SEND_PPSE");
  psse_cb_future.at(APDU_COUNT).wait();
  APDU_COUNT++;

  while (true) {
    // NCI_START_DISCOVERY
    if (is_end_of_test_ == false) {
      send(nxp_emvco_cl_service_, nci_start_discovery, aidl_return,
           "NCI_START_DISCOVERY");
      ALOGI("\n Loopback application running - waiting for test equipment "
            "discovery ...\n");
      psse_cb_future.at(APDU_COUNT).wait();
      APDU_COUNT++;

      // NCI_SEND_PPSE
      ALOGI("NCI_SEND_PPSE COUNT=%d", APDU_COUNT);
      struct timespec tm;
      clock_gettime(CLOCK_MONOTONIC, &tm);
      start_ts_ = tm.tv_nsec * 1e-3 + tm.tv_sec * 1e+6;
      ALOGI("PPSE response sent at:%llu", start_ts_);
      send(nxp_emvco_cl_service_, nci_send_ppse, aidl_return, "NCI_SEND_PPSE");
      psse_cb_future.at(APDU_COUNT).wait();
      APDU_COUNT++;
      clock_gettime(CLOCK_MONOTONIC, &tm);
      end_ts_ = tm.tv_nsec * 1e-3 + tm.tv_sec * 1e+6;
      ALOGI("PPSE response received at:%llu", end_ts_);
      ALOGI("PPSE command and response duration :%llu microsec",
            end_ts_ - start_ts_);

    } else {
      ALOGI("EOT RECEIVED ");
    }
    if (is_removal_procedure) {
      is_removal_procedure = false;
      send(nxp_emvco_cl_service_, nci_stop_discovery_discover, aidl_return,
           "NCI_STOP_DISCOVERY_DISCOVER_DISCOVER");
    } else {
      send(nxp_emvco_cl_service_, nci_stop_discovery_idle, aidl_return,
           "NCI_STOP_DISCOVERY_IDLE");
      usleep(15000);
    }
    psse_cb_future.at(APDU_COUNT).wait_for(5 * timeout);
    ++APDU_COUNT;
    is_end_of_test_ =
        false; // Reset is_end_of_test_ to send the PPSE in loop next time
    ALOGI("NCI_STOP_DISCOVERY completed");
  }
  nxp_emvco_cl_service_->setEMVCoMode(pollingConfiguration, false);

  ALOGI("TEST APP EXITED");
}
