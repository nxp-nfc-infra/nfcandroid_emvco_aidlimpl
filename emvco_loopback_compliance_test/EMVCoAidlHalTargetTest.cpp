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

#define LOG_TAG "emvco_aidl_hal_test"

#include <aidl/Gtest.h>
#include <aidl/Vintf.h>
#include <aidl/android/hardware/emvco/BnEmvcoClientCallback.h>
#include <aidl/android/hardware/emvco/BnNxpEmvco.h>
#include <aidl/android/hardware/emvco/INxpEmvco.h>
#include <aidl/android/hardware/emvco/INxpEmvcoContactlessCard.h>
#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <android/binder_auto_utils.h>
#include <android/binder_enums.h>
#include <android/binder_interface_utils.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <binder/IServiceManager.h>
#include <chrono>
#include <future>
#include <log/log.h>
#include <math.h>
#include <mutex>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <time.h>
#include <unistd.h>

using aidl::android::hardware::emvco::EmvcoEvent;
using aidl::android::hardware::emvco::EmvcoStatus;
using aidl::android::hardware::emvco::IEmvcoClientCallback;
using aidl::android::hardware::emvco::INxpEmvco;
using aidl::android::hardware::emvco::INxpEmvcoContactlessCard;

using ndk::ScopedAStatus;
using ndk::SharedRefBase;
using ndk::SpAIBinder;

using android::getAidlHalInstanceNames;
using android::PrintInstanceNameToString;
using android::base::StringPrintf;
using ndk::enum_range;
using ndk::ScopedAStatus;
using ndk::SharedRefBase;
using ndk::SpAIBinder;

/* NCI Commands */

#define NCI_START_DISCOVERY                                                    \
  { 0x21, 0x03, 0x05, 0x02, 0x00, 0x01, 0x01, 0x01 }
#define NCI_STOP_DISCOVERY                                                     \
  { 0x21, 0x06, 0x01, 0x00 }
#define NCI_SEND_PPSE                                                          \
  {                                                                            \
    0x00, 0x00, 0x14, 0x00, 0xA4, 0x04, 0x00, 0x0E, 0x32, 0x50, 0x41, 0x59,    \
        0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00       \
  }

const std::vector<uint8_t> nci_start_discovery = NCI_START_DISCOVERY;
const std::vector<uint8_t> nci_stop_discovery = NCI_STOP_DISCOVERY;
const std::vector<uint8_t> nci_send_ppse = NCI_SEND_PPSE;

// 638 test case *7 APDU including start discovery + 638 discovery = 5104
#define PPSE_SEND_MAX_TIMES 5104
#define FRAGMENT_SZ 253

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

static int received_data_length_ = 0;
static char temp_[1000];
static int starting_index_ = 0;
const static int index_zero_ = 0;
static std::vector<uint8_t> nci_send_loopback_;
static volatile bool is_end_of_test_ = false;
static volatile bool is_frgamented_apdu_ = false;
static volatile bool is_aborted_ = false;
static volatile uint8_t pollingConfiguration = 0;

std::mutex data_mutex_;
int32_t aidl_return;

std::shared_ptr<INxpEmvco> nxp_emvco_service_;
std::shared_ptr<INxpEmvcoContactlessCard> nxp_emvco_cl_service_;

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
    on_hal_event_cb_(event, event_status);
    return ::ndk::ScopedAStatus::ok();
  };
  ::ndk::ScopedAStatus sendData(const std::vector<uint8_t> &data) override {
    ALOGI("%s before mutex ", __func__);
    on_nci_data_cb_(data);
    return ::ndk::ScopedAStatus::ok();
  };

private:
  std::function<void(const std::vector<uint8_t> &)> on_nci_data_cb_;
  std::function<void(EmvcoEvent, EmvcoStatus)> on_hal_event_cb_;
};

static bool isEndOfTest(std::vector<unsigned char> &data) {
  ALOGI("%s\n", __func__);
  bool isEOT = false;
  if (data.at(0) == 0x60 && data.at(1) == 0x08) {
    ALOGI("Device Error");
    switch (data.at(3)) {
    case 0xB0:
      ALOGI("Device lost - transmission error\n");
      starting_index_ = 0;
      is_frgamented_apdu_ = false;
      isEOT = true;
      break;
    case 0xB1:
      ALOGI("Device lost - protocol error\n");
      starting_index_ = 0;
      is_frgamented_apdu_ = false;
      isEOT = true;
      break;
    case 0xB2:
      ALOGI("Device lost - timeout error\n");
      starting_index_ = 0;
      is_frgamented_apdu_ = false;
      isEOT = true;
      break;
    default:
      ALOGI("Default\n");
      break;
    }
  } else if ((data.at(0) == 0x61) && (data.at(1) == 0x06)) {
    ALOGI("RF Deactivated\n");
    isEOT = true;
  } else if ((received_data_length_ > 7) &&
             (data.at(3) == 0x00 && data.at(4) == 0x70 && data.at(5) == 0x04 &&
              data.at(6) == 0x04 && data.at(7) == 0x00)) {
    ALOGI("Device lost APDU received\n");
    isEOT = true;
  }
  return isEOT;
}

