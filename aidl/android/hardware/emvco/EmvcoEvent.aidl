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

package android.hardware.emvco;


@VintfStability
@Backing(type="int")
enum EmvcoEvent {
    EMVCO_EVENT_OPEN_CPLT         = 0,
    EMVCO_EVENT_CLOSE_CPLT        = 1,
    EMVCO_EVENT_POST_INIT_CPLT    = 2,
    EMVCO_EVENT_PRE_DISCOVER_CPLT = 3,
    EMVCO_EVENT_HCI_NETWORK_RESET = 4,
    EMVCO_EVENT_ERROR             = 5,
    EMVCO_EVENT_START_CONFIG      = 6,
    EMVCO_EVENT_START_IN_PROGRESS = 7,
    EMVCO_EVENT_START_SUCCESS     = 8,
    EMVCO_EVENT_ACTIVATED         = 9,
    EMVCO_EVENT_STOP_CONFIG       = 10,
    EMVCO_EVENT_STOP_IN_PROGRESS  = 11,
    EMVCO_EVENT_STOP_SUCCESS      = 12,
    EMVCO_EVENT_STOPPED           = 13,
}
