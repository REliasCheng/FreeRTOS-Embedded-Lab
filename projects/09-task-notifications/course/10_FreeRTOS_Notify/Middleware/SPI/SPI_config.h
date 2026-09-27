#ifndef __SPI_CONFIG_H__
#define __SPI_CONFIG_H__

#include "gd32f4xx.h"

#define USE_SPI0	1
#define USE_SPI1	0
#define USE_SPI2	0
#define USE_SPI3	0
#define USE_SPI4	1
#define USE_SPI5	0


#if USE_SPI0

// 1软实现，0硬实现
#define SPI0_SOFT		1

#define	SPI0_SCL_RCU			RCU_GPIOB
#define	SPI0_SCL_PORT			GPIOB
#define SPI0_SCL_PIN			GPIO_PIN_3

#define	SPI0_MOSI_RCU			RCU_GPIOB
#define	SPI0_MOSI_PORT		GPIOB
#define SPI0_MOSI_PIN			GPIO_PIN_5

#define	SPI0_MISO_RCU			RCU_GPIOB
#define	SPI0_MISO_PORT		GPIOB
#define SPI0_MISO_PIN			GPIO_PIN_4

#endif

#if USE_SPI4

// 1软实现，0硬实现
#define SPI4_SOFT		1
					 
#define	SPI4_SCL_RCU			RCU_GPIOF
#define	SPI4_SCL_PORT			GPIOF
#define SPI4_SCL_PIN			GPIO_PIN_7
					 
#define	SPI4_MOSI_RCU			RCU_GPIOF
#define	SPI4_MOSI_PORT		GPIOF
#define SPI4_MOSI_PIN			GPIO_PIN_9
					 
#define	SPI4_MISO_RCU			RCU_GPIOF
#define	SPI4_MISO_PORT		GPIOF
#define SPI4_MISO_PIN			GPIO_PIN_8

#endif


#endif