static std::vector<uint8_t> getNCILoopbackData(uint8_t packetBoundaryFlag,
                                               char *pTemp, int dataLength) {
  ALOGI("%s\n dataLength", __func__);
  std::vector<uint8_t> nci_send_loopback_;
  nci_send_loopback_.insert(nci_send_loopback_.begin(), packetBoundaryFlag);
  nci_send_loopback_.insert(nci_send_loopback_.begin() + 1, 0x00);
  nci_send_loopback_.insert(nci_send_loopback_.begin() + 2, dataLength);
  nci_send_loopback_.insert(nci_send_loopback_.begin() + 3, pTemp,
                            pTemp + dataLength);
  return nci_send_loopback_;
}

static void
send(std::shared_ptr<INxpEmvcoContactlessCard> nxp_emvco_cl_service_,
     const std::vector<uint8_t> data, int32_t aidl_return,
     std::string dataTag) {
  ALOGI("%s\n transceive data:%s", __func__, dataTag.c_str());
  nxp_emvco_cl_service_->transceive(data, &aidl_return);
}

static void
transceive(std::shared_ptr<INxpEmvcoContactlessCard> nxp_emvco_cl_service_,
           uint8_t packetBoundaryFlag, int32_t dataLength, char *pTemp,
           char *orgpTemp, std::string dataTag, int32_t aidl_return) {
  ALOGI("%s %s\n", __func__, dataTag.c_str());
  if (packetBoundaryFlag == 0x10) {
    while (dataLength > FRAGMENT_SZ) {
      ALOGI("fragment Max APDU size \n");
      send(nxp_emvco_cl_service_, getNCILoopbackData(0x10, pTemp, FRAGMENT_SZ),
           aidl_return, "LOOPBACK_APDU");
      dataLength -= FRAGMENT_SZ;
      pTemp += FRAGMENT_SZ;
    }
    if (dataLength > 0) {
      ALOGI("fragmented loopback data written \n");
      send(nxp_emvco_cl_service_, getNCILoopbackData(0x00, pTemp, dataLength),
           aidl_return, "LOOPBACK_APDU");
    }
    pTemp = orgpTemp;
  } else if (packetBoundaryFlag == 0x00) {
    if (dataLength > 0) {
      ALOGI("loopback data written\n");
      send(nxp_emvco_cl_service_, getNCILoopbackData(0x00, pTemp, dataLength),
           aidl_return, "LOOPBACK_APDU");
    }
  }
}

