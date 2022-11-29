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

#include <emvco_log.h>
#include <osal_memory.h>
#include <stdlib.h>
#include <string.h>
void *osal_malloc(int size) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return malloc(size);
}

void osal_free(void *pMem) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  return free(pMem);
}

void osal_memset(void *pMem, int bVal, int size) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  memset(pMem, bVal, size);
}

void osal_memcpy(void *pDest, const void *pSrc, int size) {
  LOG_EMVCO_TML_D("%s\n", __func__);
  memcpy(pDest, pSrc, size);
}