#include "heartbeat.h"
#include "vcu_comms_handler.h"
#include "logger.h"
#include "freertos_task_handles.h"
#include "iwdg.h"

#define HEARTBEAT_TASK_DELAY_MS     50
#define HEARTBEAT_MAX_MISSES		10 //Max number of times we can miss a heartbeat notification

static HeartbeatState_t vcu_connection_state = HEARTBEAT_NONE;

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
    HeartbeatNotify_t vcuNotification = 0;
	uint8_t misses = 0; //indicates how many cycles we have gone without detecting ACB

	for(;;){
        kickWatchdogBit(osThreadGetId());

		//send heartbeat message to ACB
		send_VCU_mesg(CAN_HEARTBEAT_RESPONSE);

		//Check if ACB has sent a message
		retRTOS = xTaskNotifyWait(0x00, 0x00, (uint32_t*) &vcuNotification, pdMS_TO_TICKS(HEARTBEAT_TASK_DELAY_MS));

		//check if the ACB responded
		if(retRTOS == pdTRUE && vcuNotification == HEARTBEAT_REQUEST_NOTIFY){
            // Received notification from ACU
            misses = 0; // Reset misses counter
            logMessage(vcu_connection_state == HEARTBEAT_LOST ? "Heartbeat: VCU re-connection\r\n" : "Heartbeat: Heartbeat received from the VCU\r\n", true);
            vcu_connection_state = HEARTBEAT_PRESENT; // Set state
		}
		else{
            // Did not receive notification from ACU
            if(++misses > HEARTBEAT_MAX_MISSES){
                // Lost ACU
                logMessage(vcu_connection_state == HEARTBEAT_PRESENT ? "Heartbeat: Lost Connection with VCU\r\n" : "Heartbeat: Could not connect with VCU\r\n", true);
                vcu_connection_state = HEARTBEAT_LOST;
            }
		}

		osThreadYield();
	}
}


/*
 * get_hearbeat_State
 *
 * @Brief: This method is used to get the current state of heartbeat
 */
HeartbeatState_t get_heartbeat_state(){
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


