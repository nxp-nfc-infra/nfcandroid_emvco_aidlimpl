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
enum EmvcoStatus {
    /**
     * Default status when Emvco Event with status OK.
     */
    EMVCO_STATUS_OK = 0,
    /**
     * Generic error.
     */
    EMVCO_STATUS_FAILED = 1,
    /**
     * Transport error.
     */
    EMVCO_STATUS_ERR_TRANSPORT = 2,
    /**
     * Command timeout error.
     */
    EMVCO_STATUS_ERR_CMD_TIMEOUT = 3,
    /**
     * Refused error when command is rejected.
     */
    EMVCO_STATUS_REFUSED = 4,
}
