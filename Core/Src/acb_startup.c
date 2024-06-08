#include <acu_debug_led.h>
#include "FreeRTOS.h"
#include "task.h"

#include "heartbeat.h"

#include "vcu_comms_handler.h"
#include "can.h"
#include "car_state.h"
#include "acb_startup.h"
#include "gpio.h"

#include "acu_debug_led.h"

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
#define DISABLE_MC_HEARTBEAT_CHECK 0
#define DISABLE_HEARTBEAT_CHECK 0
#define DISABLE_AIRS_CHECK 0
#define DISABLE_PRECHARGE_CHECK 0
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
        osThreadExit();
    }

	//Keep user led on to simulate LV key is on
	enum CAR_STATE state;
	osStatus_t retRTOS = 0;
	uint8_t ulNotifiedValue;
	loop_status_t saftey_loop = SAFETY_LOOP_CLOSED;


	for(;;){
        kickWatchdogBit(osThreadGetId());

        state = get_car_state();
        switch(state){
            case IDLE:
            	resetAirCtrl();
            	open_precharge();
            	setLEDState(IDLE_LED);
                //wait for button press
                retRTOS = osMessageQueueGet(setCarStateQueueHandle, &ulNotifiedValue, 0, 0);
                if(retRTOS == osOK){
                	if(ulNotifiedValue == TSA_BUTTON_PRESS){
                		go_tsa();
                	}
                }
//                else if(retRTOS == pdTRUE && (ulNotifiedValue == RTD_BUTTON_PRESS) { // || ulNotifiedValue == KILL_SWITCH_PRESS ){
//                    go_idle();
//                }

                break;
            case TRACTIVE_SYSTEM_ACTIVE:
                setLEDState(TSA);

                retRTOS = osMessageQueueGet(setCarStateQueueHandle, &ulNotifiedValue, 0, 0);
                if(retRTOS == osOK){
                    if(ulNotifiedValue == RTD_BUTTON_PRESS){
                         go_rtd();
                    }
                    else if(ulNotifiedValue == KILL_SWITCH_PRESS) { // || ulNotifiedValue == KILL_SWITCH_PRESS ){
                    	setLEDState(VCU_IDLE_REQUEST);
                        go_idle();
                    }
                }
                break;
            case READY_TO_DRIVE:
            	setLEDState(RTD);

                retRTOS = osMessageQueueGet(setCarStateQueueHandle, &ulNotifiedValue, 0, 0);
                if (retRTOS == osOK){
                    if(ulNotifiedValue == KILL_SWITCH_PRESS){
                    	setLEDState(VCU_IDLE_REQUEST);
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
        if(get_heartbeat_state() != HEARTBEAT_PRESENT && !DISABLE_HEARTBEAT_CHECK) {
        	go_idle();
        	setLEDState(NO_VCU_HEARTBEAT);
        }

        if(checkSafetyLoop() != SAFETY_LOOP_CLOSED) {
        	go_idle();
        	setLEDState(SAFETY_LOOP_OPEN_LED);
        }
        vTaskDelay(pdMS_TO_TICKS(25));
	}
	vTaskDelete( NULL );
}

//function to open airs, disable mc, notify VCU of state change
void go_idle(){
	uint8_t air_state = 0;
	//notify VCU of state change

	//if(get_car_state() != IDLE) {
	send_VCU_mesg(CAN_GO_IDLE_REQ);
	//}
	//disable mc

	//open airs
	if(get_car_state() != IDLE || 1) {
		open_precharge();
		air_state = open_airs();
		if(air_state != 0){
			 log_and_handle_error(ERROR_AIR_WELD, &air_weld_handler);
		}

		cooling_disable_pump();
		cooling_disable_rad_fans();
	}
	open_airs();
	set_car_state(IDLE);
}

//function to close airs, power mc
void go_tsa(){
	setLEDState(TSA_REQUEST);
	send_VCU_mesg(CAN_ACB_TSA_ACK);
	//Tractive System Active Procedure
	if(check_safety_loop() == SAFETY_LOOP_CLOSED || DISABLE_SAFETY_LOOP_CHECK){  //check if safety loop is closed is Pressed
		if(startup_precharge() == PRECHARGE_SUCCESS || DISABLE_PRECHARGE_CHECK){
			if(close_airs() == AIR_CLOSE_SUCCESS || DISABLE_AIRS_CHECK){
					set_car_state(TRACTIVE_SYSTEM_ACTIVE);
					send_VCU_mesg(CAN_ACB_TSA_ACK);
				} else{
					log_and_handle_error(ERROR_AIR_FAIL_TO_CLOSE, &air_close_fail_handler);
					setLEDState(FAIL);
					go_idle();
				}
			}
		}
		else{
			log_and_handle_error(ERROR_PRECHAGE_FAIL, &go_idle);
			go_idle();
			setLEDState(FAIL);
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
	setLEDState(RTD_REQUEST);
	setLEDState(BUZZER);
	sound_buzzer();
	setLEDState(ENABLE_COOLING);
	enableCoolingGently();
	set_car_state(READY_TO_DRIVE);
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
