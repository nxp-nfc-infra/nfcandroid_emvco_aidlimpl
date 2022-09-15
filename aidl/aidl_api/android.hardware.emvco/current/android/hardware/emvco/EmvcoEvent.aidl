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
///////////////////////////////////////////////////////////////////////////////
// THIS FILE IS IMMUTABLE. DO NOT EDIT IN ANY CASE.                          //
///////////////////////////////////////////////////////////////////////////////

// This file is a snapshot of an AIDL file. Do not edit it manually. There are
// two cases:
// 1). this is a frozen version file - do not edit this in any case.
// 2). this is a 'current' file. If you make a backwards compatible change to
//     the interface (from the latest frozen version), the build system will
//     prompt you to update this file with `m <name>-update-api`.
//
// You must not make a backward incompatible change to any AIDL file built
// with the aidl_interface module type with versions property set. The module
// type is used to build AIDL files in a way that they can be used across
// independently updatable components of the system. If a device is shipped
// with such a backward incompatible change, it has a high risk of breaking
// later when a module using the interface is updated, e.g., Mainline modules.

package android.hardware.emvco;
@Backing(type="int") @VintfStability
enum EmvcoEvent {
  EMVCO_EVENT_OPEN_CPLT = 0,
  EMVCO_EVENT_CLOSE_CPLT = 1,
  EMVCO_EVENT_POST_INIT_CPLT = 2,
  EMVCO_EVENT_PRE_DISCOVER_CPLT = 3,
  EMVCO_EVENT_HCI_NETWORK_RESET = 4,
  EMVCO_EVENT_ERROR = 5,
  EMVCO_EVENT_START_CONFIG = 6,
  EMVCO_EVENT_START_IN_PROGRESS = 7,
  EMVCO_EVENT_START_SUCCESS = 8,
  EMVCO_EVENT_ACTIVATED = 9,
  EMVCO_EVENT_STOP_CONFIG = 10,
  EMVCO_EVENT_STOP_IN_PROGRESS = 11,
  EMVCO_EVENT_STOP_SUCCESS = 12,
  EMVCO_EVENT_STOPPED = 13,
}
