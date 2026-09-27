#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "gd32f4xx.h"
#include <stdio.h>

// 联合体
typedef union {
  float floatValue;
  uint8_t bytesValue[4];
} FloatBytes;

// 将float转换为字节数组
void floatToBytes(float f, uint8_t bytes[], uint8_t inverse);

// 将字节数组转换为float
float bytesToFloat(uint8_t bytes[], uint8_t inverse);

// 初始化协议解析器
void Protocol_init();

// 接收一个字节数据
void Protocol_recv_byte(uint8_t byte);

// 接收一组数据
void Protocol_recv_arr(uint8_t* arr, uint32_t len);

// 执行解析任务
void Protocol_task();

// 解析完成，把结果返回给调用者
extern void on_data_parsed(uint8_t* data, uint32_t len);


// ========================== 发送 =================

void Protocol_gen_current(int current, uint8_t *data, uint32_t len);

void Protocol_gen_pid(uint8_t* bytes, uint8_t *data, uint32_t len);

#endif