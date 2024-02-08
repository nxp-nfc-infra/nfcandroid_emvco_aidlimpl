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

package android.hardware.emvco;

/** \addtogroup EMVCO_HAL_API_INTERFACE
 *  @{
 */

/**
 * @brief EMVCo Hal return status code as below
 */
@VintfStability
@Backing(type="int")
enum EmvcoStatus {
    /**
     * Indicates success status.
     */
    EMVCO_STATUS_OK = 0,

    /**
     * Indicates invalid input parameter status.
     */
    EMVCO_STATUS_INVALID_PARAMETER = 1,

    /**
     * Device specifier/handle value is invalid for the operation
     */
    EMVCO_STATUS_INVALID_DEVICE = 2,

    /**
     * A non-blocking function returns this immediately to indicate
     * that an internal operation is in progress
     */
    EMVCO_STATUS_PENDING = 3,

    /**
     * This Layer is Not initialized, hence initialization required.
     */
    EMVCO_STATUS_NOT_INITIALISED = 4,

    /**
     * The Layer is already initialized, hence initialization repeated.
     */
    EMVCO_STATUS_ALREADY_INITIALISED = 5,

    /**
     * Feature not supported
     */
    EMVCO_STATUS_FEATURE_NOT_SUPPORTED = 6,

    /**
     * The system is busy with the previous operation.
     */
    EMVCO_STATUS_BUSY = 7,

    /**
     * Write operation failed
     */
    EMVCO_STATUS_WRITE_FAILED = 8,

    /**
     * Indicates NFCEE Discovery not completed.
     * Try start the EMVCo mode again, if CT is supported and TDA present in the hardware.
     */
    EMVCO_STATUS_TDA_INIT_NOT_COMPLETED = 9,
    /**
     * Indicates NFCEE Discovery failed.
     * Try start the EMVCo mode again, if CT is supported and TDA present in the hardware.
     */
    EMVCO_STATUS_TDA_INIT_FAILED = 10,
    /**
     * Indicates NCI Error response for NCI command.
     * COntroller is not in proper state to accept the NCI command.
     * Try start the EMVCo mode again, if CT is supported and TDA present in the hardware.
     */
    EMVCO_STATUS_NCI_RESPONSE_ERR = 11,
    /**
     * Indicates mode set mode set NCI command failed.
     * Try call openTDA api with proper TDA ID and standby false.
     */
    EMVCO_STATUS_NFCEE_MODE_SET_ENABLE_FAILED = 12,
    /**
     * Indicates core connection create NCI command failed.
     * Try call openTDA api with proper TDA ID.
     */
    EMVCO_STATUS_CORE_CONN_CREATE_FAILED = 13,
    /**
     * Indicates transceive NCI data command failed.
     * Try call closeAPI and then openTDA api with proper TDA ID to recover and send the transceive data.
     */
    EMVCO_STATUS_TRANSCEIVE_FAILED = 14,
    /**
     * Indicates core connection close NCI command failed.
     * Try call closeTDA api with proper TDA ID.
     */
    EMVCO_STATUS_CORE_CONN_CLOSE_FAILED = 15,
    /**
     * Indicates mode set disable NCI command failed.
     * Try call closeTDA api with proper TDA ID.
     */
    EMVCO_STATUS_NFCEE_MODE_SET_DISABLE_FAILED = 16,

    /**
     * Indicates failure status.
     */
    EMVCO_STATUS_FAILED = 255,
}

/** @}*/