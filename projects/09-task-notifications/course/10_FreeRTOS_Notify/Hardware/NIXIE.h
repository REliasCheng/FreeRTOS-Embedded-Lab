#ifndef __NIXIE_H__
#define __NIXIE_H__

#include "gd32f4xx.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#define NIXIE_DI_RCU		RCU_GPIOA
#define NIXIE_DI_PORT		GPIOA
#define NIXIE_DI_PIN		GPIO_PIN_11

#define NIXIE_SCK_RCU		RCU_GPIOC
#define NIXIE_SCK_PORT	GPIOC
#define NIXIE_SCK_PIN   GPIO_PIN_11

#define NIXIE_RCK_RCU		RCU_GPIOA
#define NIXIE_RCK_PORT	GPIOA
#define NIXIE_RCK_PIN		GPIO_PIN_12

#define	NIXIE_DI(bit)		gpio_bit_write(NIXIE_DI_PORT, NIXIE_DI_PIN, bit ? SET : RESET)	// 数据输入
#define	NIXIE_SCK(bit)	gpio_bit_write(NIXIE_SCK_PORT, NIXIE_SCK_PIN, bit ? SET : RESET)	// 移位寄存器
#define	NIXIE_RCK(bit)	gpio_bit_write(NIXIE_RCK_PORT, NIXIE_RCK_PIN, bit ? SET : RESET)	// 锁存寄存器


#define GPIO_CONFIG(gpio_rcu, gpio_port, gpio_pin)		                         \
	rcu_periph_clock_enable(gpio_rcu);                                           \
	gpio_mode_set(gpio_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio_pin);        \
	gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, gpio_pin);\

void NIXIE_init();

// 		u8 a_dat = 0x12;	// 0001 0010	字母位
//		u8 b_idx = 0x1F;	// 0001 1111	数字位
void NIXIE_show(u8 a_dat, u8 b_idx);

// num对应数字在数组里的位置（索引）
// id 显示在指定位置(0 -> 7)
void NIXIE_display(u8 num, u8 id);

#endif