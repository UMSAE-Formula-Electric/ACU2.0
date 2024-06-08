#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "vcu_comms_handler.h"
#include "can.h"
#include "car_state.h"
#include "acb_startup.h"
#include "heartbeat.h"
#include "iwdg.h"
#include "motor_controller_can_utils.h"

#define VCU_COMMS_TASK_DELAY_MS 5

void notify_heartbeat_task(uint32_t notify_val);

_Bool isVcuCanId(uint16_t canId){
    return (canId == CAN_VCU_TO_ACU_ID || canId == CAN_VCU_SET_ACB_STATE_ID
        || canId == CAN_MC_RX_INTERNAL_VOLTAGES || canId == CAN_MC_RX_VOLT_ID);
}

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
    osMessageQueuePut(setCarStateQueueHandle, &dashButtonPress , 0, 0);
}

void send_VCU_mesg(enum STARTUP_STATUS_NOTIFY_MSG msg){
	uint8_t data = (uint8_t)msg;
	sendCan(&hcan1, &data, 1, CAN_ACU_TO_VCU_ID, CAN_RTR_DATA, CAN_NO_EXT);
}

void notify_heartbeat_task(uint32_t notify_val){
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

    CAN_RxPacketTypeDef rxPacket;
    osStatus_t isMsgTakenFromQueue;
    uint32_t canID;

    for(;;){
        kickWatchdogBit(osThreadGetId());

        isMsgTakenFromQueue = osMessageQueueGet(vcuCanCommsQueueHandle, &rxPacket, 0, 0);
        if (isMsgTakenFromQueue == osOK) {
            if (rxPacket.rxPacketHeader.IDE == CAN_ID_STD)
            {
                canID = rxPacket.rxPacketHeader.StdId;
                if (canID == CAN_VCU_TO_ACU_ID)
                {
                    processVcuToAcuCanIdRxData(rxPacket.rxPacketData);
                }
                else if (canID == CAN_VCU_SET_ACB_STATE_ID)
                {
                    processVcuSetAcuStateCanIdRxData(rxPacket.rxPacketData);
                }
                else if (canID == CAN_MC_RX_INTERNAL_VOLTAGES)
                {
                    mc_process_internal_volt_can(rxPacket.rxPacketData);
                }
                else if (canID == CAN_MC_RX_VOLT_ID)
                {
                    mc_process_volt_can(rxPacket.rxPacketData);
                }
                }
        }

        osDelay(pdMS_TO_TICKS(VCU_COMMS_TASK_DELAY_MS));
    }
}
