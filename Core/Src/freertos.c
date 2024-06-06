/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_utils.h"
#include "iwdg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for acuStateTask */
osThreadId_t acuStateTaskHandle;
const osThreadAttr_t acuStateTask_attributes = {
  .name = "acuStateTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for vcuHrtBeatTask */
osThreadId_t vcuHrtBeatTaskHandle;
const osThreadAttr_t vcuHrtBeatTask_attributes = {
  .name = "vcuHrtBeatTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for canTxTask */
osThreadId_t canTxTaskHandle;
const osThreadAttr_t canTxTask_attributes = {
  .name = "canTxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for canRxTask */
osThreadId_t canRxTaskHandle;
const osThreadAttr_t canRxTask_attributes = {
  .name = "canRxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for coolingTask */
osThreadId_t coolingTaskHandle;
const osThreadAttr_t coolingTask_attributes = {
  .name = "coolingTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for watchDogTask */
osThreadId_t watchDogTaskHandle;
const osThreadAttr_t watchDogTask_attributes = {
  .name = "watchDogTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DebugLEDTask */
osThreadId_t DebugLEDTaskHandle;
const osThreadAttr_t DebugLEDTask_attributes = {
  .name = "DebugLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for bmsCanCommTask */
osThreadId_t bmsCanCommTaskHandle;
const osThreadAttr_t bmsCanCommTask_attributes = {
  .name = "bmsCanCommTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for vcuCanCommsTask */
osThreadId_t vcuCanCommsTaskHandle;
const osThreadAttr_t vcuCanCommsTask_attributes = {
  .name = "vcuCanCommsTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for canRxPacketQueue */
osMessageQueueId_t canRxPacketQueueHandle;
const osMessageQueueAttr_t canRxPacketQueue_attributes = {
  .name = "canRxPacketQueue"
};
/* Definitions for canTxPacketQueue */
osMessageQueueId_t canTxPacketQueueHandle;
const osMessageQueueAttr_t canTxPacketQueue_attributes = {
  .name = "canTxPacketQueue"
};
/* Definitions for setCarStateQueue */
osMessageQueueId_t setCarStateQueueHandle;
const osMessageQueueAttr_t setCarStateQueue_attributes = {
  .name = "setCarStateQueue"
};
/* Definitions for bmsRxCanMsgQueue */
osMessageQueueId_t bmsRxCanMsgQueueHandle;
const osMessageQueueAttr_t bmsRxCanMsgQueue_attributes = {
  .name = "bmsRxCanMsgQueue"
};
/* Definitions for vcuCanCommsQueue */
osMessageQueueId_t vcuCanCommsQueueHandle;
const osMessageQueueAttr_t vcuCanCommsQueue_attributes = {
  .name = "vcuCanCommsQueue"
};
/* Definitions for iwdgEventGroup */
osEventFlagsId_t iwdgEventGroupHandle;
const osEventFlagsAttr_t iwdgEventGroup_attributes = {
  .name = "iwdgEventGroup"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void StartAcuStateTask(void *argument);
extern void StartVcuHrtBeatTask(void *argument);
extern void StartCanTxTask(void *argument);
extern void StartCanRxTask(void *argument);
extern void StartCoolingTask(void *argument);
extern void StartWatchDogTask(void *argument);
extern void StartDebugLEDTask(void *argument);
extern void StartBmsCanCommTask(void *argument);
extern void StartVcuCanCommsTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of canRxPacketQueue */
  canRxPacketQueueHandle = osMessageQueueNew (32, sizeof(CAN_RxPacketTypeDef), &canRxPacketQueue_attributes);

  /* creation of canTxPacketQueue */
  canTxPacketQueueHandle = osMessageQueueNew (32, sizeof(CAN_TxPacketTypeDef), &canTxPacketQueue_attributes);

  /* creation of setCarStateQueue */
  setCarStateQueueHandle = osMessageQueueNew (32, sizeof(uint8_t), &setCarStateQueue_attributes);

  /* creation of bmsRxCanMsgQueue */
  bmsRxCanMsgQueueHandle = osMessageQueueNew (16, sizeof(CAN_RxPacketTypeDef), &bmsRxCanMsgQueue_attributes);

  /* creation of vcuCanCommsQueue */
  vcuCanCommsQueueHandle = osMessageQueueNew (16, sizeof(CAN_RxPacketTypeDef), &vcuCanCommsQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, (void*) DEFAULT_TASK_ENABLED, &defaultTask_attributes);

  /* creation of acuStateTask */
  acuStateTaskHandle = osThreadNew(StartAcuStateTask, (void*) ACU_STATE_TASK_ENABLED, &acuStateTask_attributes);

  /* creation of vcuHrtBeatTask */
  vcuHrtBeatTaskHandle = osThreadNew(StartVcuHrtBeatTask, (void*) VCU_HRTBEAT_TASK_ENABLED, &vcuHrtBeatTask_attributes);

  /* creation of canTxTask */
  canTxTaskHandle = osThreadNew(StartCanTxTask, (void*) CAN_TX_TASK_ENABLED, &canTxTask_attributes);

  /* creation of canRxTask */
  canRxTaskHandle = osThreadNew(StartCanRxTask, (void*) CAN_RX_TASK_ENABLED, &canRxTask_attributes);

  /* creation of coolingTask */
  coolingTaskHandle = osThreadNew(StartCoolingTask, (void*) COOLING_TASK_ENABLED, &coolingTask_attributes);

  /* creation of watchDogTask */
  watchDogTaskHandle = osThreadNew(StartWatchDogTask, (void*) WATCH_DOG_TASK_ENABLED, &watchDogTask_attributes);

  /* creation of DebugLEDTask */
  DebugLEDTaskHandle = osThreadNew(StartDebugLEDTask, (void*) LED_TASK_ENABLED, &DebugLEDTask_attributes);

  /* creation of bmsCanCommTask */
  bmsCanCommTaskHandle = osThreadNew(StartBmsCanCommTask, (void*) BMS_CAN_COMM_TASK_ENABLED, &bmsCanCommTask_attributes);

  /* creation of vcuCanCommsTask */
  vcuCanCommsTaskHandle = osThreadNew(StartVcuCanCommsTask, (void*) VCU_CAN_COMMS_TASK_ENABLED, &vcuCanCommsTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of iwdgEventGroup */
  iwdgEventGroupHandle = osEventFlagsNew(&iwdgEventGroup_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadExit();
    }

  /* Infinite loop */
  for(;;)
  {
      kickWatchdogBit(osThreadGetId());
      osThreadYield();
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

