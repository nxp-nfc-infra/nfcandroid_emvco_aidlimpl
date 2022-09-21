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
enum NfcState {
    /**
     * STATE_OFF event to notify NFC state off event to EMVCo HAL 
     * 
     */
    STATE_OFF              = 1,
    /**
     * STATE_TURNING_ON event to notify NFC state turning on event to EMVCo HAL 
     */
    STATE_TURNING_ON             = 1,
    /**
     * STATE_ON event to notify NFC state on event to EMVCo HAL 
     */
    STATE_ON             = 2,
    /**
     * STATE_TURNING_OFF event to notify NFC state turning OFF event to EMVCo HAL 
     */
    STATE_TURNING_OFF              = 3,

}
