#ifndef __USART0_H__
#define __USART0_H__

#include "gd32f4xx.h"

// 通过宏定义，初始化一些变量值
// 是否开启接收
#define USART0_RECV_CALLBACK			1
#define USART0_RECV_BYTE_CALLBACK 0
#define USART0_PRINTF							1

// GPIO ----------------------------------------------------------------
// TX
#define USART0_TX_RCU 	 	RCU_GPIOA
#define USART0_TX_PORT  	GPIOA
#define USART0_TX_PIN   	GPIO_PIN_9
#define USART0_TX_AF  	 	GPIO_AF_7

// RX
#define USART0_RX_RCU 		RCU_GPIOA
#define USART0_RX_PORT  	GPIOA
#define USART0_RX_PIN   	GPIO_PIN_10
#define USART0_RX_AF  	 	GPIO_AF_7

// USART ----------------------------------------------------------------
// USART 波特率
#define USART0_BAUDRATE 	 	115200
#define	RX_BUFFER_LEN				1024

// DMA ------------------------------------------------------------------
#define USART0_TX_DMA_ENABLE	0
#define USART0_RX_DMA_ENABLE	0

// DMA用到的USART数据收发寄存器
#define USART0_DATA_ADDR						((uint32_t)&USART_DATA(USART0))

#if USART0_TX_DMA_ENABLE
#define USART0_TX_DMA_RCU						RCU_DMA1
#define USART0_TX_DMA_PERIPH_CH			DMA1, DMA_CH7
#define USART0_TX_DMA_CH_SUB				DMA_SUBPERI4
#endif

#if USART0_RX_DMA_ENABLE
#define USART0_RX_DMA_RCU						RCU_DMA1
#define USART0_RX_DMA_PERIPH_CH			DMA1, DMA_CH2
#define USART0_RX_DMA_CH_SUB				DMA_SUBPERI4
#endif

void USART0_init();

// 发送1字节数据
void USART0_send_byte(uint8_t data);

// 发送字节数组
void USART0_send_data(uint8_t* data, uint32_t len);

// 发送字符串
void USART0_send_string(char *data);

#if USART0_RECV_CALLBACK
extern void USART0_on_recv(uint8_t* data, uint32_t len);
#endif

#if USART0_RECV_BYTE_CALLBACK
extern void USART0_on_byte_recv(uint8_t data);
#endif


#endif