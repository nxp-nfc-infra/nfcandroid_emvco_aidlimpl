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
#define LOG_TAG "NxpNfcHal"
#include <stdio.h>
#include <string.h>
#if !defined(LOG__H_INCLUDED)
#include "emvco_config.h"
#include "emvco_log.h"
#endif
#include <cutils/properties.h>
#include <log/log.h>

const char *LOG_ITEM_EXTNS = "EMVCoExtns";
const char *LOG_ITEM_NCIHAL = "EMVCoHal";
const char *LOG_ITEM_NCIX = "EMVCoNciX";
const char *LOG_ITEM_NCIR = "EMVCoNciR";
const char *LOG_ITEM_TML = "EMVCoTml";

/* global log level structure */
nci_log_level_t gLog_level;

extern bool emvco_debug_enabled;

/*******************************************************************************
 *
 * Function         phNxpLog_SetGlobalLogLevel
 *
 * Description      Sets the global log level for all modules.
 *                  This value is set by Android property
 *nfc.log_level_global.
 *                  If value can be overridden by module log level.
 *
 * Returns          The value of global log level
 *
 ******************************************************************************/
static uint8_t set_global_log_level(void) {
  uint8_t level = LOG_DEFAULT_LOGLEVEL;
  unsigned long num = 0;
  char valueStr[PROPERTY_VALUE_MAX] = {0};

  int len = property_get(PROP_NAME_LOG_GLOBAL_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    level = (unsigned char)num;
  }
  memset(&gLog_level, level, sizeof(nci_log_level_t));
  return level;
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetHALLogLevel
 *
 * Description      Sets the HAL layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void set_hal_log_level(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};

  if (get_num_value(NAME_LOG_HAL_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.hal_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }

  len = property_get(PROP_NAME_LOG_HAL_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.hal_log_level = (unsigned char)num;
  }
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetExtnsLogLevel
 *
 * Description      Sets the Extensions layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void set_extn_log_level(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (get_num_value(NAME_LOG_EXTNS_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.extns_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }

  len = property_get(PROP_NAME_LOG_EXTNS_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.extns_log_level = (unsigned char)num;
  }
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetTmlLogLevel
 *
 * Description      Sets the Tml layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void set_tml_log_level(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (get_num_value(NAME_LOG_TML_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.tml_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }

  len = property_get(PROP_NAME_LOG_TML_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.tml_log_level = (unsigned char)num;
  }
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetNciTxLogLevel
 *
 * Description      Sets the NCI transaction layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void set_nci_tx_log_level(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (get_num_value(NAME_LOG_NCIX_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.ncix_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
  }
  if (get_num_value(NAME_LOG_NCIR_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.ncir_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }

  len = property_get(PROP_NAME_LOG_NCI_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.ncix_log_level = (unsigned char)num;
    gLog_level.ncir_log_level = (unsigned char)num;
  }
}

void initialize_log_level(void) {
  uint8_t level = set_global_log_level();
  set_hal_log_level(level);
  set_extn_log_level(level);
  set_tml_log_level(level);
  set_nci_tx_log_level(level);

  ALOGD_IF(emvco_debug_enabled, "%s: global =%u, Fwdnld =%u, extns =%u, \
                hal =%u, tml =%u, ncir =%u, \
                ncix =%u",
           __func__, gLog_level.global_log_level, gLog_level.dnld_log_level,
           gLog_level.extns_log_level, gLog_level.hal_log_level,
           gLog_level.tml_log_level, gLog_level.ncir_log_level,
           gLog_level.ncix_log_level);
}
