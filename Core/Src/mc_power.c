
#include <mc_power.h>
#include "gpio.h"

//TODO Check MC pins and their initialization
void hard_enable_MC(){
	HAL_GPIO_WritePin(MC_PWR_FB_GPIO_Port, MC_PWR_FB_Pin, GPIO_PIN_SET);
}

void startup_MC(){
    HAL_GPIO_WritePin(MC_PWR_FB_GPIO_Port, MC_PWR_FB_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MC_PWR_CTRL_GPIO_Port, MC_PWR_CTRL_Pin, GPIO_PIN_SET);
}

void shutdown_MC(){
	HAL_GPIO_WritePin(MC_PWR_FB_GPIO_Port, MC_PWR_FB_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MC_PWR_CTRL_GPIO_Port, MC_PWR_CTRL_Pin, GPIO_PIN_RESET);
}
