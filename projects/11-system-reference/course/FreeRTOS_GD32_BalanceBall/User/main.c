#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "USART.h"
#include "TIMER.h"
#include "I2C.h"
#include "App.h"

#include "FreeRTOS.h"
#include "task.h"

/***********************
任务：

************************/

TaskHandle_t            xStartTask_Handler;
TaskHandle_t            xTask1_Handler;
TaskHandle_t            xTask2_Handler;
TaskHandle_t            xTask3_Handler;
TaskHandle_t            xTask4_Handler;


void sys_init() {
  // 初始化系统嘀嗒定时器
  USART_init();
  // 初始化TIMER
  TIMER_init();
  // 初始化I2C
  I2C_init();
}


void vStartTask( void * pvParameters ) {
  // 初始化系统固件库

  sys_init();
  // ------------------------  App初始化

  App_Input_init();
  App_Balance_init();
  App_OLED_init();
#if PROTOCOL_ENABLE
  App_Protocol_init();
#endif
  printf("Start\n");

  taskENTER_CRITICAL();
  // 临界区代码
  xTaskCreate(App_Input_task,   "task_input",    64, NULL, 3, &xTask1_Handler);
  xTaskCreate(App_Balance_task, "task_balance", 128, NULL, 5, &xTask2_Handler);
  xTaskCreate(App_OLED_task,    "task_oled",    128, NULL, 2, &xTask3_Handler);
#if PROTOCOL_ENABLE
  xTaskCreate(App_Protocol_task, "task_protocol", 64, NULL, 3, NULL);
  xTaskCreate(App_Protocol_pid_task, "task_protocol_pid", 64, NULL, 4, NULL);
  xTaskCreate(App_Protocol_recv_task, "task_protocol_recv", 64, NULL, 4, NULL);
#endif
  taskEXIT_CRITICAL();

  // 销毁自己
  vTaskDelete(xStartTask_Handler);
}

int main(void) {
  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

  // 创建任务
  BaseType_t rst = xTaskCreate(
     (TaskFunction_t) vStartTask,        // 任务函数的指针，函数名
     (const char * ) "start_task",       // 任务名称，最大长度：configMAX_TASK_NAME_LEN
     (uint16_t)      128,                // 任务栈大小，单位：半字Half Word 128*2 / 字Word
     (void*)         NULL,               // 任务函数的参数：通常NULL
     (UBaseType_t)   1,                  // 任务优先级，数值越大，优先级越高
     (TaskHandle_t *)&xStartTask_Handler  // 任务句柄
   );
  // rst: pdPASS成功，pdFAIL失败

  // 开启任务调度
  vTaskStartScheduler();

  while(1) {};
}
