/*
 * imd.c
 *
 * Written by Cole, Laura, Martin
 */

#include "imd.h"
#include "FreeRTOS.h"
#include "task.h"

static uint16_t IC2Value = 0;
static uint16_t DutyCycle = 0;
static uint32_t Frequency = 0;

uint16_t PrescalerValue = 0;


uint16_t IMD_State = 0;
uint32_t resistVal = 0;


/*
 * IMD_getState
 *
 * @Brief: returns the state of the IMD
 */
uint16_t IMD_getState(void){
	// At this point, Duty Cycle and Frequency are calculated.
	uint16_t state = 0;
	uint16_t tempDuty = DutyCycle;
	uint16_t tempFreq = Frequency;

	if (IMD_OFF == tempFreq)
	{
		state = IMD_OFF;
	}
	else if (((IMD_NORMAL-TOLERANCE ) < tempFreq) && (tempFreq < IMD_NORMAL+TOLERANCE)){
		if ((IMD_DC_NORMALLOW < tempDuty) && (tempDuty < IMD_DC_NORMALHIGH))
		{
			state = IMD_NORMAL;
		}
	}
	else if (((IMD_UNDERVOLT-TOLERANCE ) < tempFreq) && (tempFreq < (IMD_UNDERVOLT+TOLERANCE))){
		if ((IMD_DC_NORMALLOW < tempDuty) && (tempDuty < IMD_DC_NORMALHIGH))
		{
			state = IMD_UNDERVOLT;
		}
	}
	else if (((IMD_SPEEDSTART-TOLERANCE ) < tempFreq) && (tempFreq < (IMD_SPEEDSTART+TOLERANCE))){
		state = IMD_SPEEDSTART;
	}
	else if (((IMD_ERROR-TOLERANCE ) < tempFreq) && tempFreq < (IMD_ERROR+TOLERANCE)){
		if((IMD_DC_ERRORLOW < tempDuty) && (tempDuty < IMD_DC_ERRORHIGH))
		{
			state = IMD_ERROR;
		}
	}
	else if (((IMD_GROUNDERROR-TOLERANCE ) < tempFreq) && (tempFreq < (IMD_GROUNDERROR+TOLERANCE))){
		if((IMD_DC_ERRORLOW < tempDuty) && (tempDuty < IMD_DC_ERRORHIGH))
		{
			state = IMD_GROUNDERROR;
		}
	}
	else{ // Should not expect to see this due to operating principles of IMD, (constant state/frequency operation + low-noise at low-freq.)
		// Also not a hard-fault at this stage of method. Potential for frequency to "recover" on a subsequent read.
		state = IMD_UNKNOWNSTATE;
	}
	return state;
}

/*
 * IMD_getResist
 *
 * @Brief: Returns the measured resistance inbetween the LV and HV systems
 */
uint32_t IMD_getResist(){
  // ********** "First successful insulation measurement at <= 17.5 seconds" ***********
  // Also note: IMD_UnderVoltage state is user determinable, ie. User is meant to configure the Bender hardware to have indicate an undervoltage condition based on user decision.
  // Do we have this implemented already? Is this a set feature based on the IMD we buy off the shelf? Else, this would be nice to look into.

  uint32_t resistance = 0; // Good value for initialize as 0 ohms resist on IMD is worst case;
  uint16_t currState = IMD_getState(); // or can use global variable but whatever; If doing this instead of referring to global var, IMD_getResist takes a little longer obv.
  uint32_t tempDuty = DutyCycle; // to avoid DutyCycle misbehaving/being affected by ISR updating global var.

  if ((currState == IMD_NORMAL) || (currState == IMD_UNDERVOLT)) // Same calc for both states.
  {
    // validate dutycycle for Data-sheet indicated valid dutycycle operating range.
      if (tempDuty < 50) //5%
      {
        resistance = 0xffffffff; //invalid/out of range
      }
      else if(tempDuty == 50){
        resistance = 0xffffffff; //max resistanve
      }
      else{
        resistance = ((900 * 1200000) / (tempDuty - 50)) - 1200000; // 900 is 90% DC and 1200 is 1200 Ohms; (Bender IR155-3203 IR155-3204.pdf ; Page 5)
      }

  }
  else if ( currState == IMD_SPEEDSTART)
  {
    // Gives a "Good" or "Bad" estimation; This is for when the IMD gets turned on so you can get feedback 2s after start-up; Get more accurate after 17.5s with Normal State.
    if ((IMD_DC_SS_BADLOW < tempDuty) && (tempDuty < IMD_DC_SS_BADHIGH))
    {
      resistance = IMD_RES_SS_BAD;
    }
    else if((IMD_DC_SS_GOODLOW < tempDuty) && (tempDuty < IMD_DC_SS_GOODHIGH))
    {
      resistance = IMD_RES_SS_GOOD;
    }
  }
  else
  {
      resistance = IMD_BADSTATE; // Operating in error condition of IMD that we cant calc res for.
  }
  return resistance;
}
