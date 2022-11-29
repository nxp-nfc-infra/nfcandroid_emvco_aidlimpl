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

#if !defined(LOG__H_INCLUDED)
#define LOG__H_INCLUDED
#include <log/log.h>
#include <osal_log.h>

typedef struct nci_log_level {
  uint8_t global_log_level;
  uint8_t extns_log_level;
  uint8_t hal_log_level;
  uint8_t dnld_log_level;
  uint8_t tml_log_level;
  uint8_t ncix_log_level;
  uint8_t ncir_log_level;
} nci_log_level_t;

/* global log level Ref */
extern nci_log_level_t gLog_level;
extern bool emvco_debug_enabled;
/* define log module included when compile */
#define ENABLE_EXTNS_TRACES TRUE
#define ENABLE_HAL_TRACES TRUE
#define ENABLE_TML_TRACES TRUE
#define ENABLE_NCIX_TRACES TRUE
#define ENABLE_NCIR_TRACES TRUE

/* ####################### Set the log module name in .conf file
 * ########################## */
#define NAME_LOG_EXTNS_LOGLEVEL "LOG_EXTNS_LOGLEVEL"
#define NAME_LOG_HAL_LOGLEVEL "LOG_NCIHAL_LOGLEVEL"
#define NAME_LOG_NCIX_LOGLEVEL "LOG_NCIX_LOGLEVEL"
#define NAME_LOG_NCIR_LOGLEVEL "LOG_NCIR_LOGLEVEL"
#define NAME_LOG_TML_LOGLEVEL "LOG_TML_LOGLEVEL"

/* ####################### Set the log module name by Android property
 * ########################## */
#define PROP_NAME_LOG_GLOBAL_LOGLEVEL "emvco.log_level_global"
#define PROP_NAME_LOG_EXTNS_LOGLEVEL "emvco.log_level_extns"
#define PROP_NAME_LOG_HAL_LOGLEVEL "emvco.log_level_hal"
#define PROP_NAME_LOG_NCI_LOGLEVEL "emvco.log_level_nci"
#define PROP_NAME_LOG_TML_LOGLEVEL "emvco.log_level_tml"

/* ####################### Set the logging level for EVERY COMPONENT here
 * ######################## :START: */
#define LOG_LOG_SILENT_LOGLEVEL 0x00
#define LOG_LOG_ERROR_LOGLEVEL 0x01
#define LOG_LOG_WARN_LOGLEVEL 0x02
#define LOG_LOG_DEBUG_LOGLEVEL 0x03
/* ####################### Set the default logging level for EVERY COMPONENT
 * here ########################## :END: */

/* The Default log level for all the modules. */
#define LOG_DEFAULT_LOGLEVEL LOG_LOG_ERROR_LOGLEVEL

/* ################################################################################################################
 */
/* ############################################### Component Names
 * ################################################ */
/* ################################################################################################################
 */

extern const char *LOG_ITEM_EXTNS;  /* Android logging tag for NxpExtns  */
extern const char *LOG_ITEM_NCIHAL; /* Android logging tag for NxpNciHal */
extern const char *LOG_ITEM_NCIX;   /* Android logging tag for NxpNciX   */
extern const char *LOG_ITEM_NCIR;   /* Android logging tag for NxpNciR   */
extern const char *LOG_ITEM_TML;    /* Android logging tag for NxpTml    */

/* ######################################## Defines used for Logging data
 * ######################################### */
#ifdef VRBS_REQ
#define LOG_FUNC_ENTRY(COMP)                                                   \
  OSAL_LOG_PRI(ANDROID_LOG_VERBOSE, (COMP), "+:%s", (__func__))
#define LOG_FUNC_EXIT(COMP)                                                    \
  OSAL_LOG_PRI(ANDROID_LOG_VERBOSE, (COMP), "-:%s", (__func__))
#endif /*VRBS_REQ*/

/* ################################################################################################################
 */
/* ######################################## Logging APIs of actual modules
 * ######################################## */
/* ################################################################################################################
 */
/* Logging APIs used by NxpExtns module */
#if (ENABLE_EXTNS_TRACES == TRUE)
#define LOG_EXTNS_D(...)                                                       \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.extns_log_level >= LOG_LOG_DEBUG_LOGLEVEL))                \
      OSAL_LOG_PRI(ANDROID_LOG_DEBUG, LOG_ITEM_EXTNS, __VA_ARGS__);            \
  }
#define LOG_EXTNS_W(...)                                                       \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.extns_log_level >= LOG_LOG_WARN_LOGLEVEL))                 \
      OSAL_LOG_PRI(ANDROID_LOG_WARN, LOG_ITEM_EXTNS, __VA_ARGS__);             \
  }
#define LOG_EXTNS_E(...)                                                       \
  {                                                                            \
    if (gLog_level.extns_log_level >= LOG_LOG_ERROR_LOGLEVEL)                  \
      OSAL_LOG_PRI(ANDROID_LOG_ERROR, LOG_ITEM_EXTNS, __VA_ARGS__);            \
  }
#else
#define LOG_EXTNS_D(...)
#define LOG_EXTNS_W(...)
#define LOG_EXTNS_E(...)
#endif /* Logging APIs used by NxpExtns module */

