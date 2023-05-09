/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
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
/******************************************************************************
 *
 *  This file contains the definition from NCI specification
 *
 ******************************************************************************/

#ifndef NCI_DEFS_H
#define NCI_DEFS_H

#include <stdint.h>

/* Define the message header size for all NCI Commands and Notifications.
 */
#define NCI_MSG_HDR_SIZE 3  /* per NCI spec */
#define NCI_DATA_HDR_SIZE 3 /* per NCI spec */

#define NCI_MT_SHIFT 5

#define MT_CMD 1 /* (MT_CMD << NCI_MT_SHIFT) = 0x20 */
#define MT_RSP 2 /* (MT_RSP << NCI_MT_SHIFT) = 0x40 */
#define MT_NTF 3 /* (MT_NTF << NCI_MT_SHIFT) = 0x60 */

/* Status Codes */
#define NCI_STATUS_OK 0x00
#define NCI_STATUS_FAILED 0x03

/* GID: Group Identifier (byte 0) */
#define NCI_GID_MASK 0x0F
#define NCI_GID_CORE 0x00      /* 0000b NCI Core group */
#define NCI_GID_RF_MANAGE 0x01 /* 0001b RF Management group */
#define NCI_GID_EE_MANAGE 0x02 /* 0010b NFCEE Management group */
#define NCI_GID_PROP 0x0F      /* 1111b Proprietary */
/* 0111b - 1110b RFU */

/* OID: Opcode Identifier (byte 1) */
#define NCI_OID_MASK 0x3F
#define NCI_OID_SHIFT 0

/**********************************************
 * RF MANAGEMENT Group Opcode    - 1
 **********************************************/
#define NCI_MSG_RF_DISCOVER_MAP 0
#define NCI_MSG_RF_SET_ROUTING 1
#define NCI_MSG_RF_GET_ROUTING 2
#define NCI_MSG_RF_DISCOVER 3
#define NCI_MSG_RF_DISCOVER_SELECT 4
#define NCI_MSG_RF_INTF_ACTIVATED 5
#define NCI_MSG_RF_DEACTIVATE 6
#define NCI_MSG_RF_FIELD 7
#define NCI_DISCOVER_PARAM_SIZE_DEACT 0x01

/* builds byte0 of NCI Command and Notification packet */
#define NCI_MSG_BLD_HDR0(p, mt, gid)                                           \
  *(p)++ = (uint8_t)(((mt) << NCI_MT_SHIFT) | (gid));

/* builds byte1 of NCI Command and Notification packet */
#define NCI_MSG_BLD_HDR1(p, oid) *(p)++ = (uint8_t)(((oid) << NCI_OID_SHIFT));

#define UINT8_TO_STREAM(p, u8)                                                 \
  { *(p)++ = (uint8_t)(u8); }

#define ARRAY_TO_STREAM(p, a, len)                                             \
  {                                                                            \
    int ijk;                                                                   \
    for (ijk = 0; ijk < (len); ijk++)                                          \
      *(p)++ = (uint8_t)(a)[ijk];                                              \
  }
/**********************************************
 * NCI Core Group Opcode        - 0
 **********************************************/
#define MSG_CORE_RESET 0
#define MSG_CORE_INIT 1
#define MSG_CORE_SET_CONFIG 2
#define MSG_CORE_GET_CONFIG 3

/* Define the message header size for all NCI Commands and Notifications.
 */
#define NCI_MSG_HDR_SIZE 3 /* per NCI spec */

/**********************************************
 * NCI Core Group Params
 **********************************************/
#define NCI_CORE_PARAM_SIZE_RESET 0x01

#define NCI_VERSION_UNKNOWN 0x00
#define NCI_VERSION_1_0 0x10
#define NCI_VERSION_2_0 0x20
#define NCI_CORE_PARAM_SIZE_INIT(X) (((X) == NCI_VERSION_2_0) ? (0x02) : (0x00))
#define NCI2_0_CORE_INIT_CMD_BYTE_0 0x00
#define NCI2_0_CORE_INIT_CMD_BYTE_1 0x00

#endif /* NCI_DEFS_H */