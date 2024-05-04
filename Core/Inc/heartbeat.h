#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "FreeRTOS.h"
#include "task.h"
#include "heartbeat_utils.h"

TaskHandle_t heartbeat_get_task();
HeartbeatState_t get_heartbeat_state();

#endif