/* Logging APIs used by NxpNciHal module */
#if (ENABLE_HAL_TRACES == TRUE)
#define LOG_EMVCOHAL_D(...)                                                    \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.hal_log_level >= LOG_LOG_DEBUG_LOGLEVEL))                  \
      OSAL_LOG_PRI(ANDROID_LOG_DEBUG, LOG_ITEM_NCIHAL, __VA_ARGS__);           \
  }
#define LOG_EMVCOHAL_W(...)                                                    \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.hal_log_level >= LOG_LOG_WARN_LOGLEVEL))                   \
      OSAL_LOG_PRI(ANDROID_LOG_WARN, LOG_ITEM_NCIHAL, __VA_ARGS__);            \
  }
#define LOG_EMVCOHAL_E(...)                                                    \
  {                                                                            \
    if (gLog_level.hal_log_level >= LOG_LOG_ERROR_LOGLEVEL)                    \
      OSAL_LOG_PRI(ANDROID_LOG_ERROR, LOG_ITEM_NCIHAL, __VA_ARGS__);           \
  }
#else
#define LOG_EMVCOHAL_D(...)
#define LOG_EMVCOHAL_W(...)
#define LOG_EMVCOHAL_E(...)
#endif /* Logging APIs used by HAL module */

/* Logging APIs used by NxpNciX module */
#if (ENABLE_NCIX_TRACES == TRUE)
#define LOG_EMVCOX_D(...)                                                      \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.ncix_log_level >= LOG_LOG_DEBUG_LOGLEVEL))                 \
      OSAL_LOG_PRI(ANDROID_LOG_DEBUG, LOG_ITEM_NCIX, __VA_ARGS__);             \
  }
#define LOG_EMVCOX_W(...)                                                      \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.ncix_log_level >= LOG_LOG_WARN_LOGLEVEL))                  \
      OSAL_LOG_PRI(ANDROID_LOG_WARN, LOG_ITEM_NCIX, __VA_ARGS__);              \
  }
#define LOG_EMVCOX_E(...)                                                      \
  {                                                                            \
    if (gLog_level.ncix_log_level >= LOG_LOG_ERROR_LOGLEVEL)                   \
      OSAL_LOG_PRI(ANDROID_LOG_ERROR, LOG_ITEM_NCIX, __VA_ARGS__);             \
  }
#else
#define LOG_EMVCOX_D(...)
#define LOG_EMVCOX_W(...)
#define LOG_EMVCOX_E(...)
#endif /* Logging APIs used by NCIx module */

/* Logging APIs used by NxpNciR module */
#if (ENABLE_NCIR_TRACES == TRUE)
#define LOG_EMVCOR_D(...)                                                      \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.ncir_log_level >= LOG_LOG_DEBUG_LOGLEVEL))                 \
      OSAL_LOG_PRI(ANDROID_LOG_DEBUG, LOG_ITEM_NCIR, __VA_ARGS__);             \
  }
#define LOG_EMVCOR_W(...)                                                      \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.ncir_log_level >= LOG_LOG_WARN_LOGLEVEL))                  \
      OSAL_LOG_PRI(ANDROID_LOG_WARN, LOG_ITEM_NCIR, __VA_ARGS__);              \
  }
#define LOG_EMVCOR_E(...)                                                      \
  {                                                                            \
    if (gLog_level.ncir_log_level >= LOG_LOG_ERROR_LOGLEVEL)                   \
      OSAL_LOG_PRI(ANDROID_LOG_ERROR, LOG_ITEM_NCIR, __VA_ARGS__);             \
  }
#else
#define LOG_EMVCOR_D(...)
#define LOG_EMVCOR_W(...)
#define LOG_EMVCOR_E(...)
#endif /* Logging APIs used by NCIR module */

/* Logging APIs used by NxpTml module */
#if (ENABLE_TML_TRACES == TRUE)
#define LOG_EMVCO_TML_D(...)                                                   \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.tml_log_level >= LOG_LOG_DEBUG_LOGLEVEL))                  \
      OSAL_LOG_PRI(ANDROID_LOG_DEBUG, LOG_ITEM_TML, __VA_ARGS__);              \
  }
#define LOG_EMVCO_TML_W(...)                                                   \
  {                                                                            \
    if ((emvco_debug_enabled) ||                                               \
        (gLog_level.tml_log_level >= LOG_LOG_WARN_LOGLEVEL))                   \
      OSAL_LOG_PRI(ANDROID_LOG_WARN, LOG_ITEM_TML, __VA_ARGS__);               \
  }
#define LOG_EMVCO_TML_E(...)                                                   \
  {                                                                            \
    if (gLog_level.tml_log_level >= LOG_LOG_ERROR_LOGLEVEL)                    \
      OSAL_LOG_PRI(ANDROID_LOG_ERROR, LOG_ITEM_TML, __VA_ARGS__);              \
  }
#else
#define LOG_EMVCO_TML_D(...)
#define LOG_EMVCO_TML_W(...)
#define LOG_EMVCO_TML_E(...)
#endif /* Logging APIs used by NxpTml module */

void initialize_log_level(void);

#endif /* LOG__H_INCLUDED */
