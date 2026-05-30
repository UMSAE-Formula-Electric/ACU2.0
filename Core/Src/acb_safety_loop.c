#include "acb_safety_loop.h"
#include "gpio.h"

/**
  * @brief  Starts the can message receive and processing task
  * @retval SAFTEY_LOOP_OPEN if loop is open, SAFTEY_LOOP_CLOSED if loop is closed)
  */
loop_status_t check_safety_loop(){

	// this is to keep track of the last known valid state
	static loop_status_t last_valid_state = SAFETY_LOOP_OPEN;

	// sampling twice with a 15ms delay
	GPIO_PinState sample1 = HAL_GPIO_ReadPin(SHCT_TAP_GPIO_Port, SHCT_TAP_Pin);
	HAL_Delay(15);
	GPIO_PinState sample2 = HAL_GPIO_ReadPin(SHCT_TAP_GPIO_Port, SHCT_TAP_Pin);


	// If both samples match update the state,
	// if not matching, means a bounce occurred
	if (sample1 == sample2) {
	        if (sample1 == GPIO_PIN_SET) {
	            last_valid_state = SAFETY_LOOP_CLOSED;
	        } else {
	            last_valid_state = SAFETY_LOOP_OPEN;
	        }
	    }

	return last_valid_state;
}
