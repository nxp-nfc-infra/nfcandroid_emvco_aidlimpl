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

#ifndef _EMVCO_CT_H_
#define _EMVCO_CT_H_
/** \addtogroup EMVCO_STACK_CONTACT_CARD_API_INTERFACE
 *  @brief  interface to interact with CT interfaces.
 *  @{
 */

#include <emvco_common.h>

#ifdef __cplusplus
extern "C" {
#endif

tda_type_t g_tda_type;

/**
 * @brief discovers the smart card connected to TDA and returns the smart card
 * control.
 *
 * @param[in] void
 * @param[out] tda_control provides the deatils of the smartcards present over
 * TDA
 *
 * @return EMVCO_STATUS returns EMVCO_STATUS_OK, if feature supported
 * and returns EMVCO_STATUS_FEATURE_NOT_SUPPORTED, if feature is
 * not supported
 *
 */
EMVCO_STATUS discover_tda_slots(tda_control_t *tda_control);

/**
 * @brief opens the contactcard.
 *
 * @param[in] tda_id id of the contact card to be opened
 * @param[out] returns the conn_id id of the contact card
 *
 * @return EMVCO_STATUS returns EMVCO_STATUS_OK, if feature supported
 * and returns EMVCO_STATUS_FEATURE_NOT_SUPPORTED, if feature is
 * not supported
 *
 */
EMVCO_STATUS open_tda_slot(int8_t tda_id, int8_t *conn_id);

/**
 * @brief closes the contactcard.
 *
 * @param[in] tda_id id of the contact card to be closed
 *
 * @return EMVCO_STATUS returns EMVCO_STATUS_OK, if feature supported
 * and returns EMVCO_STATUS_FEATURE_NOT_SUPPORTED, if feature is
 * not supported
 *
 */
EMVCO_STATUS close_tda_slot(int8_t tda_id);

/**
 *
 * @brief           This function write the data to NFCC through physical
 *                  interface (e.g. I2C) using the PN7220 driver interface.
 *
 * @param[in]       cmd_apdu: Command to TDA
 * @param[out]      rsp_apdu: Command to TDA
 *
 * @return EMVCO_STATUS returns EMVCO_STATUS_OK, if feature supported
 * and returns EMVCO_STATUS_FEATURE_NOT_SUPPORTED, if feature is
 * not supported
 *
 */
EMVCO_STATUS transceive_tda_slot(tda_data *cmd_apdu, tda_data *rsp_apdu);

#ifdef __cplusplus
}
#endif /*  C++ Compilation guard */
/** @}*/
#endif /* _EMVCO_CT_H_ */
