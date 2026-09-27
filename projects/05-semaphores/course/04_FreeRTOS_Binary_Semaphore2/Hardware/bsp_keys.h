#ifndef __BSP_KEYS_H__
#define __BSP_KEYS_H__

#include "gd32f4xx.h"

#define KEYS_CALLBACK_ENBALE	1

#define KEY1_RCU		RCU_GPIOC
#define KEY1_GPIO		GPIOC, GPIO_PIN_0

#define KEY2_RCU		RCU_GPIOC
#define KEY2_GPIO		GPIOC, GPIO_PIN_1

#define KEY3_RCU		RCU_GPIOC
#define KEY3_GPIO		GPIOC, GPIO_PIN_2

//#define KEY4_RCU		RCU_GPIOC
//#define KEY4_GPIO		GPIOC, GPIO_PIN_3

void Keys_init();

uint8_t Keys_scan();

uint8_t Keys_is_key_down(uint8_t key);
uint8_t Keys_is_key_up(uint8_t key);

#if KEYS_CALLBACK_ENBALE
extern void Keys_on_key_down(uint8_t key);
extern void Keys_on_key_up(uint8_t key);
#endif

#endif