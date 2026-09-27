#ifndef __TIMER_H__
#define __TIMER_H__

#include "gd32f4xx.h"

#include "TIMER_config.h"

// LED1: TM0CH0_ON: 	PA7 	AF1
// LED2: TM0CH0: 			PA8 	AF1
// LED3: TM7CH2_ON:		PB15 	AF3
// LED4: TM7CH2: 			PC8 	AF3
// LED5: TM7CH1_ON: 	PB14 	AF3
// LED6: TM7CH1: 			PC7 	AF3
// LED7: TM7CH0_ON: 	PA5 	AF3
// LED8: TM7CH0: 			PC6 	AF3


/******************************* TIMER 0 *********************************/
#if USE_TIMER_0
void TIMER_0_channel_update(uint16_t channel, float duty);
#endif

/******************************* TIMER 7 *********************************/
#if USE_TIMER_7
void TIMER_7_channel_update(uint16_t channel, float duty);
#endif


void TIMER_init();


#endif