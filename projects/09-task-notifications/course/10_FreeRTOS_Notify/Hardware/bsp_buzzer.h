#ifndef __BSP_BUZZER_H__
#define __BSP_BUZZER_H__

#include "gd32f4xx.h"

#ifndef u16
#define	u16	uint16_t
#endif

#ifndef u8
#define	u8	uint8_t
#endif

// GPIO
#define TIMER_GPIO	RCU_GPIOB, GPIOB, GPIO_PIN_0, GPIO_AF_2

// TIMER
#define TIMER_RCU			RCU_TIMER2
#define TIMER_PERIPH	TIMER2
#define TIMER_CH			TIMER_CH_2

// 100us , 10us 90us
// PWM （确保 FREQ * PRESCALER > 3662)
#define	PRESCALER			10
//#define FREQ					1000
//#define PERIOD				SystemCoreClock / (FREQ * PRESCALER)

// 初始化蜂鸣器
void Buzzer_init();

// 按照指定频率播放
void Buzzer_play(u16 hz_val);

// 按照指定的音调播放 1,2,3,4，..7
void Buzzer_beep(u8 hz_val_index);

// 停止播放
void Buzzer_stop();

#endif