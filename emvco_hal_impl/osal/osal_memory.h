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
#ifndef _OSAL_MEMORY_H_
#define _OSAL_MEMORY_H_

/** \addtogroup EMVCO_STACK_OSAL_API_INTERFACE
 *  @{
 */

/**
 *
 * @brief  Allocates some memoryAllocates some memory
 *
 * @param[in] dwSize   Size, in uint32_t, to be allocated
 *
 * @return            NON-NULL value:  The memory is successfully allocated ;
 *                    the return value is a pointer to the allocated memory
 * location NULL:The operation is not successful.
 *
 */
void *osal_malloc(int dwSize);

/**
 * @brief This API allows to free already allocated memory.
 *
 * \param[in] pMem  Pointer to the memory block to be deallocated
 * @return    void
 */
void osal_free(void *pMem);

/**
 * @brief                Sets the given value in the memory locations.
 *
 * @param[in] pMem      Pointer to the memory block to be set to a value
 * @param[in] bVal      Value to be set
 * @param[in] dwSize    Number of bytes to be set.
 *
 * @return    void
 */
void osal_memset(void *pMem, int bVal, int dwSize);

/**
 * @brief                Copies the values stored in the source memory to the
 *                       values stored in the destination memory.
 *
 * @param[in] pDest     Pointer to the Destination Memory
 * @param[in] pSrc      Pointer to the Source Memory
 * @param[in] dwSize    Number of bytes to be copied.
 *
 * @return    void
 */
void osal_memcpy(void *pDest, const void *pSrc, int size);

/** @}*/
#endif /* _OSAL_MEMORY_H_*/