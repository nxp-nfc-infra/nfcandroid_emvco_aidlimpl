/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
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

#include <stdio.h>

#include "phNxpConfig.h"
#include <phNfcCommon.h>
#include <phNxpLog.h>
#include <phOsalNfc_Memory.h>
#include <string.h>
#include <sys/stat.h>

#define config_name "libnfc-nxp.conf"
#if (NXP_EXTNS == TRUE)
#define extra_config_base "libnfc-"
#else
#define extra_config_base "libnfc-nxp-"
#endif
#define extra_config_ext ".conf"
#define IsStringValue 0x80000000

/*
 * Pn66T configuration data
 *
 */

//------------
// NOTE: first byte of cfgData values is the number of data bytes
//------------

//////////////////////////////////////////////////////////////////////////////#
//# NXP TVDD configurations settings
//#    Allow NFCC to configure the external TVDD
//#    Two configurations (0x01 and 0x02) are supported
//#    Only one shall be selected (hardware dependency)
//#       Config 1: VUP connected to VBAT
//#       Config 2: VUP connected to external 5V
#if ((NFC_NXP_CHIP_TYPE == SN100) || (NFC_NXP_CHIP_TYPE == SN220))
const uint8_t cfgData_Nxp_Ext_Tvdd_Cfg_1[] = {0x00};
const uint8_t cfgData_Nxp_Ext_Tvdd_Cfg_2[] = {0x00};
#else
#ifdef PN80T_CD_ENABLE
const uint8_t cfgData_Nxp_Ext_Tvdd_Cfg_1[] = {
    0x12, 0x20, 0x02, 0x0F, 0x01, 0xA0, 0x0E, 0x0B, 0x11, 0x01,
    0x01, 0x31, 0x00, 0x00, 0x1E, 0x11, 0x00, 0x10, 0x0C};
#else
const uint8_t cfgData_Nxp_Ext_Tvdd_Cfg_1[] = {
    0x12, 0x20, 0x02, 0x0F, 0x01, 0xA0, 0x0E, 0x0B, 0x31, 0x01,
    0x01, 0x31, 0x00, 0x00, 0x00, 0x01, 0x00, 0xD0, 0x0C};
#endif
const uint8_t cfgData_Nxp_Ext_Tvdd_Cfg_2[] = {
    0x12, 0x20, 0x02, 0x0F, 0x01, 0xA0, 0x0E, 0x0B, 0x11, 0x01,
    0xC2, 0xB2, 0x00, 0xB2, 0x1E, 0x1F, 0x00, 0xD0, 0x0C};
#endif
//////////////////////////////////////////////////////////////////////////////#
//# NXP proprietary settings
const uint8_t cfgData_Nxp_Act_Prop_Extn[] = {0x03, 0x2F, 0x02, 0x00};

//////////////////////////////////////////////////////////////////////////////#
//# NFC forum profile settings
const uint8_t cfgData_Nfa_Proprietary_Cfg[] = {0x05, 0xFF, 0xFF, 0x06, 0x81,
                                               0x80, 0x70, 0xFF, 0xFF};
const uint8_t cfgData_Nxp_Nfc_Profile_Extn[] = {0x08, 0x20, 0x02, 0x05, 0x01,
                                                0xA0, 0x44, 0x01, 0x00};

const uint8_t cfgData_Nxp_Rf_Conf_Blk_1[] = {0x00};

const uint8_t cfgData_Nxp_Rf_Conf_Blk_2[] = {0x00};

const uint8_t cfgData_Nxp_Rf_Conf_Blk_3[] = {0x00};

const uint8_t cfgData_Nxp_Rf_Conf_Blk_4[] = {0x00};

const uint8_t cfgData_Nxp_Rf_Conf_Blk_5[] = {0x00};

const uint8_t cfgData_Nxp_Rf_Conf_Blk_6[] = {0x00};

//////////////////////////////////////////////////////////////////////////////#
#if (NFC_NXP_CHIP_TYPE == SN220)
/*
 * 0xA007 -> Set to 1: the NFCC stay in NFC_ACT when VDDIO=0V and VEN is don’t
 * care in this situation, This parameter shall be activated only once on
 * customer production line 0xA015 -> Default power state after hardware reset :
 * Autonomous mode 0xA10F -> Default power state after hardware reset : ULPDET
 * mode (above two configs can be overridden by SET_POWER_MODE API) 0xA109 ->
 * GPIO_ULPDET_CFG If set to 0x01, NFC_GPIO2_AO is high when NFCC is waken up
 * from ULPDET mode and low when it enters again in ULPDET mode. If set to 0x00,
 * NFC_GPIO2_AO is not used during ULPDET mode
 */
