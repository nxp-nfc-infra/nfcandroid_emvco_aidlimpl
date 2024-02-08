/******************************************************************************
 *
 *  Copyright 2023 NXP
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

/*
 * EMVCO Status Values - Function Return Codes
 */

#ifndef _EMVCO_STATUS_H_
#define _EMVCO_STATUS_H_

#include <emvco_common.h>
/* Internally required by EMVCOSTVAL. */
#define EMVCOSTSHL8 (8U)
/* Required by EMVCOSTVAL. */
#define EMVCOSTBLOWER ((EMVCO_STATUS)(0x00FFU))

/*
 *  EMVCO Status Composition Macro
 *
 *  This is the macro which must be used to compose status values.
 *
 *  emvco_comp_id Component ID, as defined in emvco_comp_id.h .
 *  emvco_status Status values, as defined in emvco_status.h .
 *
 *  The macro is not required for the EMVCO_STATUS_SUCCESS value.
 *  This is the only return value to be used directly.
 *  For all other values it shall be used in assignment and conditional
 * statements, e.g.:
 *     EMVCO_STATUS status = EMVCOSTVAL(emvco_comp_id, emvco_status); ...
 *     if (status == EMVCOSTVAL(emvco_comp_id, emvco_status)) ...
 */
#define EMVCOSTVAL(emvco_comp_id, emvco_status)                                \
  (((emvco_status) == (EMVCO_STATUS_SUCCESS))                                  \
       ? (EMVCO_STATUS_SUCCESS)                                                \
       : ((((EMVCO_STATUS)(emvco_status)) & (EMVCOSTBLOWER)) |                 \
          (((uint16_t)(emvco_comp_id)) << (EMVCOSTSHL8))))

/*
 * PHEMVCO_STATUS
 * Get grp_retval from Status Code
 */
#define PHEMVCO_STATUS(emvco_status) ((emvco_status)&0x00FFU)
#define PHNFCCID(emvco_status) (((emvco_status)&0xFF00U) >> 8)

#define EMVCO_I2C_FRAGMENT_SIZE 512

/*
 * Indicates the NFC state
 */
typedef enum {
  STATE_OFF = 1,
  STATE_TURNING_ON,
  STATE_ON,
  STATE_TURNING_OFF
} nfc_status_t;

nfc_status_t nfc_status;

/*
 * The function indicates successful completion
 */
#define EMVCO_STATUS_SUCCESS (0x00)

/*
 *  The function indicates successful completion
 */
#define EMVCO_STATUS_OK (EMVCO_STATUS_SUCCESS)

/*
 * At least one parameter could not be properly interpreted
 */
#define EMVCO_STATUS_INVALID_PARAMETER (0x01)

/*
 * Device specifier/handle value is invalid for the operation
 */
#define EMVCO_STATUS_INVALID_DEVICE (0x02)

/*
 * A non-blocking function returns this immediately to indicate
 * that an internal operation is in progress
 */
#define EMVCO_STATUS_PENDING (0x03)

/*
 * This Layer is Not initialized, hence initialization required.
 */
#define EMVCO_STATUS_NOT_INITIALISED (0x04)

/*
 * The Layer is already initialized, hence initialization repeated.
 */
#define EMVCO_STATUS_ALREADY_INITIALISED (0x05)

/*
 * Feature not supported
 */
#define EMVCO_STATUS_FEATURE_NOT_SUPPORTED (0x06)

/*
 *  The system is busy with the previous operation.
 */
#define EMVCO_STATUS_BUSY (0x07)

/*
 * Write operation failed
 */
#define EMVCO_STATUS_WRITE_FAILED (0x08)

/**
 * Indicates NFCEE Discovery not completed.
 * Try start the EMVCo mode again, if CT is supported and TDA present in the
 * hardware.
 */
#define EMVCO_STATUS_TDA_INIT_NOT_COMPLETED (0x09)

/**
 * Indicates NFCEE Discovery failed.
 * Try start the EMVCo mode again, if CT is supported and TDA present in the
 * hardware.
 */
#define EMVCO_STATUS_TDA_INIT_FAILED (0x0A)

/**
 * Indicates NCI Error response for NCI command.
 * COntroller is not in proper state to accept the NCI command.
 * Try start the EMVCo mode again, if CT is supported and TDA present in the
 * hardware.
 */
#define EMVCO_STATUS_NCI_RESPONSE_ERR (0x0B)

/**
 * Indicates mode set mode set NCI command failed.
 * Try to call openTDA api with proper TDA ID and standby false.
 */
#define EMVCO_STATUS_NFCEE_MODE_SET_ENABLE_FAILED (0x0C)

/**
 * Indicates core connection create NCI command failed.
 * Try to call openTDA api with proper TDA ID.
 */
#define EMVCO_STATUS_CORE_CONN_CREATE_FAILED (0x0D)

/**
 * Indicates transceive NCI data command failed.
 * Try to call closeAPI and then openTDA api with proper TDA ID to recover and
 * send the transceive data.
 */
#define EMVCO_STATUS_TRANSCEIVE_FAILED (0x0E)

/**
 * Indicates core connection close NCI command failed.
 * Ensure TDA was opened and ry to call closeTDA api with proper TDA ID.
 */
#define EMVCO_STATUS_CORE_CONN_CLOSE_FAILED (0x0F)

/**
 * Indicates mode set disable NCI command failed.
 * Ensure TDA was opened and ry to call closeTDA api with proper TDA ID.
 */
#define EMVCO_STATUS_NFCEE_MODE_SET_DISABLE_FAILED (0x10)

/*
 * Status code for failure
 */
#define EMVCO_STATUS_FAILED (0xFF)

#endif /* _EMVCO_STATUS_H_ */
