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

#include <phNxpLog.h>
#include <phOsalNfc_Memory.h>
#include <stdlib.h>
#include <string.h>
void *phOsalNfc_Memory_malloc(int size) {
  NXPLOG_TML_D("%s\n", __func__);
  return malloc(size);
}

void phOsalNfc_Memory_free(void *pMem) {
  NXPLOG_TML_D("%s\n", __func__);
  return free(pMem);
}

void phOsalNfc_Memory_set(void *pMem, int bVal, int size) {
  NXPLOG_TML_D("%s\n", __func__);
  memset(pMem, bVal, size);
}

void phOsalNfc_Memory_copy(void *pDest, const void *pSrc, int size) {
  NXPLOG_TML_D("%s\n", __func__);
  memcpy(pDest, pSrc, size);
}