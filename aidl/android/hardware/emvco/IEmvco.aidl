/******************************************************************************
 *
 *  Copyright 2022 NXP
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

package android.hardware.emvco;

import android.hardware.emvco.IEmvcoProfileDiscovery;
import android.hardware.emvco.IEmvcoContactlessCard;
import android.hardware.emvco.IEmvcoContactCard;

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

 /**
  * @brief Interface to EMVCo HAL.
  *
  * The android application calls this interface to get the EMVCo functionality specific interface.
  *
  */
@VintfStability
interface IEmvco {
    /**
    * @brief Provides the EMVCo mode switch HAL interface.
    *
    * @param  none
    *
    * @return IEmvcoProfileDiscovery EMVCo mode switch HAL interface
    *
    */
    IEmvcoProfileDiscovery getEmvcoProfileDiscoveryInterface();

    /**
    * @brief Provides the EMVCo contact less HAL interface.
    *
    * @param  none
    *
    * @return IEmvcoContactlessCard EMVCo contact less HAL interface
    *
    */
    IEmvcoContactlessCard getEmvcoContactlessCard();

    /**
    * @brief Provides the EMVCo contact HAL interface.
    *
    * @param  none
    *
    * @return IEmvcoContactCard EMVCo contact HAL interface
    *
    */
    IEmvcoContactCard getEmvcoContactCard();
}
/** @}*/