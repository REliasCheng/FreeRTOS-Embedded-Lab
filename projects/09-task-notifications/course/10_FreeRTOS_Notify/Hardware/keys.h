#ifndef __KEYS_H__
#define __KEYS_H__

#include "gd32f4xx.h"

#define KEYS_CALLBACK_ENBALE	1

#define KEY1_RCU		RCU_GPIOD
#define KEY1_GPIO		GPIOD, GPIO_PIN_0

#define KEY2_RCU		RCU_GPIOD
#define KEY2_GPIO		GPIOD, GPIO_PIN_1

#define KEY3_RCU		RCU_GPIOD
#define KEY3_GPIO		GPIOD, GPIO_PIN_4

#define KEY4_RCU		RCU_GPIOD
#define KEY4_GPIO		GPIOD, GPIO_PIN_5

void Keys_init();

uint8_t Keys_scan();

uint8_t Keys_is_key_down(uint8_t key);
uint8_t Keys_is_key_up(uint8_t key);

#if KEYS_CALLBACK_ENBALE
void Keys_on_key_down(uint8_t key);
void Keys_on_key_up(uint8_t key);
#endif

#endif