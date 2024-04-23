#ifndef GLOBAL_BOARD_CONFIG_H
#define GLOBAL_BOARD_CONFIG_H

#define ACB

//enables the profiler
#define PROFILER_ENABLE

//enables charging of acc.
//#define CHARGING_ENABLE

//enables the independant watchdog
#define IWDG_ENABLE

//enables lv charging from 300-12
//#define LV_CHARGING_ENABLE

//powers mc at startup
#define MC_STARTUP_ENABLE

//enables heartbeat response, requires vcu enabled too for handling disconnects
#define HEARTBEAT_ENABLE

//allows pulling of data logs
#define STLINK_SERVER_ENABLE

//test buzzer on startup
//#define TEST_BUZZER

//run pump on startup
//#define PUMP_TEST

#endif
