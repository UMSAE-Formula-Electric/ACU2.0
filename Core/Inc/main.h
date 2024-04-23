/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define COOL_SYS_TEMP3_Pin GPIO_PIN_0
#define COOL_SYS_TEMP3_GPIO_Port GPIOC
#define VBATT_Pin GPIO_PIN_2
#define VBATT_GPIO_Port GPIOC
#define PUMP_FB_Pin GPIO_PIN_3
#define PUMP_FB_GPIO_Port GPIOC
#define FANS_FB_Pin GPIO_PIN_0
#define FANS_FB_GPIO_Port GPIOA
#define BUZR_FB_Pin GPIO_PIN_1
#define BUZR_FB_GPIO_Port GPIOA
#define COOL_SYS_TEMP1_Pin GPIO_PIN_3
#define COOL_SYS_TEMP1_GPIO_Port GPIOA
#define BSPD_OK_Pin GPIO_PIN_4
#define BSPD_OK_GPIO_Port GPIOA
#define LVBATT_CURR_SENS_Pin GPIO_PIN_5
#define LVBATT_CURR_SENS_GPIO_Port GPIOA
#define MC_PWR_FB_Pin GPIO_PIN_6
#define MC_PWR_FB_GPIO_Port GPIOA
#define COOL_SYS_TEMP2_Pin GPIO_PIN_7
#define COOL_SYS_TEMP2_GPIO_Port GPIOA
#define HCP_H_CURR_SENS_Pin GPIO_PIN_4
#define HCP_H_CURR_SENS_GPIO_Port GPIOC
#define HCP_L_CURR_SENS_Pin GPIO_PIN_5
#define HCP_L_CURR_SENS_GPIO_Port GPIOC
#define MC_PWR_CTRL_Pin GPIO_PIN_7
#define MC_PWR_CTRL_GPIO_Port GPIOE
#define PUMP_CTRL_Pin GPIO_PIN_10
#define PUMP_CTRL_GPIO_Port GPIOE
#define FANS_CTRL_Pin GPIO_PIN_15
#define FANS_CTRL_GPIO_Port GPIOE
#define IMD_OK_Pin GPIO_PIN_12
#define IMD_OK_GPIO_Port GPIOB
#define AMS_OK_Pin GPIO_PIN_13
#define AMS_OK_GPIO_Port GPIOB
#define AIR__FB_Pin GPIO_PIN_14
#define AIR__FB_GPIO_Port GPIOB
#define AIR__FBB15_Pin GPIO_PIN_15
#define AIR__FBB15_GPIO_Port GPIOB
#define AMS_CLR_ERR_Pin GPIO_PIN_8
#define AMS_CLR_ERR_GPIO_Port GPIOD
#define AIR__CTRL_Pin GPIO_PIN_9
#define AIR__CTRL_GPIO_Port GPIOD
#define AIR__CTRLD10_Pin GPIO_PIN_10
#define AIR__CTRLD10_GPIO_Port GPIOD
#define SHCT_TAP_Pin GPIO_PIN_12
#define SHCT_TAP_GPIO_Port GPIOD
#define BUZR_CTRL_Pin GPIO_PIN_11
#define BUZR_CTRL_GPIO_Port GPIOA
#define BRAKE_PRESS_Pin GPIO_PIN_3
#define BRAKE_PRESS_GPIO_Port GPIOD
#define IMD_RESIST_Pin GPIO_PIN_7
#define IMD_RESIST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */