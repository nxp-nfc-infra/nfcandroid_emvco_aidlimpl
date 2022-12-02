/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
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

#ifndef _EMVCO_CONFIG_H_
#define _EMVCO_CONFIG_H_
/** \addtogroup EMVCO_STACK_UTILITY_API_INTERFACE
 *  @brief interface to read configuration values on stored on config file. Also
 * provides Interface for getting the chip type, linked list and Semphore
 * implementation.
 *  @{
 */

/**
 *
 *
 * @brief       API function for getting a string value of a setting
 *
 * @param[in]       name name of the config param to read.
 * @param[in]       p_value pointer to input buffer.
 * @param[in]       len input buffer length.
 *
 * @return      True if found, otherwise False.
 *
 */
int get_str_value(const char *name, char *p_value, unsigned long len);

/***
 *
 *
 * @brief  API function for getting a numerical value of a setting
 *
 * @param[in]       name name of the config param to read.
 * @param[in]       p_value pointer to input buffer.
 * @param[in]       len input buffer length.
 *
 * @return      True if found, otherwise False.
 *
 */
int get_num_value(const char *name, void *p_value, unsigned long len);

/**
 *
 *
 * @brief Read byte array value from the config file.
 *
 * @param[in]        name - name of the config param to read.
 * @param[in]        pValue  - pointer to input buffer.
 * @param[in]        bufflen - input buffer length.
 * @param[in]        len - out parameter to return the number of bytes read from
 *                   config file, return -1 in case bufflen is not enough.
 *
 * @return     TRUE[1] if config param name is found in the config file, else
 *              FALSE[0]
 *
 */
int get_byte_array_value(const char *name, char *pValue, long bufflen,
                         long *len);

/**
 *
 * Function:    resetConfig
 *
 * @brief reset settings array
 * @param[in] void
 * @return    void
 */
void reset_config(void);
int is_config_modified();
int update_config_time_stamp();

#define NAME_CFG_NXP_NFC_DEV_NODE "CFG_NXP_NFC_DEV_NODE"
#define NAME_LOG_EXTNS_LOGLEVEL "LOG_EXTNS_LOGLEVEL"
#define NAME_LOG_NCIHAL_LOGLEVEL "LOG_NCIHAL_LOGLEVEL"
#define NAME_LOG_NCIX_LOGLEVEL "LOG_NCIX_LOGLEVEL"
#define NAME_LOG_NCIR_LOGLEVEL "LOG_NCIR_LOGLEVEL"
#define NAME_LOG_FWDNLD_LOGLEVEL "LOG_FWDNLD_LOGLEVEL"
#define NAME_LOG_TML_LOGLEVEL "LOG_TML_LOGLEVEL"

