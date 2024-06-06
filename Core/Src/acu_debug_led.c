#include <acu_debug_led.h>
#include "stm32f4xx.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"


volatile enum LED_STATE state;
int blockerLoopCount = 0;

void StartDebugLEDTask(void* argument)	{
	uint8_t isTaskActivated = (int)argument;
	if (isTaskActivated == 0) {
		osThreadExit();
	}

	for(;;) {
		switch(state) {
		case IDLE_LED:
			led_set_1_red();
			led_clear_2();
			break;
		case TSA_REQUEST:
			led_set_1_blue();
			led_clear_2();
			break;
		case PRECHARGE:
			led_set_1_white();
			led_clear_2();
			break;
		case TSA:
			led_set_1_green();
			led_set_2_red();
			break;
		case RTD_REQUEST:
			led_set_1_green();
			led_set_2_blue();
			break;
		case BUZZER:
			led_set_1_green();
			led_set_2_blue();
			break;
		case ENABLE_COOLING:
			led_set_1_green();
			led_set_2_purple();
			break;
		case RTD:
            led_set_1_green();
            led_set_2_green();
			break;
		case FAIL:
			for(int i = 0; i < FAIL_COUNT; i++) {
				led_set_1_red();
				led_set_2_red();
				osDelay(pdMS_TO_TICKS(FAIL_TIME));
				led_set_1_white();
				led_set_2_white();
				osDelay(pdMS_TO_TICKS(FAIL_TIME));
				blockerLoopCount--;
			}
			break;
		case SAFETY_LOOP_OPEN_LED:
        	led_clear_all_leds();
        	led_set_1_red();
        	osDelay(pdMS_TO_TICKS(SAFETY_LOOP_OSC_TIME + TASK_DELAY));
        	led_clear_all_leds();
        	led_set_2_red();
        	osDelay(pdMS_TO_TICKS(SAFETY_LOOP_OSC_TIME));
        	led_clear_all_leds();
			break;
		case NO_VCU_HEARTBEAT:
			led_set_1_red();
			osDelay(pdMS_TO_TICKS(NO_HEARTBEAT_TIME + TASK_DELAY));
        	led_clear_all_leds();
        	led_set_2_blue();
        	osDelay(pdMS_TO_TICKS(NO_HEARTBEAT_TIME));
        	led_clear_all_leds();
			break;
		case NO_MC_HEARTBEAT:
			break;
		case VCU_IDLE_REQUEST:
			for(int i = 0; i < FAIL_COUNT; i++) {
				led_set_1_red();
				led_set_2_white();
				osDelay(pdMS_TO_TICKS(FAIL_TIME));
				led_set_1_white();
				led_set_2_red();
				osDelay(pdMS_TO_TICKS(FAIL_TIME));
			}
			break;
		}
		osDelay(pdMS_TO_TICKS(50));
	}
}

void setLEDState(enum LED_STATE newState) {
	state = newState;
}

void led_clear_all_leds(){
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_RESET);
}

void led_set_all_leds(){
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_SET);
}

void led_set_1_red() {
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_RESET);
}

void led_set_1_blue() {
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_SET);
}

void led_set_1_green() {
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_RESET);
}

void led_set_1_purple() {
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_RESET);
}

void led_set_1_white() {
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_SET);
}

void led_clear_1() {
    HAL_GPIO_WritePin(ACU_LED1_RED_GPIO_Port,ACU_LED1_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_GREEN_GPIO_Port,ACU_LED1_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED1_BLUE_GPIO_Port,ACU_LED1_BLUE_Pin, GPIO_PIN_RESET);
}

void led_set_2_blue() {
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_RESET);
}

void led_set_2_green() {
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_RESET);
}

void led_set_2_red() {
    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_RESET);
}

void led_set_2_purple() {
    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_RESET);
}

void led_set_2_white() {
    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_SET);
}

void led_clear_2() {
    HAL_GPIO_WritePin(ACU_LED2_RED_GPIO_Port,ACU_LED2_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_GREEN_GPIO_Port,ACU_LED2_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port,ACU_LED2_BLUE_Pin, GPIO_PIN_RESET);
}



