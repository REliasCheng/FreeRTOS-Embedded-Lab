#ifndef __TIMER_CONFIG_H__
#define __TIMER_CONFIG_H__

#include "gd32f4xx.h"

#define USE_TIMER_0		1
#define USE_TIMER_1		0
#define USE_TIMER_2		0
#define USE_TIMER_3		0
#define USE_TIMER_4		0
#define USE_TIMER_5		0
#define USE_TIMER_6		0
#define USE_TIMER_7		1

/******************************* TIMER 0 *********************************/
#if USE_TIMER_0

#define TM0_PRESCALER		1
#define TM0_FREQ				10000
#define TM0_PERIOD			SystemCoreClock / (TM0_FREQ * TM0_PRESCALER)

#define TM0_CH0				RCU_GPIOA, GPIOA, GPIO_PIN_8, GPIO_AF_1
#define TM0_CH0_ON		RCU_GPIOA, GPIOA, GPIO_PIN_7,	GPIO_AF_1

//#define TM0_CH1				RCU_GPIOA, GPIOA, GPIO_PIN_8, GPIO_AF_1
//#define TM0_CH1_ON		RCU_GPIOA, GPIOA, GPIO_PIN_7,	GPIO_AF_1

#endif

/******************************* TIMER 7 *********************************/
#if USE_TIMER_7
#define TM7_PRESCALER		1
#define TM7_FREQ				10000
#define TM7_PERIOD			SystemCoreClock / (TM7_FREQ * TM7_PRESCALER)

// AF3 ------------------
// CH0£ºPC6, PI5, PA5_ON, PA7_ON, PH13_ON
// CH1£ºPC7, PI6, PB0_ON, PB14_ON, PH14_ON

// CH2£ºPC8, PI7£¬PB1_ON, PB15_ON, PH15_ON
#define TM7_CH2					RCU_GPIOC, GPIOC, GPIO_PIN_8, 	GPIO_AF_3
#define TM7_CH2_ON			RCU_GPIOB, GPIOB, GPIO_PIN_15, 	GPIO_AF_3

#endif

#endif