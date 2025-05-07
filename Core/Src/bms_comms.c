//
// Created by caleb on 2024-06-02.
//

#include "bms_comms.h"

#include <stdint.h>

#include "bms_can_utils.h"
#include "can.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "iwdg.h"

void StartBmsCanCommTask(void *argument)
{
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadExit();
    }

    CAN_RxPacketTypeDef rxPacket;
    osStatus_t isMsgTakenFromQueue;
    uint32_t canID;

    for(;;)
    {
        kickWatchdogBit(osThreadGetId());

        isMsgTakenFromQueue = osMessageQueueGet(bmsRxCanMsgQueueHandle, &rxPacket, 0, pdMS_TO_TICKS(500));
        if (isMsgTakenFromQueue == osOK)
        {
            canID = rxPacket.rxPacketHeader.StdId;
            if (canID == CAN_BMS_OVERALL_ID)
            {
                process_bms_overall_packet(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_DIAGNOSTIC_ID)
            {
                process_bms_diagnostic_packet(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_VOLTAGE_ID)
            {
                process_bms_voltage_packet(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_MODULE_TEMPERATURE)
            {
                process_bms_module_temp_can(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_CELL_TEMPERATURE)
            {
                process_bms_temp_packet(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_CELL_BALANCING_RATE)
            {
                process_bms_cell_temp_balancing_rate_can(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_STATE_OF_CHARGE)
            {
                process_bms_state_of_charge_can(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_CONTACTOR_CONTROL)
            {
                process_bms_contactor_control_can(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_ENERGY_PARAM)
            {
                process_bms_energy_param_can(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_STATS)
            {
                process_bms_stats_can(rxPacket.rxPacketData);
            }
            else if (canID == CAN_BMS_EVENTS)
            {
                process_bms_events_can(rxPacket.rxPacketData);
            }
            else
            {
                // Should typeA and typeB function call go here? -> it contains all the if statements
                // and could remove the function and only have the if that's less confusing
                process_typeA_and_typeB_can_packets(&rxPacket);
            }
        }
        // remove these after testing
//        bms_request_overall_parameters();
//        bms_request_diagnostic_codes();
  //      bms_request_overall_battery_voltages();
//        bms_request_overall_cell_temp();

        osDelay(pdMS_TO_TICKS(200));
    }
}
