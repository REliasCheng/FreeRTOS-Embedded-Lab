#ifndef __USART_CONFIG_H__
#define __USART_CONFIG_H__

#include "gd32f4xx.h"

#define USE_USART0        1
#define USE_USART1        0
#define USE_USART2        0
#define USE_UART3         0
#define USE_UART4         0
#define USE_USART5        0
#define USE_UART6         0
#define USE_UART7         0

// USART0 ===================================================
#if USE_USART0
#define USART0_BAUDRATE 115200

#define RX_BUFFER_LEN 1024

// 功能开关配置 1打开，0关闭
#define USART0_RECV_CALLBACK    	1

#define USART0_TX_DMA_ENABLE			1
#define USART0_RX_DMA_ENABLE			1

// GPIO引脚配置 --------------------------------
#define USART0_TX_RCU							RCU_GPIOA
#define USART0_TX_PORT						GPIOA
#define USART0_TX_PIN							GPIO_PIN_9
					
#define USART0_RX_RCU							RCU_GPIOA
#define USART0_RX_PORT						GPIOA
#define USART0_RX_PIN							GPIO_PIN_10

// DMA配置      --------------------------------
#define USART0_TX_DMA_RCU					RCU_DMA1
#define USART0_TX_DMA_PERIPH_CH		DMA1, DMA_CH7
#define USART0_TX_DMA_CH_SUB			DMA_SUBPERI4

#define USART0_RX_DMA_RCU					RCU_DMA1
#define USART0_RX_DMA_PERIPH_CH 	DMA1, DMA_CH2
#define USART0_RX_DMA_CH_SUB			DMA_SUBPERI4
#endif

#endif