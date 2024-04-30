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
#define DISABLE_MC_HEARTBEAT_CHECK 0
#define DISABLE_AIRS_CHECK 0
#define DISABLE_PRECHARGE_CHECK 0
#define DISABLE_TSA_CHECKS 0

////////////////
//Error Handlers
void tsa_check_loop_fail_handler();
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

	//mc_data_request_init();

#ifdef TEST_BUZZER
  sound_buzzer_speaker_jenk();
#endif

	for(;;){
        kickWatchdogBit(osThreadGetId());

        state = get_car_state();
        switch(state){
            case IDLE:
                //wait for button press
                retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);
                led_clear_1();
                if(retRTOS == pdTRUE && ulNotifiedValue == TSA_BUTTON_PRESS){
                    go_tsa();
                }
                //check the status of the safety loop and send it to VCU
                //so that the VCU can inform the driver
                if(!DISABLE_SAFETY_LOOP_CHECK) {
                    if(check_safety_loop() != SAFETY_LOOP_CLOSED){
                        send_VCU_mesg(CAN_NO_SAFETY_LOOP_SET);
                    }
                    else{
                        send_VCU_mesg(CAN_NO_SAFETY_LOOP_CLEAR);
                    }
                }
                break;

            case TRACTIVE_SYSTEM_ACTIVE:
                retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);
                led_set_1_blue();
                if(retRTOS == pdPASS){
                    if(ulNotifiedValue == RTD_BUTTON_PRESS){
                        go_rtd();
                    }
                    else if(ulNotifiedValue == TSA_BUTTON_PRESS || ulNotifiedValue == KILL_SWITCH_PRESS ){
                        go_idle();
                    }
                    else{
                      logMessage("Unexpected State Change, TSA", false);
                      go_idle();
                    }
                }
                else{
                    //check safety loop state
                    if(!DISABLE_SAFETY_LOOP_CHECK){
                        saftey_loop = check_safety_loop();
                        if(saftey_loop != SAFETY_LOOP_CLOSED){
                            vTaskDelay(pdMS_TO_TICKS(500));
                            saftey_loop = check_safety_loop();
                            if(saftey_loop != SAFETY_LOOP_CLOSED){
                                log_and_handle_error(ERROR_ACB_SAFETY_LOOP_OPEN, &go_idle);
                            }
                        }
                    }
                }

                if(get_heartbeat_state() != HEARTBEAT_HAS_VCU) {
                    go_idle();
                }

                //Ready to Drive Procedure
                break;

            case READY_TO_DRIVE:
                retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);
                led_set_1_green();
                if (retRTOS == pdPASS){
                    if(ulNotifiedValue == RTD_BUTTON_PRESS ||ulNotifiedValue == TSA_BUTTON_PRESS || ulNotifiedValue == KILL_SWITCH_PRESS){
                        go_idle();
                    }
                    else{
                      logMessage("Bad runtime change request", false);
                    }
                }
                else{
                    //check if saftey loop has opened
                    if(!DISABLE_SAFETY_LOOP_CHECK) {
                        saftey_loop = check_safety_loop();
                        if(saftey_loop != SAFETY_LOOP_CLOSED){
                            vTaskDelay(pdMS_TO_TICKS(500));
                            saftey_loop = check_safety_loop();
                            if(saftey_loop != SAFETY_LOOP_CLOSED){
                                log_and_handle_error(ERROR_ACB_SAFETY_LOOP_OPEN, &go_idle);
                            }
                        }
                    }
                }

                if(get_heartbeat_state() != HEARTBEAT_HAS_VCU) {
                    go_idle();
                }

                break;
            default:
                break;
        }
		vTaskDelay(pdMS_TO_TICKS(100));
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
#ifndef MC_STARTUP_ENABLE
	shutdown_MC();
#endif
	//open airs
	air_state = open_airs();
	if(air_state != 0){
		 log_and_handle_error(ERROR_AIR_WELD, &air_weld_handler);
	}
	cooling_disable_pump();
	cooling_disable_rad_fans();
	led_set_2_green();
}

//function to close airs, power mc
void go_tsa(){
	HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port, ACU_LED2_BLUE_Pin, GPIO_PIN_RESET);
	//Tractive System Active Procedure

	if(DISABLE_TSA_CHECKS) {
		led_set_2_red();
		if(!DISABLE_PRECHARGE_CHECK) {
			startup_precharge();
		}
		close_airs();
		send_VCU_mesg(CAN_ACB_TSA_ACK);
		set_car_state(TRACTIVE_SYSTEM_ACTIVE);

		return;
	}

	if(check_safety_loop() == SAFETY_LOOP_CLOSED || DISABLE_SAFETY_LOOP_CHECK){  //check if safety loop is closed is Pressed

		if(startup_precharge() || DISABLE_PRECHARGE_CHECK){
			if(!DISABLE_AIRS_CHECK) {
				if(close_airs()){
					log_and_handle_error(ERROR_AIR_FAIL_TO_CLOSE, &air_close_fail_handler);
				}
				else{
					set_car_state(TRACTIVE_SYSTEM_ACTIVE);
					send_VCU_mesg(CAN_ACB_TSA_ACK);
				}
			} else {
				set_car_state(TRACTIVE_SYSTEM_ACTIVE);
				send_VCU_mesg(CAN_ACB_TSA_ACK);
			}
		}
		else{
			log_and_handle_error(ERROR_PRECHAGE_FAIL, &go_idle);
		}
		open_precharge();
	}

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
	HAL_GPIO_WritePin(ACU_LED2_BLUE_GPIO_Port, ACU_LED2_BLUE_Pin, GPIO_PIN_SET);
	sound_buzzer();
	set_car_state(READY_TO_DRIVE);
	send_VCU_mesg(CAN_ACB_RTD_ACK);
	enableCoolingGently();
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
