/*
 * startup_condition.c
 *
 *  Created on: Dec 27, 2018
 *      Author: Martin Rickey
 */

#include "startup_condition.h"
#include "logger.h"
#include "iwdg.h"
#include "acu_debug_led.h"

/*
 * checkStartupCondition
 *
 * @Brief This method checks under what condition the processor started and logs it
 * @Note: This function makes logs to the sd card so sdInitialize should already have been called
 */
void checkStartupCondition(){
	//check startup condition
	if(startFromIWDG()){
		logMessage("IWDG reset", false);
        led_set_1_purple();
	}
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET){//check for power on reset
		logMessage("Power on reset", false);
	}
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET){
		logMessage("Pin reset", false);
	}
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET){
		logMessage("Brown out reset", false);
	}
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET){
		logMessage("Software reset", false);
	}
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET){
		logMessage("Low power reset", false);
	}
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET){
		logMessage("Window watchdog reset", false);
	}
	else{
		logMessage("Unknown startup condition", false);
	}

	/* Clear reset flags */
    __HAL_RCC_CLEAR_RESET_FLAGS();
}