const uint8_t cfgData_Nxp_Core_Conf_Extn[] = {
    0x1D, 0x20, 0x02, 0x1A, 0x06, 0xA0, 0xED, 0x01, 0x01, // Enable eSE
#if (NXP_NFCC_DYNAMIC_DUAL_UICC == TRUE)
    0xA0, 0xEC, 0x01, 0x01, // Enable UICC1
    0xA0, 0xD4, 0x01, 0x01, // Enable UICC2
#else
    0xA0, 0xEC, 0x01, 0x00, // Disable UICC1
    0xA0, 0xD4, 0x01, 0x00, // Disable UICC2
#endif
    0xA0, 0x07, 0x01, 0x01, 0xA0, 0x15, 0x01, 0x02, 0xA1,
    0x0F, 0x02, 0x01, 0x01};
#elif (NFC_NXP_CHIP_TYPE == SN100)
/*
 * 0xA015 -> Default power state after hardware reset : Autonomous mode
 * 0xA10F -> Default power state after hardware reset : ULPDET mode
 *          (above two configs can be overridden by SET_POWER_MODE API)
 * 0xA018 -> VEN connected to GPIO3
 * 0xA109 -> if set to '0' NFC_GPIO2_AO is not used during ULPDET mode
 */
const uint8_t cfgData_Nxp_Core_Conf_Extn[] = {
    0x20, 0x20, 0x02, 0x1D, 0x07, 0xA0, 0xED, 0x01, 0x01, // Enable eSE
#if (NXP_NFCC_DYNAMIC_DUAL_UICC == TRUE)
    0xA0, 0xEC, 0x01, 0x01,                               // Enable UICC1
    0xA0, 0xD4, 0x01, 0x01,                               // Enable UICC2
#else
    0xA0, 0xEC, 0x01, 0x00, // Disable UICC1
    0xA0, 0xD4, 0x01, 0x00, // Disable UICC2
#endif
    0xA0, 0x15, 0x01, 0x02, 0xA0, 0x18, 0x01, 0x01, 0xA1,
    0x0F, 0x01, 0x01, 0xA1, 0x09, 0x01, 0x00};
#elif (NFC_NXP_CHIP_TYPE == PN557)
/* Removing READER_JEWEL_RID_CFG [A0 5E]*/
/* Removing READER_TAG_DETECTOR_CFG [A0 40] as not valid for wearable*/
// Core configuration extensions
//  It includes
//  Wired mode settings A0ED, A0EE
//  Tag Detector A040, A041, A043    *** Removed
//  Low Power mode A007
//  Clock settings A002, A003
//  PbF settings A008
//  Clock timeout settings A004
//  How eSE connected to PN553 A012
//  UICC2 bit rate A0D1
//  SWP1A interface A0D4
//  DWP intf behavior config A037
//  DWP Speed A0D5
//  eSE (SVDD) PWR REQ settings A0F2
/*
** Added A0B2(End_of_operation Delay) with value set to 0x19(25x100us = 2.5ms).
** Workaround enabled only for PN81T. After receiving the end of operation
*event,
** any CLT is blocked during this time.
*/
const uint8_t cfgData_Nxp_Core_Conf_Extn[] = {
    0x24, 0x20, 0x02, 0x21, 0x08, 0xA0, 0xED, 0x01, 0x01, 0xA0,
    0x37, 0x01, 0x35, 0xA0, 0xF2, 0x01, 0x00, 0xA0, 0xD5, 0x01,
    0x0A, 0xA0, 0xD8, 0x01, 0x02, 0xA0, 0xEC, 0x01, 0x00, 0xA0,
    0x12, 0x01, 0x02, 0xA0, 0xB2, 0x01, 0x19};
