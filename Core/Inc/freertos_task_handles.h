/*
 * freertos_task_handles.h
 *
 *  Created on: Apr 14, 2024
 *      Author: tonyz
 */

#ifndef INC_FREERTOS_TASK_HANDLES_H_
#define INC_FREERTOS_TASK_HANDLES_H_

#include "cmsis_os.h"

extern osThreadId_t defaultTaskHandle;
extern osThreadId_t acuStateTaskHandle;
extern osThreadId_t vcuHrtBeatTaskHandle;
extern osThreadId_t canTxTaskHandle;
extern osThreadId_t canRxTaskHandle;
extern osThreadId_t coolingTaskHandle;
extern osThreadId_t watchDogTaskHandle;
extern osThreadId_t bmsCanCommTaskHandle;

extern const osThreadAttr_t defaultTask_attributes;
extern const osThreadAttr_t acuStateTask_attributes;
extern const osThreadAttr_t vcuHrtBeatTask_attributes;
extern const osThreadAttr_t canTxTask_attributes;
extern const osThreadAttr_t canRxTask_attributes;
extern const osThreadAttr_t coolingTask_attributes;
extern const osThreadAttr_t watchDogTask_attributes;
extern const osThreadAttr_t bmsCanCommTask_attributes;

#endif /* INC_FREERTOS_TASK_HANDLES_H_ */
