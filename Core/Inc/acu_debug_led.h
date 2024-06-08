#ifndef LED_CTRL_H
#define LED_CTRL_H

void led_clear_all_leds();
void led_set_all_leds();

void led_set_1_red();
void led_set_1_blue();
void led_set_1_green();
void led_set_1_purple();
void led_set_1_white();
void led_clear_1();

void led_set_2_blue();
void led_set_2_red();
void led_set_2_green();
void led_set_2_purple();
void led_set_2_white();
void led_clear_2();

#define SAFETY_LOOP_OSC_TIME 50
#define NO_HEARTBEAT_TIME 50
#define FAIL_COUNT 20
#define FAIL_TIME 50

#define TASK_DELAY 50

enum LED_STATE {
	IDLE_LED = 0,
	TSA_REQUEST,
	PRECHARGE,
	TSA,
	RTD_REQUEST,
	BUZZER,
	ENABLE_COOLING,
	RTD,
	FAIL,
	SAFETY_LOOP_OPEN_LED,
	NO_VCU_HEARTBEAT,
	NO_MC_HEARTBEAT,
	VCU_IDLE_REQUEST,
	PRECHARGE_FAIL,
	AIR_FAIL
};

void setLEDState(enum LED_STATE newState);


#endif
