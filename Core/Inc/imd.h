/*
 * main.h
 *
 *  Created on: Oct 2, 2017
 *      Author: root
 */

#ifndef IMD_H_
#define IMD_H_


/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include "gpio.h"
// Reference material: Bender IR155-3203 IR155-3204.pdf ; Datasheet for our IMD.

// These all relate to frequencies; Freq of IMD_Res dictates state
#define CNT_HZ_FACTOR 562
#define TOLERANCE 3 // HZ
//These are frequencies that indicate the operating state of the IMD.
#define IMD_OFF 0
#define IMD_NORMAL 10
#define IMD_UNDERVOLT 20
#define IMD_SPEEDSTART 30
#define IMD_ERROR 40
#define IMD_GROUNDERROR 50
#define IMD_UNKNOWNSTATE 69

// Maximum possible resistance value to be measured.
#define MAX_RES_VAL 106800

#define IMD_BADSTATE -1 // this is a dummy value that "getResist()" tosses in response to trying to read a resistance when the IMD's operating state is in an error state.

// These Define DutyCycle ranges that validate the operating condition of the IMD;ie. IMD output values are not valid when the DutyCycle(DC) is not within the allowed DC range
// for a particular operating state (freq.). They are all specified by the Datasheet.
#define IMD_DC_NORMALLOW 45 // 5% with tolerance
#define IMD_DC_NORMALHIGH 955 // 95% with tolerance.
// Defines range of IMD outputting duty cycle for "Bad" resistance (as per datasheet). from 90%(minus 1%) to 95%(+1%)
#define IMD_DC_SS_BADLOW 890
#define IMD_DC_SS_BADHIGH 960
// Similarly, range of DC values for "Good" resistance being 5-10% (w/ added tolerance)
#define IMD_DC_SS_GOODLOW 40
#define IMD_DC_SS_GOODHIGH 110
// Semi-arbitrary resistance values to correspond to "Good" and "Bad" speed-start resistance measures. Both defines are user defined; ***Not real values***
#define IMD_RES_SS_GOOD 987987; // Res is resistance;
#define IMD_RES_SS_BAD 0;

#define IMD_DC_ERRORLOW 470
#define IMD_DC_ERRORHIGH 530

/*function prototypes ---*/
uint16_t IMD_getState(void);
uint32_t IMD_getResist();
#endif /* MAIN_H_ */
