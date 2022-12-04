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
#include <emvco_log.h>
#include <emvco_status.h>
#include <emvco_tml.h>

void led_switch_control(emvco_status_t emvco_status) {
  LOG_EMVCOHAL_D("%s emvco_status:%d", __func__, emvco_status);
  EMVCO_STATUS status = EMVCO_STATUS_FAILED;
  switch (emvco_status) {
  case EMVCO_MODE_ON: {
    status = tml_ioctl(GreenLedOn);
    if (EMVCO_STATUS_SUCCESS == status) {
      LOG_EMVCOHAL_D("GREEN LED ON - SUCCESS\n");
    } else {
      LOG_EMVCOHAL_D("GREEN LED ON - FAILED\n");
    }
    break;
  }
  case EMVCO_MODE_OFF: {
    status = tml_ioctl(GreenLedOff);
    if (EMVCO_STATUS_SUCCESS == status) {
      LOG_EMVCOHAL_D("GREEN LED OFF - SUCCESS\n");
    } else {
      LOG_EMVCOHAL_D("GREEN LED OFF  - FAILED\n");
    }
    break;
  }
  default:
    break;
  }
}