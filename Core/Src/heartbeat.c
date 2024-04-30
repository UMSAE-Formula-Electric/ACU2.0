#include "stm32f4xx.h"
#include "heartbeat.h"
#include "vcu_comms_handler.h"
#include "logger.h"
#include "freertos_task_handles.h"
#include "iwdg.h"

#define HEARTBEAT_TASK_DELAY_MS     50
#define HEARTBEAT_MAX_MISSES		20 //Max number of times we can miss a heartbeat notification

heatbeat_state_t vcu_connection_state;

/*
 * heartbeat_master_task
 *
 * @Brief: This task is used to check if the VCU is reachable as well as send
 * heartbeat messages to the VCU
 */
void StartVcuHrtBeatTask(void *argument){
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadTerminate(osThreadGetId());
    }

	BaseType_t retRTOS;
	uint32_t ulNotifiedValue = 0;
	vcu_connection_state = HEARTBEAT_HAS_VCU;
	uint8_t misses = 0; //indicates how many cycles we have gone without detecting ACB

	for(;;){
        kickWatchdogBit(osThreadGetId());

		//send heartbeat message to ACB
		send_VCU_mesg(CAN_HEARTBEAT_RESPONSE);

		//Check if ACB has sent a message
		retRTOS = xTaskNotifyWait(0x00,0x00, &ulNotifiedValue, 0);

		//check if the ACB responded
		if(retRTOS == pdTRUE && ulNotifiedValue == HEARTBEAT_REQUEST_NOTIFY){
			//received notification from ACB

			//reset misses counter
			misses = 0;

			//check if we have previously lost the ACB
			if(vcu_connection_state == HEARTBEAT_LOST_VCU){
			  logMessage("Heartbeat: VCU re-connection", true);
			}
			else if(vcu_connection_state == HEARTBEAT_NO_VCU){
				//connecting with acb for first time
			  logMessage("Heartbeat: Made connection with VCU", true);
			}

			//set state
			vcu_connection_state = HEARTBEAT_HAS_VCU;
		}
		else{
			//did not receive notification from ACB
			if(vcu_connection_state == HEARTBEAT_HAS_VCU){

				if(misses < HEARTBEAT_MAX_MISSES){
					//losing ACB
					misses++;

					//just for safety
					vcu_connection_state = HEARTBEAT_HAS_VCU;
				}
				else{
					//lost ACB
					vcu_connection_state = HEARTBEAT_LOST_VCU;
					logMessage("Heartbeat: Lost Connection with VCU", true);
				}
			}
			else if(vcu_connection_state == HEARTBEAT_NO_VCU ){
				if(misses < HEARTBEAT_MAX_MISSES){
					//losing ACB
					misses++;

					//just for safety
					vcu_connection_state = HEARTBEAT_NO_VCU;
				}
				else{
					//lost ACB
					vcu_connection_state = HEARTBEAT_NO_VCU;
					logMessage("Heartbeat: Could not connect with VCU", true);
				}
			}
			else{
				//acb_connection_state = HEARTBEAT_LOST_VCU;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_TASK_DELAY_MS));
	}
}


/*
 * get_hearbeat_State
 *
 * @Brief: This method is used to get the current state of heartbeat
 */
heatbeat_state_t get_heartbeat_state(){
	return vcu_connection_state;
}

/*
 * heartbeat_get_task
 *
 * @Brief: Gets the handle for the heartbeat task
 */
TaskHandle_t heartbeat_get_task(){
	return vcuHrtBeatTaskHandle;
}


