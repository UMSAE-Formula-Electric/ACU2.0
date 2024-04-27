#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdint.h"

#include "buzzer.h"
#include "gpio.h"

#define BUZZER_DELAY 2000 //[ms] the length of the RTD buzzer sound
//Just for test
/*
 * sound_buzzer
 *
 * @Brief: This function is used for sounding the RTD buzzer. This function
 * must be called from a task after the scheduler has been started. This function
 * will delay for BUZZER_DELAY ms.
 */
void sound_buzzer(){
  HAL_GPIO_WritePin(BUZR_CTRL_GPIO_Port, BUZR_CTRL_Pin, GPIO_PIN_SET);
  vTaskDelay(pdMS_TO_TICKS(BUZZER_DELAY));
  HAL_GPIO_WritePin(BUZR_CTRL_GPIO_Port, BUZR_CTRL_Pin, GPIO_PIN_RESET);
}

/*
 * sound_buzzer
 *
 * @Brief: This function is used for sounding the RTD buzzer. This function
 * must be called from a task after the scheduler has been started. This function
 * will delay for BUZZER_DELAY ms.
 */
void sound_buzzer_speaker_jenk(){
  for(uint8_t ii = 0; ii < BUZZER_DELAY/10; ii++){
    HAL_GPIO_WritePin(BUZR_CTRL_GPIO_Port, BUZR_CTRL_Pin, GPIO_PIN_SET);
    vTaskDelay(pdMS_TO_TICKS(5));
    HAL_GPIO_WritePin(BUZR_CTRL_GPIO_Port, BUZR_CTRL_Pin, GPIO_PIN_RESET);
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
