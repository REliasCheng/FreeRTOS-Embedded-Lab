#ifndef __BSP_BUZZER_H__
#define __BSP_BUZZER_H__

#include "gd32f4xx.h"

#define	u16	uint16_t
#define	u8	uint8_t

// 初始化蜂鸣器
void Buzzer_init();

// 按照指定频率播放
void Buzzer_play(u16 hz_val);

void Buzzer_beep(u16 idx);

// 停止播放
void Buzzer_stop();


#endif