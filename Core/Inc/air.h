#ifndef AIR_H
#define AIR_H

#include "stm32f4xx.h"

void air_init();

uint8_t open_airs();
uint8_t close_airs();

typedef enum PRECHARGE_STATUS {
	PRECHARGE_FAILED = 0,
	PRECHARGE_SUCCESS = 1
} precharge_status_t;

typedef enum AIR_CLOSE_STATUS {
	AIR_CLOSE_SUCCESS = 0,
	AIR_NEG_CLOSE_FAILURE = 1,
	AIR_POS_CLOSE_FAILURE = 2
} air_status_t;


void open_precharge();
precharge_status_t startup_precharge();
void unchecked_air_close();


////////////////
//Error Handlers
void air_weld_handler();
void air_close_fail_handler();
////////////////


#endif
