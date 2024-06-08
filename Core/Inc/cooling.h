#ifndef COOLING_H
#define COOLING_H


#include <stdint-gcc.h>

void cooling_init();
void cooling_enable_pump();
void cooling_disable_pump();
void cooling_enable_acc_fans();
void cooling_disable_acc_fans();
void enableCooling();
void disableCooling();
int checkTempsForCooling();

void cooling_enable_rad_fans();
void cooling_disable_rad_fans();

float cooling_igbt_temp_to_c(uint32_t temperature);
void Cooling_Control_Task(void * pvParameters);
void enableCoolingGently();

#define IGBT_COOLING_TEMP 500
#define CONTROL_BOARD_COOLING_TEMP 500
#define MOTOR_COOLING_TEMP 500


#endif
