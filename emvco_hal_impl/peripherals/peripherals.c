/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
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