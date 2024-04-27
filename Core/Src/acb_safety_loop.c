#include "acb_safety_loop.h"
#include "gpio.h"

/**
  * @brief  Starts the can message recieve and processing task
  * @retval SAFTEY_LOOP_OPEN if loop is open, SAFTEY_LOOP_CLOSED if loop is closed)
  */
loop_status_t check_safety_loop(){
	uint8_t loop_status = SAFETY_LOOP_OPEN;
	if(HAL_GPIO_ReadPin(SHCT_TAP_GPIO_Port, SHCT_TAP_Pin)){
		loop_status = SAFETY_LOOP_CLOSED;
	}
	return loop_status;
}
