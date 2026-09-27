#ifndef __BSP_LED_PWM_H__
#define __BSP_LED_PWM_H__

#include "gd32f4xx.h"

// LED
#define TIMER_CH_LED1		TIMER0, TIMER_CH_0
#define TIMER_CH_LED2		TIMER0, TIMER_CH_0
#define TIMER_CH_LED3		TIMER7, TIMER_CH_2
#define TIMER_CH_LED4		TIMER7, TIMER_CH_2
#define TIMER_CH_LED5		TIMER7, TIMER_CH_1
#define TIMER_CH_LED6		TIMER7, TIMER_CH_1
#define TIMER_CH_LED7		TIMER7, TIMER_CH_0
#define TIMER_CH_LED8		TIMER7, TIMER_CH_0


void LED_pwm_init();

void LED_pwm_update(uint32_t timer_periph, uint16_t channel, float duty);

#endif