#else
const uint8_t cfgData_Nxp_Core_Conf_Extn[] = {
    0x20, 0x20, 0x02, 0x1D, 0x07, 0xA0, 0xED, 0x01, 0x01, 0xA0, 0x37,
    0x01, 0x35, 0xA0, 0xF2, 0x01, 0x00, 0xA0, 0xD5, 0x01, 0x0A, 0xA0,
    0xD8, 0x01, 0x02, 0xA0, 0xEC, 0x01, 0x00, 0xA0, 0x12, 0x01, 0x02};
#endif

/* Removed support for NFC-DEP 0x50 - LF_PROTOCOL_TYPE for wearable */
#if (NFC_NXP_CHIP_TYPE == PN557 || NFC_NXP_CHIP_TYPE == SN100 ||               \
     NFC_NXP_CHIP_TYPE == SN220)
const uint8_t cfgData_Nxp_Core_Conf[] = {
    0x34, 0x20, 0x02, 0x31, 0x0F, 0x28, 0x01, 0x00, 0x21, 0x01, 0x00,
    0x30, 0x01, 0x08, 0x31, 0x01, 0x03, 0x32, 0x01, 0x20, 0x38, 0x01,
    0x01, 0x33, 0x04, 0x01, 0x02, 0x03, 0x04, 0x54, 0x01, 0x06, 0x50,
    0x01, 0x00, 0x5B, 0x01, 0x00, 0x80, 0x01, 0x01, 0x81, 0x01, 0x01,
    0x82, 0x01, 0x0E, 0x18, 0x01, 0x01, 0x85, 0x01, 0x01 /*NFCC_CONFIG_CONTROL
                                                            set to 0x01; NFCC is
                                                            allowed to manage RF
                                                            configuration*/
};
#else
const uint8_t cfgData_Nxp_Core_Conf[] = {
    0x2E, 0x20, 0x02, 0x2B, 0x0D, 0x28, 0x01, 0x00, 0x21, 0x01, 0x00, 0x30,
    0x01, 0x08, 0x31, 0x01, 0x03, 0x32, 0x01, 0x20, 0x38, 0x01, 0x01, 0x33,
    0x04, 0x01, 0x02, 0x03, 0x04, 0x54, 0x01, 0x06,
    //        0x50, 0x01, 0x02, /* Removed */
    0x5B, 0x01, 0x00, 0x80, 0x01, 0x01, 0x81, 0x01, 0x01, 0x82, 0x01, 0x0E,
    0x18, 0x01, 0x01};
#endif /*#if (NFC_NXP_CHIP_TYPE == PN557)*/
//////////////////////////////////////////////////////////////////////////////#
//# Core configuration rf field filter settings to enable set to 01 to disable
// set # to 00 last bit
const uint8_t cfgData_Nxp_Core_Rf_Field[] = {0x08, 0x20, 0x02, 0x05, 0x01,
                                             0xA0, 0x62, 0x01, 0x00};

#if ((NFC_NXP_CHIP_TYPE == SN100) || (NFC_NXP_CHIP_TYPE == SN220))
const uint8_t cfgData_Nxp_Spc_Compensation_Command[] = {
    0x12, 0x2F, 0x3D, 0x0F, 0x30, 0x00, 0xE0, 0x2E, 0x32, 0x51,
    0x4B, 0x0A, 0x36, 0xA2, 0x1A, 0x02, 0x00, 0x05, 0x1F};
