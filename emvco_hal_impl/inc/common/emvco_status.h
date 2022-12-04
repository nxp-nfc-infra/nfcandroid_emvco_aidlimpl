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

/*
 * EMVCO Status Values - Function Return Codes
 */

#ifndef _EMVCO_STATUS_H_
#define _EMVCO_STATUS_H_

#include <emvco_types.h>

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
 *  Status Codes
 *
 *  Generic Status codes for the EMVCO components. Combined with the Component
 * ID they build the value (status) returned by each function. Example:
 *      grp_comp_id "Component ID" -  e.g. 0x10, plus
 *      status code as listed in this file - e.g. 0x03
 *      result in a status value of 0x0003.
 */

/*
 * The function indicates successful completion
 */
#define EMVCO_STATUS_SUCCESS (0x0000)

/*
 *  The function indicates successful completion
 */
#define EMVCO_STATUS_OK (EMVCO_STATUS_SUCCESS)

/*
 * At least one parameter could not be properly interpreted
 */
#define EMVCO_STATUS_INVALID_PARAMETER (0x0001)

/*
 * The buffer provided by the caller is too small
 */
#define EMVCO_STATUS_BUFFER_TOO_SMALL (0x0003)

/*
 * Device specifier/handle value is invalid for the operation
 */
#define EMVCO_STATUS_INVALID_DEVICE (0x0006)

/*
 * The function executed successfully but could have returned
 * more information than space provided by the caller
 */
#define EMVCO_STATUS_MORE_INFORMATION (0x0008)

/*
 * No response from the remote device received: Time-out
 */
#define EMVCO_STATUS_RF_TIMEOUT (0x0009)

/*
 * RF Error during data transaction with the remote device
 */
#define EMVCO_STATUS_RF_ERROR (0x000A)

/*
 * Not enough resources Memory, Timer etc(e.g. allocation failed.)
 */
#define EMVCO_STATUS_INSUFFICIENT_RESOURCES (0x000C)

/*
 * A non-blocking function returns this immediately to indicate
 * that an internal operation is in progress
 */
#define EMVCO_STATUS_PENDING (0x000D)

/*
 * A board communication error occurred
 * (e.g. Configuration went wrong)
 */
#define EMVCO_STATUS_BOARD_COMMUNICATION_ERROR (0x000F)

/*
 * Invalid State of the particular state machine
 */
#define EMVCO_STATUS_INVALID_STATE (0x0011)

/*
 * This Layer is Not initialized, hence initialization required.
 */
#define EMVCO_STATUS_NOT_INITIALISED (0x0031)

/*
 * The Layer is already initialized, hence initialization repeated.
 */
#define EMVCO_STATUS_ALREADY_INITIALISED (0x0032)

/*
 * Feature not supported
 */
#define EMVCO_STATUS_FEATURE_NOT_SUPPORTED (0x0033)

/*  The Unregistration command has failed because the user wants to unregister
 * on
 * an element for which he was not registered
 */
#define EMVCO_STATUS_NOT_REGISTERED (0x0034)

/* The Registration command has failed because the user wants to register on
 * an element for which he is already registered
 */
#define EMVCO_STATUS_ALREADY_REGISTERED (0x0035)

/*  Single Tag with Multiple
    Protocol support detected */
#define EMVCO_STATUS_MULTIPLE_PROTOCOLS (0x0036)

/*
 * Feature not supported
 */
#define EMVCO_STATUS_MULTIPLE_TAGS (0x0037)

/*
 * A DESELECT event has occurred
 */
#define EMVCO_STATUS_DESELECTED (0x0038)

/*
 * A RELEASE event has occurred
 */
#define EMVCO_STATUS_RELEASED (0x0039)

/*
 * The operation is currently not possible or not allowed
 */
#define EMVCO_STATUS_NOT_ALLOWED (0x003A)

/*
 *  The system is busy with the previous operation.
 */
#define EMVCO_STATUS_BUSY (0x006F)

/* NDEF Mapping error codes */

/* The remote device (type) is not valid for this request. */
#define EMVCO_STATUS_INVALID_REMOTE_DEVICE (0x001D)

/* Read operation failed */
#define EMVCO_STATUS_READ_FAILED (0x0014)

/*
 * Write operation failed
 */
#define EMVCO_STATUS_WRITE_FAILED (0x0015)

/* Non Ndef Compliant */
#define EMVCO_STATUS_NO_NDEF_SUPPORT (0x0016)

/* Could not proceed further with the write operation: reached card EOF*/
#define EMVCO_STATUS_EOF_NDEF_CONTAINER_REACHED (0x001A)

/* Incorrect number of bytes received from the card*/
#define EMVCO_STATUS_INVALID_RECEIVE_LENGTH (0x001B)