#define NAME_MIFARE_READER_ENABLE "MIFARE_READER_ENABLE"
#define NAME_LEGACY_MIFARE_READER "LEGACY_MIFARE_READER"
#define NAME_FW_STORAGE "FW_STORAGE"
#define NAME_FW_TYPE "NXP_FW_TYPE"
#define NAME_FW_PROTECION_OVERRIDE "NXP_FW_PROTECION_OVERRIDE"
#define NAME_SYS_CLK_SRC_SEL "NXP_SYS_CLK_SRC_SEL"
#define NAME_SYS_CLK_FREQ_SEL "NXP_SYS_CLK_FREQ_SEL"
#define NAME_SYS_CLOCK_TO_CFG "NXP_SYS_CLOCK_TO_CFG"
#define NAME_ACT_PROP_EXTN "NXP_ACT_PROP_EXTN"
#define NAME_EXT_TVDD_CFG "NXP_EXT_TVDD_CFG"
#define NAME_EXT_TVDD_CFG_1 "NXP_EXT_TVDD_CFG_1"
#define NAME_EXT_TVDD_CFG_2 "NXP_EXT_TVDD_CFG_2"
#define NAME_EXT_TVDD_CFG_3 "NXP_EXT_TVDD_CFG_3"
#define NAME_RF_CONF_BLK_1 "NXP_RF_CONF_BLK_1"
#define NAME_RF_CONF_BLK_2 "NXP_RF_CONF_BLK_2"
#define NAME_RF_CONF_BLK_3 "NXP_RF_CONF_BLK_3"
#define NAME_RF_CONF_BLK_4 "NXP_RF_CONF_BLK_4"
#define NAME_RF_CONF_BLK_5 "NXP_RF_CONF_BLK_5"
#define NAME_RF_CONF_BLK_6 "NXP_RF_CONF_BLK_6"
#define NAME_CORE_CONF_EXTN "NXP_CORE_CONF_EXTN"
#define NAME_CORE_CONF "NXP_CORE_CONF"
#define NAME_NFC_PROFILE_EXTN "NXP_NFC_PROFILE_EXTN"
#define NAME_CHINA_TIANJIN_RF_ENABLED "NXP_CHINA_TIANJIN_RF_ENABLED"
#define NAME_SWP_SWITCH_TIMEOUT "NXP_SWP_SWITCH_TIMEOUT"
#define NAME_SWP_FULL_PWR_ON "NXP_SWP_FULL_PWR_ON"
#define NAME_CORE_RF_FIELD "NXP_CORE_RF_FIELD"
#define NAME_I2C_FRAGMENTATION_ENABLED "NXP_I2C_FRAGMENTATION_ENABLED"
#define NAME_ISO_DEP_MAX_TRANSCEIVE "ISO_DEP_MAX_TRANSCEIVE"
#define NAME_AID_MATCHING_PLATFORM "AID_MATCHING_PLATFORM"
#define NAME_EMVCO_DEBUG_ENABLED "EMVCO_DEBUG_ENABLED"
#define NAME_RF_STATUS_UPDATE_ENABLE "RF_STATUS_UPDATE_ENABLE"
#define NAME_DEFAULT_ROUTE "DEFAULT_ROUTE"
#define NAME_DEFAULT_OFFHOST_ROUTE "DEFAULT_OFFHOST_ROUTE"
#define NAME_DEFAULT_NFCF_ROUTE "DEFAULT_NFCF_ROUTE"
#define NAME_DEFAULT_SYS_CODE_ROUTE "DEFAULT_SYS_CODE_ROUTE"
#define NAME_DEFAULT_SYS_CODE_PWR_STATE "DEFAULT_SYS_CODE_PWR_STATE"
#define NAME_DEVICE_HOST_WHITE_LIST "DEVICE_HOST_WHITE_LIST"
#define NAME_OFF_HOST_ESE_PIPE_ID "OFF_HOST_ESE_PIPE_ID"
#define NAME_OFF_HOST_SIM_PIPE_ID "OFF_HOST_SIM_PIPE_ID"
#define NAME_NFA_PROPRIETARY_CFG "NFA_PROPRIETARY_CFG"
#define NAME_PRESENCE_CHECK_ALGORITHM "PRESENCE_CHECK_ALGORITHM"
#define NAME_OFFHOST_ROUTE_ESE "OFFHOST_ROUTE_ESE"
#define NAME_OFFHOST_ROUTE_UICC "OFFHOST_ROUTE_UICC"
#define NAME_DEFAULT_ISODEP_ROUTE "DEFAULT_ISODEP_ROUTE"

/* default configuration */
#define default_storage_location "/data/vendor/nfc"

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#define TYPE_VAL 0
#define TYPE_DATA 1
#define TYPE_STR 2

typedef struct {
  unsigned char key;
  unsigned char type;
  const void *val;
} NxpParam_t;

#define CONFIG_VAL (void *)

