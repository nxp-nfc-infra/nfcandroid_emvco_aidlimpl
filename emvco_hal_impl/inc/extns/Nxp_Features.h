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

#ifndef NXP_FEATURES_H
#define NXP_FEATURES_H
#include <unistd.h>

#define FW_MOBILE_MAJOR_NUMBER_PN553 0x01
#define FW_MOBILE_MAJOR_NUMBER_PN81A 0x02
#define FW_MOBILE_MAJOR_NUMBER_PN551 0x05
#define FW_MOBILE_MAJOR_NUMBER_PN557 0x01
#define FW_MOBILE_MAJOR_NUMBER_PN548AD 0x01

typedef enum {
  unknown,
  pn547C2,
  pn65T,
  pn548C2,
  pn66T,
  pn551,
  pn67T,
  pn553,
  pn80T,
  pn557,
  pn81T
} tNFC_chipType;

typedef struct {
  tNFC_chipType chipType;
  uint16_t _PHDNLDNFC_USERDATA_EEPROM_OFFSET;
  uint16_t _PHDNLDNFC_USERDATA_EEPROM_LEN;
  uint8_t _FW_MOBILE_MAJOR_NUMBER;
} tNfc_featureList;

extern tNfc_featureList nfcFL;

#define CONFIGURE_FEATURELIST(chipType)                                        \
  {                                                                            \
    nfcFL.chipType = chipType;                                                 \
    if (chipType == pn81T) {                                                   \
      nfcFL.chipType = pn557;                                                  \
    } else if (chipType == pn80T) {                                            \
      nfcFL.chipType = pn553;                                                  \
    } else if (chipType == pn67T) {                                            \
      nfcFL.chipType = pn551;                                                  \
    } else if (chipType == pn66T) {                                            \
      nfcFL.chipType = pn548C2;                                                \
    }                                                                          \
    CONFIGURE_FEATURELIST_NFCC(chipType)                                       \
  }

#define CONFIGURE_FEATURELIST_NFCC(chipType)                                   \
  {                                                                            \
    nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET = 0x023CU;                         \
    nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN = 0x0C80U;                            \
                                                                               \
    if (chipType == pn557 || chipType == pn81T) {                              \
                                                                               \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN557;            \
    } else if (chipType == pn553 || chipType == pn80T) {                       \
                                                                               \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN553;            \
                                                                               \
    } else if (chipType == pn551 || chipType == pn67T) {                       \
                                                                               \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET = 0x02BCU;                       \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN = 0x0C00U;                          \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN551;            \
                                                                               \
    } else if (chipType == pn548C2 || chipType == pn66T) {                     \
                                                                               \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET = 0x02BCU;                       \
      nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN = 0x0C00U;                          \
      nfcFL._FW_MOBILE_MAJOR_NUMBER = FW_MOBILE_MAJOR_NUMBER_PN548AD;          \
    }                                                                          \
  }

#endif