int hexToDecimal(char *hex) {
  long long decimal = 0, base = 1;
  int i = 0, len;
  len = (size_t)sizeof(unsigned char);
  for (i = len; i >= 0; i--) {
    if (hex[i] >= '0' && hex[i] <= '9') {
      decimal += (hex[i] - 48) * base;
      base *= 16;
    } else if (hex[i] >= 'A' && hex[i] <= 'F') {
      decimal += (hex[i] - 55) * base;
      base *= 16;
    } else if (hex[i] >= 'a' && hex[i] <= 'f') {
      decimal += (hex[i] - 87) * base;
      base *= 16;
    }
  }
  ALOGI("Decimal:%lld \n", decimal);
  return decimal;
}
void signal_callback_handler(int signum) {
  ALOGI("%s Self test App abort requested, signum:%d", __func__, signum);
  if (nxp_emvco_cl_service_ != nullptr) {
    nxp_emvco_cl_service_->doSetEMVCoMode(pollingConfiguration, false);
  }
  exit(signum);
  ALOGI("%s Self test App aborted", __func__);
}
int main(int argc, char **argv) {
  ABinderProcess_startThreadPool();

  ALOGI("%s Entered %d arguments", __func__, argc);
  for (int i = 0; i < argc; ++i) {
    ALOGI("%s argv:", argv[i]);
  }
  if (argc == 3) {
    if (strstr(argv[2], "A") != NULL || strstr(argv[2], "a") != NULL) {
      pollingConfiguration = 1;
    }
    if (strstr(argv[2], "B") != NULL || strstr(argv[2], "b") != NULL) {
      pollingConfiguration = 2;
    }
    if (strstr(argv[2], "AB") != NULL || strstr(argv[2], "ab") != NULL) {
      pollingConfiguration = 3;
    }
  } else {
    printf("\n Select atleast one polling technolgy to enable EMVCo mode\n "
           "Example#1: \"./EMVCoAidlHalComplianceTest Type A\" will enable Type A "
           "for polling \n Example#2: \"./EMVCoAidlHalComplianceTest Type AB\" "
           "will enable Type AB for polling \n \n ");
    return 0;
  }

  if (pollingConfiguration == 0) {
    printf(
        "\n Select supported polling technolgy (A/B/AB) to enable EMVCo mode\n "
        "Example#1: \"./EMVCoAidlHalComplianceTest Type A\" will enable "
        "Type A for polling \n Example#2: \"./EMVCoAidlHalComplianceTest Type "
        "AB\" will enable Type AB for polling \n \n ");
    return 0;
  }
  signal(SIGINT, signal_callback_handler);

  char length[3];
  char *pLength = length;
  char *orgpTemp = temp_;
  char *pTemp = temp_;

  std::vector<std::future<void>> psse_cb_future;

  std::chrono::milliseconds timeout{CALLBACK_TIMEOUT};

  for (int i = 0; i < PPSE_SEND_MAX_TIMES; i++) {
    std::promise<void> ppse_promise;
    psse_cb_future.push_back(ppse_promise.get_future());
    psse_cb_promise_.push_back(std::move(ppse_promise));
  }

  const std::string instance =
      std::string() + INxpEmvco::descriptor + "/default";
  SpAIBinder binder(AServiceManager_waitForService(instance.c_str()));
  nxp_emvco_service_ = INxpEmvco::fromBinder(binder);

  nxp_emvco_service_->getNxpEmvcoContactlessCard(&nxp_emvco_cl_service_);

  auto mCallback = ::ndk::SharedRefBase::make<EmvcoClientCallback>(
      [](auto event, auto status) {
        ALOGI("Event callback");
        (void)event;
        (void)status;
      },
      [&pTemp, &orgpTemp, &pLength, &length](auto &in_data) {
        ALOGI("Data callback");
        const std::lock_guard<std::mutex> lock(data_mutex_);
        ALOGI("Data callback after mutex lock");

        try {
          std::vector<unsigned char> data(in_data.begin(), in_data.end());
          received_data_length_ = data.size();
          ALOGI("Received data.size():%d", received_data_length_);

          if (isEndOfTest(data)) {
            ALOGI("End of Test\n");
            is_end_of_test_ = true;
            psse_cb_promise_.at(APDU_COUNT).set_value();
          } else if (data.at(0) == 0x10) {
            sprintf(length, "%x", data.at(2));
            received_data_length_ = hexToDecimal(pLength);
            ALOGI("Fragment APDU received_data_length_:%d",
                  received_data_length_);
            is_frgamented_apdu_ = true;
            memcpy(&temp_[starting_index_], &data.at(3), received_data_length_);
            starting_index_ += received_data_length_;
          } else if (data.at(0) == 0x00) {

            sprintf(length, "%x", data.at(2));
            received_data_length_ = hexToDecimal(pLength);
            ALOGI("Non Fragment APDU received_data_length_:%d",
                  received_data_length_);
            if (received_data_length_ < 1) {
              ALOGI("Not valid Non fragment APDU received length less than 1");
              starting_index_ = 0;
              return;
            }

            if (is_frgamented_apdu_) {
              ALOGI("Continuation of Fragment APDU");
              memcpy(&temp_[starting_index_], &data.at(3),
                     received_data_length_);
              transceive(nxp_emvco_cl_service_, 0x10,
                         starting_index_ + received_data_length_ - 2, pTemp,
                         orgpTemp, "CMD_APDU", aidl_return);

              starting_index_ = 0;
              is_frgamented_apdu_ = false;
            } else {
              memcpy(&temp_[index_zero_], &data.at(3), received_data_length_);
              transceive(nxp_emvco_cl_service_, 0x00, received_data_length_ - 2,
                         pTemp, orgpTemp, "CMD_APDU", aidl_return);
            }
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
                  .doRegisterEMVCoEventListener(mCallback, &register_status)
                  .isOk());
  nxp_emvco_cl_service_->doSetEMVCoMode(pollingConfiguration, true);

  psse_cb_future.at(APDU_COUNT).wait();
  APDU_COUNT++;

  send(nxp_emvco_cl_service_, nci_send_ppse, aidl_return, "NCI_SEND_PPSE");
  psse_cb_future.at(APDU_COUNT).wait();
  APDU_COUNT++;

  while (!is_aborted_) {
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
      received_data_length_ = 0;
    }

    send(nxp_emvco_cl_service_, nci_stop_discovery, aidl_return,
         "NCI_STOP_DISCOVERY");
    psse_cb_future.at(APDU_COUNT).wait_for(5 * timeout);
    ++APDU_COUNT;
    is_end_of_test_ =
        false; // Reset is_end_of_test_ to send the PPSE in loop next time
    ALOGI("NCI_STOP_DISCOVERY completed");
  }
  nxp_emvco_cl_service_->doSetEMVCoMode(pollingConfiguration, false);

  ALOGI("TEST APP EXITED");
}