#endif
const NxpParam_t NXPConfig[] = {
    //////////////////////////////////////////////////////////////////////////////#
    {CFG_NXPLOG_EXTNS_LOGLEVEL, TYPE_VAL, CONFIG_VAL 0x03},
    {CFG_NXPLOG_NCIHAL_LOGLEVEL, TYPE_VAL, CONFIG_VAL 0x03},
    {CFG_NXPLOG_NCIX_LOGLEVEL, TYPE_VAL, CONFIG_VAL 0x03},
    {CFG_NXPLOG_NCIR_LOGLEVEL, TYPE_VAL, CONFIG_VAL 0x03},
    {CFG_NXPLOG_FWDNLD_LOGLEVEL, TYPE_VAL, CONFIG_VAL 0x03},
    {CFG_NXPLOG_TML_LOGLEVEL, TYPE_VAL, CONFIG_VAL 0x03},
    //////////////////////////////////////////////////////////////////////////////#
    //# Nfc Device Node name
    {CFG_NXP_NFC_DEV_NODE, TYPE_STR, "/dev/nxpnfc"},

    //////////////////////////////////////////////////////////////////////////////#
    //# Extension for Mifare reader enable
    {CFG_MIFARE_READER_ENABLE, TYPE_VAL, CONFIG_VAL 0x00},

    //////////////////////////////////////////////////////////////////////////////#
    //# Extension for Mifare reader enable
    {CFG_NXP_AGC_DEBUG_ENABLE, TYPE_VAL, CONFIG_VAL 0x00},

//////////////////////////////////////////////////////////////////////////////#

#ifdef CLK_SRC_NOCLOCK
    {CFG_NXP_SYS_CLK_SRC_SEL, TYPE_VAL, CONFIG_VAL 0x02},
#else
#ifdef PN80T_CD_ENABLE
    {CFG_NXP_SYS_CLK_SRC_SEL, TYPE_VAL, CONFIG_VAL 0x01},
#else
    {CFG_NXP_SYS_CLK_SRC_SEL, TYPE_VAL, CONFIG_VAL 0x00},
#endif
#endif
    //////////////////////////////////////////////////////////////////////////////#

    {CFG_NXP_SYS_CLK_FREQ_SEL, TYPE_VAL, CONFIG_VAL NXP_SYS_CLK_FREQ_SEL},
#if ((NFC_NXP_CHIP_TYPE == SN100) || (NFC_NXP_CHIP_TYPE == SN220))
    {CFG_NXP_SPC_COMPENSATION_COMMAND, TYPE_DATA,
     cfgData_Nxp_Spc_Compensation_Command},
#endif
//////////////////////////////////////////////////////////////////////////////#
//# The timeout value to be used for clock request acknowledgment
//# min value = 0x01 to max = 0x06
#if ((NFC_NXP_CHIP_TYPE == SN100) || (NFC_NXP_CHIP_TYPE == SN220))
    //////////////////////////////////////////////////////////////////////////////#
    //# The delay to try to start PLL/XTAL when using sys clock 256/fc units =
    //~18.8 us
    //# min value = 0x01 to max = 0x1F
    {CFG_NXP_CLOCK_REQ_DELAY, TYPE_VAL, CONFIG_VAL 0x16},
#else
    {CFG_NXP_SYS_CLOCK_TO_CFG, TYPE_VAL, CONFIG_VAL 0x06},
#endif

    //# Power to eSE is controlled by DH or PMU depending on following
    // configurations

    {CFG_NXP_ESE_POWER_DH_CONTROL, TYPE_VAL, CONFIG_VAL 0x01},

//////////////////////////////////////////////////////////////////////////////#
//# NXP TVDD configurations settings
//#    Allow NFCC to configure the external TVDD
//#    Two configurations (0x01 and 0x02) are supported
//#    Only one shall be selected (hardware dependancy)
//#       Config 1: VUP connected to VBAT
//#       Config 2: VUP connected to external 5V
#ifdef PN80T_CD_ENABLE
    {CFG_NXP_EXT_TVDD_CFG, TYPE_VAL,
     CONFIG_VAL 0x01}, // PN80T based CD with Battery
#else
    {CFG_NXP_EXT_TVDD_CFG, TYPE_VAL, CONFIG_VAL 0x02}, // PN80T Demo Board
#endif
    {CFG_NXP_EXT_TVDD_CFG_1, TYPE_DATA, cfgData_Nxp_Ext_Tvdd_Cfg_1},
    {CFG_NXP_EXT_TVDD_CFG_2, TYPE_DATA, cfgData_Nxp_Ext_Tvdd_Cfg_2},

    //////////////////////////////////////////////////////////////////////////////#
    //# NXP proprietary settings
    {CFG_NXP_ACT_PROP_EXTN, TYPE_DATA, cfgData_Nxp_Act_Prop_Extn},

    //////////////////////////////////////////////////////////////////////////////#
    //# NFC forum profile settings
    {CFG_NXP_NFC_PROFILE_EXTN, TYPE_DATA, cfgData_Nxp_Nfc_Profile_Extn},

    //////////////////////////////////////////////////////////////////////////////#
    //# NFA PROPRIETARY CFG
    {CFG_NXP_NFA_PROPRIETARY_CFG, TYPE_DATA, cfgData_Nfa_Proprietary_Cfg},

    //////////////////////////////////////////////////////////////////////////////#
    //# NXP RF configuration ALM/PLM settings
    //# This section needs to be updated with the correct values based on the
    // platform
    //# default values are applied by the Firmware

    {CFG_NXP_RF_CONF_BLK_1, TYPE_DATA, cfgData_Nxp_Rf_Conf_Blk_1},
    {CFG_NXP_RF_CONF_BLK_2, TYPE_DATA, cfgData_Nxp_Rf_Conf_Blk_2},
    {CFG_NXP_RF_CONF_BLK_3, TYPE_DATA, cfgData_Nxp_Rf_Conf_Blk_3},
    {CFG_NXP_RF_CONF_BLK_4, TYPE_DATA, cfgData_Nxp_Rf_Conf_Blk_4},
    {CFG_NXP_RF_CONF_BLK_5, TYPE_DATA, cfgData_Nxp_Rf_Conf_Blk_5},
    {CFG_NXP_RF_CONF_BLK_6, TYPE_DATA, cfgData_Nxp_Rf_Conf_Blk_6},

    //////////////////////////////////////////////////////////////////////////////#
    //# Core configuration extensions
    {CFG_NXP_CORE_CONF_EXTN, TYPE_DATA, cfgData_Nxp_Core_Conf_Extn},

    //////////////////////////////////////////////////////////////////////////////#
    //# Core configuration rf field filter settings to enable set to 01 to
    // disable set
    //# to 00 last bit
    {CFG_NXP_CORE_RF_FIELD, TYPE_DATA, cfgData_Nxp_Core_Rf_Field},

    //////////////////////////////////////////////////////////////////////////////#
    //# Core configuration settings
    {CFG_NXP_CORE_CONF, TYPE_DATA, cfgData_Nxp_Core_Conf},

    //////////////////////////////////////////////////////////////////////////////#
    //# To enable i2c fragmentation set i2c fragmentation enable 0x01 to disable
    // set
    //# to 0x00
    {CFG_NXP_I2C_FRAGMENTATION_ENABLED, TYPE_VAL, CONFIG_VAL 0x01},

    //////////////////////////////////////////////////////////////////////////////#
    //# Mifare Classic Key settings
    //#NXP_CORE_MFCKEY_SETTING={20, 02, 25,04, A0, 51, 06, A0, A1, A2, A3, A4,
    // A5,
    //#                                     A0, 52, 06, D3, F7, D3, F7, D3, F7,
    //#                                     A0, 53, 06, FF, FF, FF, FF, FF, FF,
    //#                                     A0, 54, 06, 00, 00, 00, 00, 00, 00}

    //////////////////////////////////////////////////////////////////////////////#
    //# Default SE Options
    //# No secure element 0x00
    //# eSE               0x01
    //# UICC              0x02
    //# Multi SE          0x03
    {CFG_NXP_DEFAULT_SE, TYPE_VAL, CONFIG_VAL 0x03},

    //////////////////////////////////////////////////////////////////////////////#
    //#Timeout in secs to get NFCEE Discover notification
    {CFG_NXP_DEFAULT_NFCEE_DISC_TIMEOUT, TYPE_VAL, CONFIG_VAL 20},

    //////////////////////////////////////////////////////////////////////////////#
    {CFG_NXP_DEFAULT_NFCEE_TIMEOUT, TYPE_VAL, CONFIG_VAL 20},

    //////////////////////////////////////////////////////////////////////////////#
    //#Enable SWP full power mode when device is power off
    {CFG_NXP_SWP_FULL_PWR_ON, TYPE_VAL, CONFIG_VAL 0x00},

    //////////////////////////////////////////////////////////////////////////////#
    //#Chip type
    //#PN547C2            0x01
    //#PN65T              0x02
    //#PN548AD            0x03
    //#PN66T              0x04
    {CFG_NXP_NFC_CHIP, TYPE_VAL, CONFIG_VAL 0x04},

    //////////////////////////////////////////////////////////////////////////////#
    //# CE when Screen state is locked
    //# This setting is for DEFAULT_AID_ROUTE,
    //# DEFAULT_DESFIRE_ROUTE and DEFAULT_MIFARE_CLT_ROUTE
    //# Disable           0x00
    //# Enable            0x01
    {CFG_NXP_CE_ROUTE_STRICT_DISABLE, TYPE_VAL, CONFIG_VAL 0x01},

    //////////////////////////////////////////////////////////////////////////////#
    //#Timeout in secs
    {CFG_NXP_SWP_RD_START_TIMEOUT, TYPE_VAL, CONFIG_VAL 0x0A},

    //////////////////////////////////////////////////////////////////////////////#
    //#Timeout in secs
    {CFG_NXP_SWP_RD_TAG_OP_TIMEOUT, TYPE_VAL, CONFIG_VAL 0x01},

    //////////////////////////////////////////////////////////////////////////////#
    //#Set the Mifare Desfire route Location :
    //#This settings will be used when application does not set this parameter
    //# host 0x00
    //# eSE  0x01
    //# UICC 0x02
    {CFG_DEFAULT_DESFIRE_ROUTE, TYPE_VAL, CONFIG_VAL 0x01},

    //////////////////////////////////////////////////////////////////////////////#
    //#Set the Mifare CLT route Location :
    //#This settings will be used when application does not set this parameter
    //# host 0x00
    //# eSE  0x01
    //# UICC 0x02
    {CFG_DEFAULT_MIFARE_CLT_ROUTE, TYPE_VAL, CONFIG_VAL 0x01},

    //////////////////////////////////////////////////////////////////////////////#
    //#Set the default AID Power state :
    //#This settings will be used when application does not set this parameter
    //# bit pos 0 = Switch On
    //# bit pos 1 = Switch Off
    //# bit pos 2 = Battery Off
    //# bit pos 3 = Screen Lock
    //# bit pos 4 = Screen Off
    {CFG_DEFAULT_AID_PWR_STATE, TYPE_VAL, CONFIG_VAL 0x19},

    //////////////////////////////////////////////////////////////////////////////#
    //#Set the Mifare Desfire Power state :
    //#This settings will be used when application does not set this parameter
    //# bit pos 0 = Switch On
    //# bit pos 1 = Switch Off
    //# bit pos 2 = Battery Off
    //# bit pos 3 = Screen Lock
    //# bit pos 4 = Screen Off
    {CFG_DEFAULT_DESFIRE_PWR_STATE, TYPE_VAL, CONFIG_VAL 0x1B},

    //////////////////////////////////////////////////////////////////////////////#
    //#Set the Mifare CLT Power state :
    //#This settings will be used when application does not set this parameter
    //# bit pos 0 = Switch On
    //# bit pos 1 = Switch Off
    //# bit pos 2 = Battery Off
    //# bit pos 3 = Screen Lock
    //# bit pos 4 = Screen Off
    {CFG_DEFAULT_MIFARE_CLT_PWR_STATE, TYPE_VAL, CONFIG_VAL 0x1B},

    //////////////////////////////////////////////////////////////////////////////#
    //# AID Matching platform options
    //# AID_MATCHING_L 0x01
    //# AID_MATCHING_K 0x02
    {CFG_AID_MATCHING_PLATFORM, TYPE_VAL, CONFIG_VAL 0x01},

//////////////////////////////////////////////////////////////////////////////#
//# T4T_NDEF_NFCC_ENABLE_DISABLE
//# Enable  0x01
//# Disable  0x00
#if ((NXP_EXTNS == TRUE) && (NXP_NFCEE_NDEF_ENABLE == TRUE))
    {CFG_NXP_T4T_NDEFEE_ENABLE, TYPE_VAL, CONFIG_VAL 0x01},
#else
    {CFG_NXP_T4T_NDEFEE_ENABLE, TYPE_VAL, CONFIG_VAL 0x00},
#endif

    //////////////////////////////////////////////////////////////////////////////#
    //#Set the default AID route Location :
    //#This settings will be used when application does not set this parameter
    //# host 0x00
    //# eSE  0x01
    //# UICC 0x02
    {CFG_DEFAULT_AID_ROUTE, TYPE_VAL, CONFIG_VAL 0x00},

    //////////////////////////////////////////////////////////////////////////////#
    //# Allowed range of swp timeout setting is 0x00 to 0x3C [0 - 60].
    //# Timeout in milliseconds, for example
    //# No Timeout  0x00
    //# 10 millisecond timeout 0x0A
    {CFG_NXP_SWP_SWITCH_TIMEOUT, TYPE_VAL, CONFIG_VAL 0x0A},

    //////////////////////////////////////////////////////////////////////////////#
    //#CHINA_TIANJIN_RF_SETTING
    //#Enable  0x01
    //#Disable 0x00
    {CFG_NXP_CHINA_TIANJIN_RF_ENABLED, TYPE_VAL, CONFIG_VAL 0x01},
    //////////////////////////////////////////////////////////////////////////////#
    //#NXP_CN_TRANSIT_CMA_BYPASSMODE_ENABLE
    //#Enable this config it prevents EMVCo PICC compliancy and Mifare backward
    // compatibility works
    //#Disable this config EMVCo PICC compliancy works and Mifare backward
    // compatibility will not work
    //#Default config is Disable
    //#Enable  0x01
    //#Disable  0x00
    //////////////////////////////////////////////////////////////////////////////#
    {CFG_NXP_CN_TRANSIT_CMA_BYPASSMODE_ENABLE, TYPE_VAL, CONFIG_VAL 0x00},
    //////////////////////////////////////////////////////////////////////////////#
    //## Set configuration optimization decision setting
    //## Enable    = 0x01
    //## Disable   = 0x00
    {CFG_NXP_SET_CONFIG_ALWAYS, TYPE_VAL, CONFIG_VAL 0x00},

    //////////////////////////////////////////////////////////////////////////////#
    //# Enable/Disable checking default proto SE Id
    //# Disable           0x00
    //# Enable            0x01
    {CFG_NXP_CHECK_DEFAULT_PROTO_SE_ID, TYPE_VAL, CONFIG_VAL 0x01},

#if 0
//# It is required only for some specific PN81T samples, there it should be enabled.
//#Enable            0x01
//#Disable           0x00
    {CFG_NXP_MF_CLT_WA,     TYPE_VAL, CONFIG_VAL 0x00},

    {CFG_NXP_ISO_DEP_MAX_TRANSCEIVE,     TYPE_VAL, CONFIG_VAL 0xFEFF},

    {CFG_NXP_PRESENCE_CHECK_ALGORITHM,     TYPE_VAL, CONFIG_VAL 0x02},

    {CFG_NXP_T4T_NFCEE_ENABLE,     TYPE_VAL, CONFIG_VAL 0x01},

    {CFG_NXP_DEFAULT_T4TNFCEE_AID_POWER_STATE,     TYPE_VAL, CONFIG_VAL 0x3B},
#endif
};

