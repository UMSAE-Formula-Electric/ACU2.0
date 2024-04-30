#ifndef VCU_COMMS_HANDLER_H
#define VCU_COMMS_HANDLER_H
#include "car_state.h"
#include "can.h"

void processVcuSetAcuStateCanIdRxData(const uint8_t *RxData);
void processVcuToAcuCanIdRxData(const uint8_t *RxData);
uint8_t vcu_comms_init();
void send_TSA_ack();
void send_TSA_nack();
void send_precharge_pass();
void send_precharge_fail();
void send_air_fail();
void send_VCU_mesg(enum ACB_TO_CAN_MSG msg);

#endif
