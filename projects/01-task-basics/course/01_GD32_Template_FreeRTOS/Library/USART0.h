#ifndef __USART0_H__
#define __USART0_H__

#include "gd32f4xx.h"
#include "USART_config.h"

void USART0_init();
// 发送一个字节数据
void USART0_send_data(uint8_t data);
// 发送多个字节
void USART0_send_array(uint8_t* data, uint32_t len);
// 发送字符数组 （\0是字符串结束标记）
void USART0_send_string(char* data);

#if USART0_TX_DMA_ENABLE

// 发送一个字节数据
void USART0_dma_send_data(uint8_t data);
// 发送多个字节
void USART0_dma_send_array(uint8_t* data, uint32_t len);
// 发送字符数组 （\0是字符串结束标记）
void USART0_dma_send_string(char* data);

#endif 

extern void USART0_on_recv(uint8_t* buffer, uint32_t len);

#endif