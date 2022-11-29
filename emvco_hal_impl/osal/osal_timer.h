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

/*
 * OSAL header files related to Timer functions.
 */

#ifndef _OSAL_TIMER_H_
#define _OSAL_TIMER_H_
#include <emvco_common.h>
/*
************************* Include Files ****************************************
*/

/*
 * Timer callback interface which will be called once registered timer
 * time out expires.
 *        TimerId  - Timer Id for which callback is called.
 *        p_context - Parameter to be passed to the callback function
 */
typedef void (*p_osal_emvco_timer_callback_t)(uint32_t TimerId,
                                              void *p_context);

/*
 * The Timer could not be created due to a
 * system error */
#define PH_OSALNFC_TIMER_CREATE_ERROR (0X00E0)

/*
 * The Timer could not be started due to a
 * system error or invalid handle */
#define PH_OSALNFC_TIMER_START_ERROR (0X00E1)

/*
 * The Timer could not be stopped due to a
 * system error or invalid handle */
#define PH_OSALNFC_TIMER_STOP_ERROR (0X00E2)

/*
 * The Timer could not be deleted due to a
 * system error or invalid handle */
#define PH_OSALNFC_TIMER_DELETE_ERROR (0X00E3)

/*
 * Invalid timer ID type.This ID used indicate timer creation is failed */
#define PH_OSALNFC_TIMER_ID_INVALID (0xFFFF)

/*
 * OSAL timer message .This message type will be posted to
 * calling application thread.*/
#define PH_OSALNFC_TIMER_MSG (0x315)

/*
***************************Globals,Structure and Enumeration ******************
*/

uint32_t osal_timer_create(void);
EMVCO_STATUS
osal_timer_start(uint32_t dw_timer_id, uint32_t dwRegTimeCnt,
                 p_osal_emvco_timer_callback_t pApplication_callback,
                 void *p_context);
EMVCO_STATUS osal_timer_stop(uint32_t dw_timer_id);
EMVCO_STATUS osal_timer_delete(uint32_t dw_timer_id);
void osal_timer_cleanup(void);
uint32_t osal_check_for_available_timer(void);
EMVCO_STATUS osal_check_timer_presence(void *pobject_handle);

#endif /* _OSAL_TIMER_H_ */