typedef enum {
  CFG_LOG_EXTNS_LOGLEVEL,
  CFG_LOG_NCIHAL_LOGLEVEL,
  CFG_LOG_NCIX_LOGLEVEL,
  CFG_LOG_NCIR_LOGLEVEL,
  CFG_LOG_FWDNLD_LOGLEVEL,
  CFG_LOG_TML_LOGLEVEL,
  CFG_NXP_NFC_DEV_NODE,

  CFG_MIFARE_READER_ENABLE,
  CFG_NXP_AGC_DEBUG_ENABLE,
  CFG_NXP_FW_NAME,

  CFG_NXP_FW_PROTECION_OVERRIDE,

  CFG_NXP_SYS_CLK_SRC_SEL,
  CFG_NXP_SYS_CLK_FREQ_SEL,
  CFG_NXP_SYS_CLOCK_TO_CFG,
  CFG_NXP_CLOCK_REQ_DELAY,

  CFG_NXP_ESE_POWER_DH_CONTROL,

  CFG_NXP_EXT_TVDD_CFG,
  CFG_NXP_EXT_TVDD_CFG_1,
  CFG_NXP_EXT_TVDD_CFG_2,
  CFG_NXP_EXT_TVDD_CFG_3,
  CFG_NXP_ACT_PROP_EXTN,

  CFG_NXP_RF_CONF_BLK_1,
  CFG_NXP_RF_CONF_BLK_2,
  CFG_NXP_RF_CONF_BLK_3,
  CFG_NXP_RF_CONF_BLK_4,
  CFG_NXP_RF_CONF_BLK_5,
  CFG_NXP_RF_CONF_BLK_6,

  CFG_NXP_CORE_CONF_EXTN,
  CFG_NXP_CORE_CONF,
  CFG_NXP_CORE_MFCKEY_SETTING,
  CFG_NXP_CORE_STANDBY,
  CFG_NXP_CORE_RF_FIELD,

  CFG_NXP_NFC_PROFILE_EXTN,
  CFG_NXP_NFA_PROPRIETARY_CFG,
  CFG_NXP_NFC_CHIP,

  CFG_NXP_SWP_FULL_PWR_ON,
  CFG_NXP_SWP_RD_START_TIMEOUT,
  CFG_NXP_SWP_RD_TAG_OP_TIMEOUT,

  CFG_NXP_I2C_FRAGMENTATION_ENABLED,

  CFG_NXP_DEFAULT_SE,
  CFG_NXP_DEFAULT_NFCEE_TIMEOUT,
  CFG_NXP_DEFAULT_NFCEE_DISC_TIMEOUT,

  CFG_NXP_UICC_FORWARD_FUNCTIONALITY_ENABLE,

  CFG_NXP_CE_ROUTE_STRICT_DISABLE,

  CFG_DEFAULT_AID_ROUTE,
  CFG_DEFAULT_DESFIRE_ROUTE,
  CFG_DEFAULT_MIFARE_CLT_ROUTE,
  CFG_DEFAULT_AID_PWR_STATE,
  CFG_DEFAULT_DESFIRE_PWR_STATE,
  CFG_DEFAULT_MIFARE_CLT_PWR_STATE,

  CFG_NXP_NFC_MERGE_RF_PARAMS,
  CFG_AID_MATCHING_PLATFORM,
  CFG_NXP_T4T_NDEFEE_ENABLE,
  CFG_NXP_SWP_SWITCH_TIMEOUT,
  CFG_NXP_CHINA_TIANJIN_RF_ENABLED,
  CFG_NXP_CN_TRANSIT_CMA_BYPASSMODE_ENABLE,
  CFG_NXP_SET_CONFIG_ALWAYS,
  CFG_NXP_CHECK_DEFAULT_PROTO_SE_ID,
  CFG_NXP_MF_CLT_WA,
#if ((NFC_NXP_CHIP_TYPE == SN100) || (NFC_NXP_CHIP_TYPE == SN220))
  CFG_NXP_SPC_COMPENSATION_COMMAND,
#endif
} NxpNfccConfig;

//////////////////////////////////////////////////////////////////////////////#
/*# System clock source selection configuration as per pn553 user manual
 * section 14.1*/
/* PLL clock src is 0 */
/* XTAL clock src is 1 */
#define CLK_SRC_NOXTAL 2
//#define CLK_SRC_XTAL       1
//#define CLK_SRC_PLL        0

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif
/** @}*/
#endif //_EMVCO_CONFIG_H_