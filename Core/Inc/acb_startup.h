#ifndef ACB_STARTUP_H
#define ACB_STARTUP_H



void Startup_Init();
uint8_t startup_Task_start();
TaskHandle_t get_startup_task();
void go_idle();
void go_tsa();
void go_rtd();

enum AIR_STATE {
	BOTH_OPEN = 0,
	POS_CLOSED,
	NEG_CLOSED,
	BOTH_CLOSED
};

//values for kill switches
enum DASH_BUTTON {
	RTD_BUTTON_PRESS = 1,
	KILL_SWITCH_PRESS = 2,
	TSA_BUTTON_PRESS = 3,
};

#endif
