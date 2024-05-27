#include <acu_debug_led.h>
#include "FreeRTOS.h"
#include "task.h"

#include "heartbeat.h"

#include "vcu_comms_handler.h"
#include "can.h"
#include "car_state.h"
#include "acb_startup.h"
#include "gpio.h"

#include "error_handler.h"

#include "air.h"
#include "acb_safety_loop.h"
#include "buzzer.h"
#include "cooling.h"
#include "global_board_config.h"

#include "logger.h"
#include "freertos_task_handles.h"
#include "iwdg.h"

#define HOON_MODE 0

#define DISABLE_SAFETY_LOOP_CHECK 0
#define DISABLE_MC_HEARTBEAT_CHECK 1
#define DISABLE_AIRS_CHECK 0
#define DISABLE_PRECHARGE_CHECK 1
#define DISABLE_TSA_CHECKS 0

////////////////
//Error Handlers
void tsa_check_loop_fail_handler();
loop_status_t checkSafetyLoop();
////////////////

/**
  * @brief  Returns the handle for the ACB task
  * @retval
  */
TaskHandle_t get_startup_task(){
	return acuStateTaskHandle;
}

/**
  * @brief  handle the startup routine then do nothing?
  * @retval never return from a freeRTOS task, kills task if infinite task ends
  */
void StartAcuStateTask(void *argument){
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadTerminate(osThreadGetId());
    }

	//Keep user led on to simulate LV key is on
	enum CAR_STATE state;
	BaseType_t retRTOS = 0;
	uint32_t ulNotifiedValue;
	loop_status_t saftey_loop = SAFETY_LOOP_CLOSED;


	for(;;){
        kickWatchdogBit(osThreadGetId());

        state = get_car_state();
        switch(state){
            case IDLE:
            	led_set_1_red();
                //wait for button press
                retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);
                if(retRTOS == pdTRUE && ulNotifiedValue == TSA_BUTTON_PRESS){
                    go_tsa();
                }
//                else if(retRTOS == pdTRUE && (ulNotifiedValue == RTD_BUTTON_PRESS) { // || ulNotifiedValue == KILL_SWITCH_PRESS ){
//                    go_idle();
//                }

                break;
            case TRACTIVE_SYSTEM_ACTIVE:
                led_set_1_green();
                led_set_2_red();

                retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);
                if(retRTOS == pdPASS){
                    if(ulNotifiedValue == RTD_BUTTON_PRESS){
                        go_rtd();
                    }
                    else if(ulNotifiedValue == KILL_SWITCH_PRESS) { // || ulNotifiedValue == KILL_SWITCH_PRESS ){
                        go_idle();
                    }
                }
                break;
            case READY_TO_DRIVE:
                led_set_1_green();
                led_set_2_green();

                retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);
                if (retRTOS == pdPASS){
                    if(ulNotifiedValue == KILL_SWITCH_PRESS){
                        go_idle();
                    }
                    else{
                      logMessage("Bad runtime change request", false);
                    }
                }
                break;
            default:
                break;
        }
		//vTaskDelay(pdMS_TO_TICKS(25));

        if(get_heartbeat_state() != HEARTBEAT_PRESENT) {
        	led_clear_all_leds();
        	led_set_2_blue();
        	go_idle();
        	led_clear_all_leds();
        }

        if(checkSafetyLoop() != SAFETY_LOOP_CLOSED) {
        	led_clear_all_leds();
        	led_set_2_red();
        	go_idle();
        	led_clear_all_leds();
        }

        osThreadYield();

	}
	vTaskDelete( NULL );
}

//function to open airs, disable mc, notify VCU of state change
void go_idle(){
	uint8_t air_state = 0;
	//notify VCU of state change
	set_car_state(IDLE);
	send_VCU_mesg(CAN_GO_IDLE_REQ);
	//disable mc

	//open airs
	open_precharge();
	air_state = open_airs();
	if(air_state != 0){
		 log_and_handle_error(ERROR_AIR_WELD, &air_weld_handler);
	}

	cooling_disable_pump();
	cooling_disable_rad_fans();
	led_clear_all_leds();
}

//function to close airs, power mc
void go_tsa(){
	led_set_1_blue();
	//Tractive System Active Procedure
	if(check_safety_loop() == SAFETY_LOOP_CLOSED || DISABLE_SAFETY_LOOP_CHECK){  //check if safety loop is closed is Pressed
		if(startup_precharge() == PRECHARGE_SUCCESS || DISABLE_PRECHARGE_CHECK){
			if(close_airs() == AIR_CLOSE_SUCCESS || DISABLE_AIRS_CHECK){
					set_car_state(TRACTIVE_SYSTEM_ACTIVE);
					send_VCU_mesg(CAN_ACB_TSA_ACK);
				} else{
					log_and_handle_error(ERROR_AIR_FAIL_TO_CLOSE, &air_close_fail_handler);
					go_idle();
				}
			}
		}
		else{
			log_and_handle_error(ERROR_PRECHAGE_FAIL, &go_idle);
		}
		open_precharge();

//		if(get_car_state() != TRACTIVE_SYSTEM_ACTIVE) {
//			go_idle();
//		}
	}


loop_status_t checkSafetyLoop() {
	loop_status_t safety_loop = SAFETY_LOOP_CLOSED;
	if(!DISABLE_SAFETY_LOOP_CHECK) {
		safety_loop = check_safety_loop();
		if(safety_loop != SAFETY_LOOP_CLOSED) {
			vTaskDelay(pdMS_TO_TICKS(50));
			safety_loop = check_safety_loop();
		}
	}
	return safety_loop;
}

/*
 * go_rtd
 *
 * @Brief: This function is used to bring the car into the ready to drive
 * state.
 *
 * TODO: Disable the automatic enabling of the cooling system and make
 * it work off the IGBT temperatures.
 */
void go_rtd(){
	led_set_2_blue();
	sound_buzzer();
	set_car_state(READY_TO_DRIVE);
	led_set_2_purple();
	enableCoolingGently();
	send_VCU_mesg(CAN_ACB_RTD_ACK);
}

/*
 * tsa_check_loop_fail_handler
 *
 * @Brief: This is probably supposed to be used with the error handling
 * functionality. However it has not been used yet.
 */
void tsa_check_loop_fail_handler(){
	send_VCU_mesg(CAN_ACB_TSA_NACK);
}