/* The data format/composition is not understood/correct. */
#define EMVCO_STATUS_INVALID_FORMAT (0x001C)

/* There is not sufficient storage available. */
#define EMVCO_STATUS_INSUFFICIENT_STORAGE (0x001F)

/* The Ndef Format procedure has failed. */
#define EMVCO_STATUS_FORMAT_ERROR (0x0023)

/* The NCI Cedit error */
#define EMVCO_STATUS_CREDIT_TIMEOUT (0x0024)

/*
 * Response Time out for the control message(NFCC not responded)
 */
#define EMVCO_STATUS_RESPONSE_TIMEOUT (0x0025)

/*
 * Device is already connected
 */
#define EMVCO_STATUS_ALREADY_CONNECTED (0x0026)

/*
 * Device is already connected
 */
#define EMVCO_STATUS_ANOTHER_DEVICE_CONNECTED (0x0027)

/*
 * Single Target Detected and Activated
 */
#define EMVCO_STATUS_SINGLE_TAG_ACTIVATED (0x0028)

/*
 * Single Target Detected
 */
#define EMVCO_STATUS_SINGLE_TAG_DISCOVERED (0x0029)

/*
 * Secure element Detected and Activated
 */
#define EMVCO_STATUS_SECURE_ELEMENT_ACTIVATED (0x0028)

/*
 * Unknown error Status Codes
 */
#define EMVCO_STATUS_UNKNOWN_ERROR (0x00FE)

/*
 * Status code for failure
 */
#define EMVCO_STATUS_FAILED (0x00FF)

/*
 * The function/command has been aborted
 */
#define EMVCO_STATUS_CMD_ABORTED (0x0002)

/*
 * No target found after poll
 */
#define EMVCO_STATUS_NO_TARGET_FOUND (0x000A)

/* Attempt to disconnect a not connected remote device. */
#define EMVCO_STATUS_NO_DEVICE_CONNECTED (0x000B)

/* External RF field detected. */
#define EMVCO_STATUS_EXTERNAL_RF_DETECTED (0x000E)

/* Message is not allowed by the state machine
 * (e.g. configuration went wrong)
 */
#define EMVCO_STATUS_MSG_NOT_ALLOWED_BY_FSM (0x0010)

/*
 * No access has been granted
 */
#define EMVCO_STATUS_ACCESS_DENIED (0x001E)

/* No registry node matches the specified input data. */
#define EMVCO_STATUS_NODE_NOT_FOUND (0x0017)

/* The current module is busy ; one might retry later */
#define EMVCO_STATUS_SMX_BAD_STATE (0x00F0)

/* The Abort mechanism has failed for unexpected reason: user can try again*/
#define EMVCO_STATUS_ABORT_FAILED (0x00F2)

/* The Registration command has failed because the user wants to register as
 * target
 * on a operating mode not supported
 */
#define EMVCO_STATUS_REG_OPMODE_NOT_SUPPORTED (0x00F5)

/*
 * Shutdown in progress, cannot handle the request at this time.
 */
#define EMVCO_STATUS_SHUTDOWN (0x0091)

/*
 * Target is no more in RF field
 */
#define EMVCO_STATUS_TARGET_LOST (0x0092)

/*
 * Request is rejected
 */
#define EMVCO_STATUS_REJECTED (0x0093)

/*
 * Target is not connected
 */
#define EMVCO_STATUS_TARGET_NOT_CONNECTED (0x0094)

/*
 * Invalid handle for the operation
 */
#define EMVCO_STATUS_INVALID_HANDLE (0x0095)

/*
 * Process aborted
 */
#define EMVCO_STATUS_ABORTED (0x0096)

/*
 * Requested command is not supported
 */
#define EMVCO_STATUS_COMMAND_NOT_SUPPORTED (0x0097)

/*
 * Tag is not NDEF compilant
 */
#define EMVCO_STATUS_NON_NDEF_COMPLIANT (0x0098)

/*
 * Not enough memory available to complete the requested operation
 */
#define EMVCO_STATUS_NOT_ENOUGH_MEMORY (0x001F)

/*
 * Indicates incoming connection
 */
#define EMVCO_STATUS_INCOMING_CONNECTION (0x0045)

/*
 * Indicates Connection was successful
 */
#define EMVCO_STATUS_CONNECTION_SUCCESS (0x0046)

/*
 * Indicates Connection failed
 */
#define EMVCO_STATUS_CONNECTION_FAILED (0x0047)

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

typedef enum { EMVCO_MODE_ON = 1, EMVCO_MODE_OFF } emvco_mode_status_t;

emvco_mode_status_t emvco_status;

#endif /* _EMVCO_STATUS_H_ */
