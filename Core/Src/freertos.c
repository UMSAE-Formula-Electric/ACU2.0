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
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for chargeStateTask */
osThreadId_t chargeStateTaskHandle;
const osThreadAttr_t chargeStateTask_attributes = {
  .name = "chargeStateTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for vcuHrtBeatTask */
osThreadId_t vcuHrtBeatTaskHandle;
const osThreadAttr_t vcuHrtBeatTask_attributes = {
  .name = "vcuHrtBeatTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for lvbmChargeTask */
osThreadId_t lvbmChargeTaskHandle;
const osThreadAttr_t lvbmChargeTask_attributes = {
  .name = "lvbmChargeTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for canTxTask */
osThreadId_t canTxTaskHandle;
const osThreadAttr_t canTxTask_attributes = {
  .name = "canTxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for canRxTask */
osThreadId_t canRxTaskHandle;
const osThreadAttr_t canRxTask_attributes = {
  .name = "canRxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for coolingTask */
osThreadId_t coolingTaskHandle;
const osThreadAttr_t coolingTask_attributes = {
  .name = "coolingTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for watchDogTask */
osThreadId_t watchDogTaskHandle;
const osThreadAttr_t watchDogTask_attributes = {
  .name = "watchDogTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
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
extern void StartChargeStateTask(void *argument);
extern void StartVcuHrtBeatTask(void *argument);
extern void StartLvbmChargeTask(void *argument);
extern void StartCanTxTask(void *argument);
extern void StartCanRxTask(void *argument);
extern void StartCoolingTask(void *argument);
extern void StartWatchDogTask(void *argument);

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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of acuStateTask */
  acuStateTaskHandle = osThreadNew(StartAcuStateTask, (void*) ACU_STATE_TASK_ENABLED, &acuStateTask_attributes);

  /* creation of chargeStateTask */
  chargeStateTaskHandle = osThreadNew(StartChargeStateTask, (void*) CHARGE_STATE_TASK_ENABLED, &chargeStateTask_attributes);

  /* creation of vcuHrtBeatTask */
  vcuHrtBeatTaskHandle = osThreadNew(StartVcuHrtBeatTask, (void*) VCU_HRTBEAT_TASK_ENABLED, &vcuHrtBeatTask_attributes);

  /* creation of lvbmChargeTask */
  lvbmChargeTaskHandle = osThreadNew(StartLvbmChargeTask, (void*) LVBM_CHARGE_TASK_ENABLED, &lvbmChargeTask_attributes);

  /* creation of canTxTask */
  canTxTaskHandle = osThreadNew(StartCanTxTask, (void*) CAN_TX_TASK_ENABLED, &canTxTask_attributes);

  /* creation of canRxTask */
  canRxTaskHandle = osThreadNew(StartCanRxTask, (void*) CAN_RX_TASK_ENABLED, &canRxTask_attributes);

  /* creation of coolingTask */
  coolingTaskHandle = osThreadNew(StartCoolingTask, (void*) COOLING_TASK_ENABLED, &coolingTask_attributes);

  /* creation of watchDogTask */
  watchDogTaskHandle = osThreadNew(StartWatchDogTask, (void*) WATCH_DOG_TASK_ENABLED, &watchDogTask_attributes);

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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