/*******************************************************************************
**
** Function:    CNfcConfig::find()
**
** Description: search if a setting exist in the setting array
**
** Returns:     pointer to the setting object
**
*******************************************************************************/
const NxpParam_t *NxpParamFind(const unsigned char key) {
  NXPLOG_EXTNS_D("%s, NxpParamFind key=%c", __func__, key);
  int i;
  int listSize;

  listSize = (sizeof(NXPConfig) / sizeof(NxpParam_t));

  if (listSize == 0) {
    NXPLOG_EXTNS_D("%s, NxpParamFind returned NULL as listSize is zero",
                   __func__);
    return NULL;
  }

  for (i = 0; i < listSize; ++i) {
    NXPLOG_EXTNS_D("%s, NxpParamFind iterating key=%c", __func__,
                   NXPConfig[i].key);
    NXPLOG_EXTNS_D("%s,  NxpParamFind iterating type=%c", __func__,
                   NXPConfig[i].type);
    if (NXPConfig[i].type == TYPE_DATA) {
      NXPLOG_EXTNS_D("%s,  data len = %d\n", __func__,
                     *((unsigned char *)(NXPConfig[i].val)));
    } else {
      NXPLOG_EXTNS_D("%s , (0x%lx)\n", __func__,
                     (long unsigned int)NXPConfig[i].val);
    }

    if (NXPConfig[i].key == key) {
      if (NXPConfig[i].type == TYPE_DATA) {
        NXPLOG_EXTNS_D("%s found key %d, data len = %d\n", __func__, key,
                       *((unsigned char *)(NXPConfig[i].val)));
      } else {
        NXPLOG_EXTNS_D("%s found key %d = (0x%lx)\n", __func__, key,
                       (long unsigned int)NXPConfig[i].val);
      }
      return &(NXPConfig[i]);
    }
  }
  return NULL;
}

