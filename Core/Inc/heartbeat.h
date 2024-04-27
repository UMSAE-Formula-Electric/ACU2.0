#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "FreeRTOS.h"
#include "task.h"

typedef enum {
	HEARTBEAT_REQUEST_NOTIFY = 0,
	HEARTBEAT_RESPONSE_NOTIFY
} heatbeat_notif_vals_t;

typedef enum {
	HEARTBEAT_NO_VCU = 0, //THE ACB has not yet connected with the VCU
	HEARTBEAT_LOST_VCU, 	//The ACB was previously connected with the VCU but has since disconnected
	HEARTBEAT_HAS_VCU
} heatbeat_state_t;

uint8_t heartbeat_init();
TaskHandle_t heartbeat_get_task();
heatbeat_state_t get_heartbeat_state();

#endif
