#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "vcu_comms_handler.h"
#include "can.h"
#include "car_state.h"
#include "acb_startup.h"
#include "heartbeat.h"
#include "can_utils.h"
#include "iwdg.h"

#define VCU_COMMS_TASK_DELAY_MS 20
extern QueueHandle_t ACB_VCU_CAN_Queue;

void notify_heartbeat_task(HeartbeatNotify_t notify_val);

void processVcuToAcuCanIdRxData(const uint8_t *RxData) {
    switch(RxData[0]){
        case CAN_HEARTBEAT_REQUEST:
            notify_heartbeat_task(HEARTBEAT_REQUEST_NOTIFY);
            break;

        case CAN_BATTERY_VOLTAGE_REQUEST:
            break;
    }
}

void processVcuSetAcuStateCanIdRxData(const uint8_t *RxData) {
    TaskHandle_t startupTask = NULL;
    osStatus_t retOS;
    enum DASH_BUTTON dashButtonPress;

    switch(RxData[0]){
        case TRACTIVE_SYSTEM_ACTIVE:
        	dashButtonPress = TSA_BUTTON_PRESS;
            break;
        case READY_TO_DRIVE:
        	dashButtonPress = RTD_BUTTON_PRESS;
            break;
        case IDLE:
        	dashButtonPress = KILL_SWITCH_PRESS;
            break;
        default:
            break;
    }
    retOS = osMessageQueuePut(setCarStateQueueHandle, &dashButtonPress , 0, 0);
}

void process_VCU_CAN_packets(void *argument){
    CAN_RxHeaderTypeDef packetToProcess;
    uint8_t RxData[8];
    BaseType_t ret;

	for(;;){

		ret = xQueueReceive(ACB_VCU_CAN_Queue,&packetToProcess, portMAX_DELAY);
		if(ret == pdPASS){
			if(packetToProcess.StdId ==  CAN_VCU_SET_ACB_STATE_ID){
                processVcuSetAcuStateCanIdRxData(RxData);
            }
            else if(packetToProcess.StdId == CAN_ACU_CAN_ID){
                processVcuToAcuCanIdRxData(RxData);
            }
		}
	}
	vTaskDelete(NULL);
}


void send_VCU_mesg(enum STARTUP_STATUS_NOTIFY_MSG msg){
	uint8_t data = (uint8_t)msg;
	sendCan(&hcan1, &data, 1, CAN_ACU_TO_VCU_ID, CAN_RTR_DATA, CAN_NO_EXT);
}

void notify_heartbeat_task(HeartbeatNotify_t notify_val){
	TaskHandle_t task = NULL;
	task = heartbeat_get_task();
	if(task != NULL){
		xTaskNotify( task, notify_val, eSetValueWithOverwrite);
	}
}

void StartVcuCanCommsTask(void *argument){
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadExit();
    }

    for(;;){
        kickWatchdogBit(osThreadGetId());

        osDelay(pdMS_TO_TICKS(VCU_COMMS_TASK_DELAY_MS));
    }
}
