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
#include "emvco_tml_i2c.h"
#include <emvco_log.h>
#include <emvco_status.h>
#include <emvco_tml.h>

EMVCO_STATUS led_switch_control(led_control_t led_control) {
  LOG_EMVCOHAL_D("%s led_control:%d", __func__, led_control);
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  switch (led_control) {
  case GREEN_LED_ON: {
    status = tml_ioctl(GreenLedOn);
    if (EMVCO_STATUS_SUCCESS == status) {
      LOG_EMVCOHAL_D("GREEN LED ON - SUCCESS\n");
    } else {
      LOG_EMVCOHAL_D("GREEN LED ON - FAILED\n");
    }
    break;
  }
  case GREEN_LED_OFF: {
    status = tml_ioctl(GreenLedOff);
    if (EMVCO_STATUS_SUCCESS == status) {
      LOG_EMVCOHAL_D("GREEN LED OFF - SUCCESS\n");
    } else {
      LOG_EMVCOHAL_D("GREEN LED OFF  - FAILED\n");
    }
    break;
  }
  case RED_LED_ON: {
    status = tml_ioctl(RedLedOn);
    if (EMVCO_STATUS_SUCCESS == status) {
      LOG_EMVCOHAL_D("RED LED ON - SUCCESS\n");
    } else {
      LOG_EMVCOHAL_D("RED LED ON - FAILED\n");
    }
    break;
  }
  case RED_LED_OFF: {
    status = tml_ioctl(RedLedOff);
    if (EMVCO_STATUS_SUCCESS == status) {
      LOG_EMVCOHAL_D("RED LED OFF - SUCCESS\n");
    } else {
      LOG_EMVCOHAL_D("RED LED OFF  - FAILED\n");
    }
    break;
  }
  default:
    break;
  }
  return status;
}