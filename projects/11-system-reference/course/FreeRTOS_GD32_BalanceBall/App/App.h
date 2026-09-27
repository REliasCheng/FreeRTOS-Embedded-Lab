#ifndef __APP_H__
#define __APP_H__

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#define PROTOCOL_ENABLE   1

// 4,294,960,000

/****************** 用户输入 *******************/

void App_Input_init();

void App_Input_task();

/******************* 小球平衡 ******************/

extern uint8_t is_balance_enable;
// 目标距离
extern int target; // mm    
// 当前距离
extern int current; // mm
    
// PID参数
extern float kp; // 0.135 error
extern float kd;
extern float ki;

void App_Balance_init();

void App_Balance_task();

/******************** 协议收发 *****************/

extern xQueueHandle xProtocolSendQueue;

void App_Protocol_init();

void App_Protocol_task();
void App_Protocol_pid_task();
void App_Protocol_recv_task();

/******************** OLED屏幕 ******************/

void App_OLED_init();

void App_OLED_task();

void App_OLED_refresh(uint8_t fromISR);

void App_OLED_suspend();

#endif