/*******************************************************************************
**
** Function:    GetStrValue()
**
** Description: API function for getting a string value of a setting
**
** Returns:     True if found, otherwise False.
**
*******************************************************************************/
int GetNxpStrValue(const char *key, char *pValue, unsigned long len) {
  if (!pValue)
    return FALSE;

  NXPLOG_EXTNS_D("%s GetNxpStrValue key=%s", __func__, key);
  const NxpParam_t *pParam = NxpParamFind(*key);

  if (pParam == NULL) {
    NXPLOG_EXTNS_D("%s GetNxpStrValue returned false pParam is NULL", __func__);
    return FALSE;
  }

  if ((pParam->type == TYPE_STR) && (pParam->val != NULL) &&
      (strlen(pParam->val) <= len)) {
    phOsalNfc_Memory_set(pValue, 0, len);
    phOsalNfc_Memory_copy(pValue, pParam->val, strlen(pParam->val));
    NXPLOG_EXTNS_D("%s GetNxpByteArrayValue *pValue=%s", __func__, pValue);
    NXPLOG_EXTNS_D("%s GetNxpStrValue returned true at the end", __func__);
    return TRUE;
  }
  NXPLOG_EXTNS_D("%s GetNxpStrValue returned false at the end", __func__);
  return FALSE;
}

