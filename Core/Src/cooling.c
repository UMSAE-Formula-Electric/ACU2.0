#include "cooling.h"
#include "stm32f4xx.h"
#include "motor_controller_can_utils.h"
#include "FreeRTOS.h"
#include "task.h"
#include "car_state.h"
#include "gpio.h"
#include "cmsis_os2.h"
#include "iwdg.h"

#define RAD_FAN_PERIOD (SystemCoreClock/20000)

#define COOLING_MOTOR_TEMP_TO_C

#define COOLING_IGBT_TEMP_PT_0  16000 // 0 C
#define COOLING_IGBT_TEMP_PT_1  18200 //30 C
#define COOLING_IGBT_TEMP_PT_2  21700 //60 C
#define COOLING_IGBT_TEMP_PT_3  25100 //90 C



#define COOLING_IGBT_TEMP_OFFSET_ZONE_0       218.18
#define COOLING_IGBT_TEMP_MULTIPLIER_ZONE_0   0.01364

#define COOLING_IGBT_TEMP_OFFSET_ZONE_1       126
#define COOLING_IGBT_TEMP_MULTIPLIER_ZONE_1   0.008571

#define COOLING_IGBT_TEMP_OFFSET_ZONE_2       131.47
#define COOLING_IGBT_TEMP_MULTIPLIER_ZONE_2   0.008824


#define COOLING_MC_AIR_OFFSET     209.35
#define COOLING_MC_AIR_MULTIPLIER 0.01957

#define COOLING_MC_PUMP_ON_TEMP 35.0 //degrees cel
#define COOLING_MC_PUMP_OFF_TEMP 32.0

#define COOLING_MC_RAD_FAN_ON_TEMP 50.0 //degrees cel
#define COOLING_MC_RAD_FAN_OFF_TEMP 40.0



#define COOLING_AMS_TEMP_TO_C

//TODO: Turn on Accumulator Fans if Cells are too hot
//TODO: Turn on Rad Fans if motor or motor controller are too hot

//Pump logic: If in TSA, turn on pump. If idle with high temps turn on pump

//Rad logic: If pump is on and temps still too high -> define too high

//Acc Fan logc: If cell temps are too high turn on fans

void cooling_init() {

	//disable everything
	cooling_disable_pump();
	cooling_disable_rad_fans();
}

void cooling_enable_pump() {
    HAL_GPIO_WritePin(PUMP_CTRL_GPIO_Port, PUMP_CTRL_Pin, GPIO_PIN_SET);
}

void cooling_disable_pump() {
    HAL_GPIO_WritePin(PUMP_CTRL_GPIO_Port, PUMP_CTRL_Pin, GPIO_PIN_RESET);
}

void cooling_enable_rad_fans() {
	//set 10% duty cycle
    HAL_GPIO_WritePin(FANS_CTRL_GPIO_Port, FANS_CTRL_Pin, GPIO_PIN_SET);
	//TIM_SetCompare2(TIM9, RAD_FAN_PERIOD/10); //Default 10% duty cycle is TIM_BaseStruct.TIM_Period/10
}

void cooling_disable_rad_fans() {
    HAL_GPIO_WritePin(FANS_CTRL_GPIO_Port, FANS_CTRL_Pin, GPIO_PIN_RESET);
	//TIM_SetCompare2(TIM9, 0);
}


//Convert igbt temp from MC to degreees c, non-linearish
float cooling_igbt_temp_to_c(uint32_t temperature){
	float temp_in_c = 0;
	if(COOLING_IGBT_TEMP_PT_0 < temperature && temperature <= COOLING_IGBT_TEMP_PT_1){
		temp_in_c =  COOLING_IGBT_TEMP_MULTIPLIER_ZONE_0 *temperature  - COOLING_IGBT_TEMP_OFFSET_ZONE_0;
	}
	else if(COOLING_IGBT_TEMP_PT_1 < temperature && temperature <= COOLING_IGBT_TEMP_PT_2) {
		temp_in_c =  COOLING_IGBT_TEMP_MULTIPLIER_ZONE_1 *temperature  - COOLING_IGBT_TEMP_OFFSET_ZONE_1;
	}
	else if(COOLING_IGBT_TEMP_PT_1 < temperature){
		temp_in_c =  COOLING_IGBT_TEMP_MULTIPLIER_ZONE_2 *temperature  - COOLING_IGBT_TEMP_OFFSET_ZONE_2;
	}
	else{
		temp_in_c = 0.0;
	}
	return temp_in_c;
}

float cooling_mc_air_temp_to_c(uint32_t temperature){
    float temp_in_c = 0;
    temp_in_c = COOLING_MC_AIR_MULTIPLIER * temperature - COOLING_MC_AIR_OFFSET;
    return temp_in_c;
}


/**
  * @brief  Monitor temperatures and turn pump and rad fans on accordingly
  * @retval never return from a freeRTOS task, kills task if infinite task ends
  */
void StartCoolingTask(void *argument){
    uint8_t isTaskActivated = (int)argument;
    if (isTaskActivated == 0) {
        osThreadTerminate(osThreadGetId());
    }

	float mc_igbt_temp = 0.0; //degrees c
	float mc_air_temp = 0.0; // degrees c

	for(;;){
        kickWatchdogBit(osThreadGetId());
      if(get_car_state() == READY_TO_DRIVE){//check for rtd

    	if(mc_getAverageIGBTTemp() > COOLING_MC_PUMP_ON_TEMP) {
    		//cooling_enable_pump();
    	} else if(mc_getAverageIGBTTemp() < COOLING_MC_PUMP_OFF_TEMP){
    		//cooling_disable_pump();
    	}

    	if(mc_getAverageIGBTTemp() > COOLING_MC_RAD_FAN_ON_TEMP) {
    		//cooling_enable_rad_fans();
    	} else if(mc_getAverageIGBTTemp() < COOLING_MC_RAD_FAN_OFF_TEMP) {
    		//cooling_disable_rad_fans();
    	}

        //read temps and convert to eng units
        //mc_igbt_temp = cooling_igbt_temp_to_c( MC_getIGBTTemp() );
        //mc_air_temp = cooling_mc_air_temp_to_c( MC_getAirTemp() );
        /*
        //check turning on pumps
        if(mc_igbt_temp > COOLING_MC_PUMP_ON_TEMP || mc_air_temp >  COOLING_MC_PUMP_ON_TEMP){
          cooling_enable_pump();
        }
        else if(mc_igbt_temp < (COOLING_MC_PUMP_ON_TEMP) && mc_air_temp < (COOLING_MC_PUMP_ON_TEMP ) ){
          cooling_disable_pump();
        }

        //check turning on rad fans
        if(mc_igbt_temp > COOLING_MC_RAD_FAN_ON_TEMP || mc_air_temp >  COOLING_MC_RAD_FAN_ON_TEMP){
          cooling_enable_rad_fans();
        }
        else if(mc_igbt_temp < (COOLING_MC_RAD_FAN_ON_TEMP) && mc_air_temp < (COOLING_MC_RAD_FAN_ON_TEMP) ){
          cooling_disable_rad_fans();
        }
        */
      }
      else{
        //not rtd let it cool to ambient
        cooling_disable_pump();
        vTaskDelay(pdMS_TO_TICKS(3000));
        cooling_disable_rad_fans();
      }
      vTaskDelay(10000);
      osThreadYield();
    }

}

void enableCoolingGently() {
	cooling_enable_pump();
	vTaskDelay(pdMS_TO_TICKS(2000));
	cooling_enable_rad_fans();
}

