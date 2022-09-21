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

#ifndef _PHNXPNCIHAL_ADAPTATION_H_
#define _PHNXPNCIHAL_ADAPTATION_H_

#ifdef __cplusplus
extern "C" {
#endif
typedef uint8_t nfc_event_t;
typedef uint8_t nfc_status_t;

/*
 * The callback passed in from the NFC stack that the HAL
 * can use to pass events back to the stack.
 */
typedef void(nfc_stack_callback_t)(nfc_event_t event,
                                   nfc_status_t event_status);

/*
 * The callback passed in from the NFC stack that the HAL
 * can use to pass incomming data to the stack.
 */
typedef void(nfc_stack_data_callback_t)(uint16_t data_len, uint8_t *p_data);

typedef void(nfc_state_change_callback_t)(bool enableNfc);

enum {
  HAL_NFC_OPEN_CPLT_EVT = 0u,
  HAL_NFC_CLOSE_CPLT_EVT = 1u,
  HAL_NFC_POST_INIT_CPLT_EVT = 2u,
  HAL_NFC_PRE_DISCOVER_CPLT_EVT = 3u,
  HAL_NFC_REQUEST_CONTROL_EVT = 4u,
  HAL_NFC_RELEASE_CONTROL_EVT = 5u,
  HAL_NFC_ERROR_EVT = 6u,
  HAL_HCI_NETWORK_RESET = 7u,
};
enum {
  EMVCO_EVENT_START_CONFIG = 6u,
  EMVCO_EVENT_START_IN_PROGRESS = 7u,
  EMVCO_EVENT_START_SUCCESS = 8u,
  EMVCO_EVENT_ACTIVATED = 9u,
  EMVCO_EVENT_STOP_CONFIG = 10u,
  EMVCO_EVENT_STOP_IN_PROGRESS = 11u,
  EMVCO_EVENT_STOP_SUCCESS = 12u,
  EMVCO_EVENT_STOPPED = 13u,
  EMVCO_EVENT_UNKNOWN = 14u,
};
enum {
  HAL_NFC_STATUS_OK = 0u,
  HAL_NFC_STATUS_FAILED = 1u,
  HAL_NFC_STATUS_ERR_TRANSPORT = 2u,
  HAL_NFC_STATUS_ERR_CMD_TIMEOUT = 3u,
  HAL_NFC_STATUS_REFUSED = 4u,
};
/**
 * Every device data structure must begin with hw_device_t
 * followed by module specific public methods and attributes.
 */
typedef struct hw_device_t {
  /** tag must be initialized to HARDWARE_DEVICE_TAG */
  uint32_t tag;

  /**
   * Version of the module-specific device API. This value is used by
   * the derived-module user to manage different device implementations.
   *
   * The module user is responsible for checking the module_api_version
   * and device version fields to ensure that the user is capable of
   * communicating with the specific module implementation.
   *
   * One module can support multiple devices with different versions. This
   * can be useful when a device interface changes in an incompatible way
   * but it is still necessary to support older implementations at the same
   * time. One such example is the Camera 2.0 API.
   *
   * This field is interpreted by the module user and is ignored by the
   * HAL interface itself.
   */
  uint32_t version;

  /** reference to the module this device belongs to */
  struct hw_module_t *module;

  /** padding reserved for future use */
#ifdef __LP64__
  uint64_t reserved[12];
#else
  uint32_t reserved[12];
#endif

  /** Close this device */
  int (*close)(struct hw_device_t *device);

} hw_device_t;
typedef struct nfc_nci_device {
  /**
   * Common methods of the NFC NCI device.  This *must* be the first member of
   * nfc_nci_device_t as users of this structure will cast a hw_device_t to
   * nfc_nci_device_t pointer in contexts where it's known the hw_device_t
   * references a nfc_nci_device_t.
   */
  struct hw_device_t common;
  /*
   * (*open)() Opens the NFC controller device and performs initialization.
   * This may include patch download and other vendor-specific initialization.
   *
   * If open completes successfully, the controller should be ready to perform
   * NCI initialization - ie accept CORE_RESET and subsequent commands through
   * the write() call.
   *
   * If open() returns 0, the NCI stack will wait for a HAL_NFC_OPEN_CPLT_EVT
   * before continuing.
   *
   * If open() returns any other value, the NCI stack will stop.
   *
   */
  int (*open)(const struct nfc_nci_device *p_dev, nfc_stack_callback_t *p_cback,
              nfc_stack_data_callback_t *p_data_cback);

  /*
   * (*write)() Performs an NCI write.
   *
   * This method may queue writes and return immediately. The only
   * requirement is that the writes are executed in order.
   */
  int (*write)(const struct nfc_nci_device *p_dev, uint16_t data_len,
               const uint8_t *p_data);

  /*
   * (*core_initialized)() is called after the CORE_INIT_RSP is received from
   * the NFCC. At this time, the HAL can do any chip-specific configuration.
   *
   * If core_initialized() returns 0, the NCI stack will wait for a
   * HAL_NFC_POST_INIT_CPLT_EVT before continuing.
   *
   * If core_initialized() returns any other value, the NCI stack will continue
   * immediately.
   */
  int (*core_initialized)(const struct nfc_nci_device *p_dev,
                          uint8_t *p_core_init_rsp_params);

  /*
   * (*pre_discover)() Is called every time before starting RF discovery.
   * It is a good place to do vendor-specific configuration that must be
   * performed every time RF discovery is about to be started.
   *
   * If pre_discover() returns 0, the NCI stack will wait for a
   * HAL_NFC_PRE_DISCOVER_CPLT_EVT before continuing.
   *
   * If pre_discover() returns any other value, the NCI stack will start
   * RF discovery immediately.
   */
  int (*pre_discover)(const struct nfc_nci_device *p_dev);

  /*
   * (*close)() Closed the NFC controller. Should free all resources.
   */
  int (*close)(const struct nfc_nci_device *p_dev);

  /*
   * (*control_granted)() Grant HAL the exclusive control to send NCI commands.
   * Called in response to HAL_REQUEST_CONTROL_EVT.
   * Must only be called when there are no NCI commands pending.
   * HAL_RELEASE_CONTROL_EVT will notify when HAL no longer needs exclusive
   * control.
   */
  int (*control_granted)(const struct nfc_nci_device *p_dev);

  /*
   * (*power_cycle)() Restart controller by power cyle;
   * HAL_OPEN_CPLT_EVT will notify when operation is complete.
   */
  int (*power_cycle)(const struct nfc_nci_device *p_dev);
} nfc_nci_device_t;

typedef struct {
  struct nfc_nci_device nci_device;

  /* Local definitions */
} pn547_dev_t;

/* NXP HAL functions */
int phNxpNciHal_open(nfc_stack_callback_t *p_cback,
                     nfc_stack_data_callback_t *p_data_cback,
                     nfc_state_change_callback_t *p_nfc_state_cback);
int phNxpNciHal_MinOpen();
int phNxpNciHal_write(uint16_t data_len, const uint8_t *p_data);
int phNxpNciHal_write_internal(uint16_t data_len, const uint8_t *p_data);
int phNxpNciHal_core_initialized(uint16_t core_init_rsp_len,
                                 uint8_t *p_core_init_rsp_params);
int phNxpNciHal_pre_discover(void);
int phNxpNciHal_close(bool);
int phNxpNciHal_configDiscShutdown(void);
int phNxpNciHal_control_granted(void);
int phNxpNciHal_power_cycle(void);
int phNxpNciHal_ioctl(long arg, void *p_data);
void phNxpNciHal_do_factory_reset(void);
int phNxpNciHal_Minclose(void);
void phNxpNciHal_doSetEMVCoMode(const int8_t emvco_config,
                                bool_t in_isStartEMVCo);
void phNxpNciHal_handleNfcStateChanged(int32_t nfc_state);

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
#endif /* _PHNXPNCIHAL_ADAPTATION_H_ */
