#ifndef AIR_H
#define AIR_H

#include "stm32f4xx.h"

void air_init();

uint8_t open_airs();
uint8_t close_airs();

void open_precharge();
uint8_t startup_precharge();
void unchecked_air_close();


////////////////
//Error Handlers
void air_weld_handler();
void air_close_fail_handler();
////////////////


#endif
