#include <mc_power.h>
#include "FreeRTOS.h"
#include "task.h"

#include "air.h"
#include "motor_controller_can_utils.h"
#include "gpio.h"
#include "vcu_comms_handler.h"
#include "acb_startup.h"
#include "can.h"

#define AIR_CLOSE_DELAY 500
#define AIR_OPEN_DELAY  500

#define DISABLE_AIRS_CHECK 0
#define DISABLE_PRECHARGE_CHECK 0

void setAirCtrl() {
    HAL_GPIO_WritePin(AIR_POS_CTRL_GPIO_Port, AIR_POS_CTRL_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AIR_NEG_CTRL_GPIO_Port, AIR_NEG_CTRL_Pin, GPIO_PIN_SET);
}

void resetAirCtrl() {
    HAL_GPIO_WritePin(AIR_POS_CTRL_GPIO_Port, AIR_POS_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_NEG_CTRL_GPIO_Port, AIR_NEG_CTRL_Pin, GPIO_PIN_RESET);
}

/**
  * @brief Closes the airs, check feedback to confirm state
  * @retval 0 on success, 1 if pos air didn't close, 2 if neg air didnt close, 3 if both airs didn't close
  */
air_status_t close_airs(){
	air_status_t air_state = AIR_CLOSE_SUCCESS;
    setAirCtrl();
	vTaskDelay(pdMS_TO_TICKS(AIR_CLOSE_DELAY));

	if(!DISABLE_AIRS_CHECK) {
		if(HAL_GPIO_ReadPin(AIR_POS_FB_GPIO_Port, AIR_POS_FB_Pin) != 1){
			//neg air did not close
			air_state = air_state | AIR_POS_CLOSE_FAILURE;
            resetAirCtrl();
        }
		if(HAL_GPIO_ReadPin(AIR_NEG_FB_GPIO_Port, AIR_NEG_FB_Pin) != 1){
			//pos air did not close
			air_state = air_state | AIR_NEG_CLOSE_FAILURE;
            resetAirCtrl();
		}
	}
	return air_state;
}


/**
  * @brief Opens the airs, check feedback to confirm state
  * @retval 0 on success, 1 if pos air didn't open, 2 if neg air didnt open, 3 if both airs didn't open
  */
uint8_t open_airs(){
	uint8_t air_state = 0;
    resetAirCtrl();
#ifndef TESTAIRS
	vTaskDelay(pdMS_TO_TICKS(AIR_OPEN_DELAY));
#endif
	if(HAL_GPIO_ReadPin(AIR_POS_FB_GPIO_Port, AIR_POS_FB_Pin) != 0){
		//neg air did not open
		air_state = air_state | 0x1;
        resetAirCtrl();
	}
	if(HAL_GPIO_ReadPin(AIR_NEG_FB_GPIO_Port, AIR_NEG_FB_Pin) != 0){
		//pos air did not open
		air_state = air_state | 0x2;
        resetAirCtrl();
	}
	return air_state;
}

/*
 * startup_precharge
 *
 * @Brief: This function executes the pre charge routine. This function must be called
 * from within a task. This function will check to see if the
 */

precharge_status_t startup_precharge(){
	precharge_status_t precharge_success = PRECHARGE_FAILED;

	HAL_GPIO_WritePin(AIR_NEG_CTRL_GPIO_Port, AIR_NEG_CTRL_Pin, GPIO_PIN_SET);
	vTaskDelay(pdMS_TO_TICKS(AIR_CLOSE_DELAY));

	if(!DISABLE_AIRS_CHECK) {
		if(HAL_GPIO_ReadPin(AIR_NEG_FB_GPIO_Port, AIR_NEG_FB_Pin) != 1){
			resetAirCtrl();
			return precharge_success;
		}
	}

	HAL_GPIO_WritePin(PRECHRG_CTRL_GPIO_Port, PRECHRG_CTRL_Pin, GPIO_PIN_SET);
	led_set_1_white();
	vTaskDelay(pdMS_TO_TICKS(5000));

	if(isMCBusCharged() == BUS_CHARGED || DISABLE_PRECHARGE_CHECK){
		HAL_GPIO_WritePin(AIR_POS_CTRL_GPIO_Port, AIR_POS_CTRL_Pin, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(AIR_CLOSE_DELAY));
		precharge_success = PRECHARGE_SUCCESS;
	}
	HAL_GPIO_WritePin(PRECHRG_CTRL_GPIO_Port, PRECHRG_CTRL_Pin, GPIO_PIN_RESET);
	return precharge_success;

}

void open_precharge(){
	HAL_GPIO_WritePin(PRECHRG_CTRL_GPIO_Port, PRECHRG_CTRL_Pin, GPIO_PIN_RESET);
}


void air_weld_handler(){
	send_VCU_mesg(CAN_AIR_WELD_SET);
	shutdown_MC();
	go_idle();
	//send message to other board using error function
	//send_air_fail();
	//set non-volitile storage
	//die
}

void air_close_fail_handler(){
	//send_precharge_fail(); relace with send error function
	//send_air_fail(); relace with send error function
	go_idle();
}

uint8_t unchecked_precharge(){
	uint8_t precharge_success = 1;
	HAL_GPIO_WritePin(AIR_NEG_CTRL_GPIO_Port, AIR_NEG_CTRL_Pin, GPIO_PIN_SET);
	vTaskDelay(pdMS_TO_TICKS(AIR_CLOSE_DELAY));
	if(HAL_GPIO_ReadPin(AIR_NEG_FB_GPIO_Port, AIR_NEG_FB_Pin) != 1){
		//pos air did not open
		resetAirCtrl();
		precharge_success = 0;
	}
	HAL_GPIO_WritePin(PRECHRG_CTRL_GPIO_Port, PRECHRG_CTRL_Pin, GPIO_PIN_SET);
	vTaskDelay(pdMS_TO_TICKS(5000));
	return precharge_success;

}

void unchecked_air_close(){
	if(unchecked_precharge()){
		close_airs();
	}
	open_precharge();
}
