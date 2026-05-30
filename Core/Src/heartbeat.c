#include "heartbeat.h"
#include "vcu_comms_handler.h"
#include "logger.h"
#include "freertos_task_handles.h"
#include "iwdg.h"
#include "semphr.h"

#define HEARTBEAT_TASK_DELAY_MS     100
#define HEARTBEAT_MAX_MISSES		10 //Max number of times we can miss a heartbeat notification

static HeartbeatState_t vcu_connection_state = HEARTBEAT_NONE;
static SemaphoreHandle_t heartbeatMutex = NULL;

void heartbeat_init(void){
	heartbeatMutex = xSemaphoreCreateMutex();
}

/*
 * heartbeat_master_task
 *
 * @Brief: This task is used to check if the VCU is reachable as well as send
 * heartbeat messages to the VCU
 */
void StartVcuHrtBeatTask(void *argument){
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadExit();
    }

	BaseType_t retRTOS;
    uint32_t vcuNotification = 0;
	uint8_t misses = 0; //indicates how many cycles we have gone without detecting ACB

	for(;;){
        kickWatchdogBit(osThreadGetId());

		//send heartbeat message to ACB
		send_VCU_mesg(CAN_HEARTBEAT_RESPONSE);

		//Check if ACB has sent a message
		retRTOS = xTaskNotifyWait(0x00, 0x00, &vcuNotification, pdMS_TO_TICKS(HEARTBEAT_TASK_DELAY_MS));

		//check if the ACB responded
		if(retRTOS == pdTRUE && vcuNotification == HEARTBEAT_REQUEST_NOTIFY){
		    misses = 0;

		    /*1 read + 1 write*/
		    HeartbeatState_t prev_state;
		    xSemaphoreTake(heartbeatMutex, portMAX_DELAY);
		    prev_state = vcu_connection_state;          // capture old
		    vcu_connection_state = HEARTBEAT_PRESENT;   // write new
		    xSemaphoreGive(heartbeatMutex);
		    /*lock released*/

		    logMessage(prev_state == HEARTBEAT_LOST
		                 ? "Heartbeat: VCU re-connection\r\n"
		                 : "Heartbeat: Heartbeat received from the VCU\r\n", true);

		    //TODO in the can send
		    /*
		    HeartbeatState_t snapshot;
			xSemaphoreTake(heartbeatMutex, portMAX_DELAY); //TAKE
			snapshot = vcu_connection_state; //WRITE
			xSemaphoreGive(heartbeatMutex); //GIVE

			send_VCU_state_over_can(snapshot);   // long steps are outside the lock
		     */
		}
		else {
		    if(++misses > HEARTBEAT_MAX_MISSES){
		        HeartbeatState_t prev_state;
		        xSemaphoreTake(heartbeatMutex, portMAX_DELAY);
		        prev_state = vcu_connection_state;
		        vcu_connection_state = HEARTBEAT_LOST;
		        xSemaphoreGive(heartbeatMutex);

		        logMessage(prev_state == HEARTBEAT_PRESENT
		                     ? "Heartbeat: Lost Connection with VCU\r\n"
		                     : "Heartbeat: Could not connect with VCU\r\n", true);
		    }
		}
		vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_TASK_DELAY_MS / 4));
	}
}


/*
 * get_hearbeat_State
 *
 * @Brief: This method is used to get the current state of heartbeat
 */
HeartbeatState_t get_heartbeat_state(){
	HeartbeatState_t state;
	xSemaphoreTake(heartbeatMutex, portMAX_DELAY);
	state = vcu_connection_state;
	xSemaphoreGive(heartbeatMutex);
	return state;
}

/*
 * heartbeat_get_task
 *
 * @Brief: Gets the handle for the heartbeat task
 */
TaskHandle_t heartbeat_get_task(){
	return vcuHrtBeatTaskHandle;
}