/*******************************************************************************
**
** Function:    GetByteArrayValue()
**
** Description: Read byte array value from the config file.
**
** Parameters:
**              name    - name of the config param to read.
**              pValue  - pointer to input buffer.
**              len     - input buffer length.
**              readlen - out parameter to return the number of bytes read from
*config file,
**                        return -1 in case bufflen is not enough.
**
** Returns:     TRUE[1] if config param name is found in the config file, else
*FALSE[0]
**
*******************************************************************************/
int GetNxpByteArrayValue(const char *key, char *pValue, long bufflen,
                         long *readlen)
// int GetNxpByteArrayValue(unsigned char key, const void** pValue, long
// *readlen)
{
  (void)bufflen;
  NXPLOG_EXTNS_D("%s, GetNxpByteArrayValue key=%s", __func__, key);
  NXPLOG_EXTNS_D("%s, GetNxpByteArrayValue key=%c", __func__, *key);
  NXPLOG_EXTNS_D("%s, GetNxpByteArrayValue key=%c", __func__, *pValue);
  if (!*pValue) {
    NXPLOG_EXTNS_D("%s, GetNxpByteArrayValue pValue returned false", __func__);
    return FALSE;
  }
  const NxpParam_t *pParam = NxpParamFind(*key);

  if (pParam == NULL) {
    NXPLOG_EXTNS_D("%s, GetNxpByteArrayValue pParam returned false", __func__);
    return FALSE;
  }

  if ((pParam->type == TYPE_DATA) && (pParam->val != 0)) {
    //*pValue = &(((unsigned char *)pParam->val)[1]);
    *pValue = (((unsigned char *)pParam->val)[1]);
    NXPLOG_EXTNS_D("%s, GetNxpByteArrayValue pValue=%s", __func__, pValue);
    *readlen = (long)((unsigned char *)(pParam->val))[0];
    NXPLOG_EXTNS_D("%s GetNxpByteArrayValue at the end returned true",
                   __func__);
    return TRUE;
  }
  NXPLOG_EXTNS_D("%s GetNxpByteArrayValue at the end returned false", __func__);
  return FALSE;
}

/*******************************************************************************
**
** Function:    GetNumValue
**
** Description: API function for getting a numerical value of a setting
**
** Returns:     true, if successful
**
*******************************************************************************/
// int GetNxpNumValue(unsigned char key, void* pValue, unsigned long len)
int GetNxpNumValue(const char *key, void *pValue, unsigned long len) {
  if (!pValue)
    return FALSE;

  const NxpParam_t *pParam = NxpParamFind(*key);

  if (pParam == NULL)
    return FALSE;

  unsigned long v = (unsigned long)pParam->val;

  switch (len) {
  case sizeof(unsigned long):
    *((unsigned long *)(pValue)) = (unsigned long)v;
    break;
  case sizeof(unsigned short):
    *((unsigned short *)(pValue)) = (unsigned short)v;
    break;
  case sizeof(unsigned char):
    *((unsigned char *)(pValue)) = (unsigned char)v;
    break;
  default:
    return FALSE;
  }

  return TRUE;
}
