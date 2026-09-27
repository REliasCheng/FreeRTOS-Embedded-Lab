#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/******************************
创建3个子任务

1. 任务Key扫描：PA0按下，发送信号
2. task1: 等待接收信号
3. task2: 不等待信号，独立运行 2000ms

---------------- 信号量
a. 创建二值信号量 Create
b. task1 等待获取信号 Take
c. 按下事件发送信号 Give
d. 串口接收中断里发信号 Give

*********************************/
TaskHandle_t 			xStartTask_Handler;
TaskHandle_t 			xTask1_Handler;
TaskHandle_t 			xTask2_Handler;
TaskHandle_t 			xTaskKey_Handler;

SemaphoreHandle_t xSemaphore = NULL;

void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);
	
	// 在中断里发送二值(二进制)信号量
	xSemaphoreGiveFromISR(xSemaphore, NULL);
}

void GPIO_init() {
  // 按钮浮空输入（下拉输入）
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
}


void task_key(void * params) {

  FlagStatus pre_stat = RESET;
  uint8_t flag = 0;
  while(1) {

    FlagStatus cur_stat = gpio_input_bit_get(GPIOA, GPIO_PIN_0);
    if(cur_stat != pre_stat) {
      pre_stat = cur_stat;

      if(cur_stat) { // 按下
        // 发送信号
				printf("Give Semaphore\n");
				xSemaphoreGive(xSemaphore);
      }
    }

    // 防止抖动，减少CPU负荷
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// 等待接收信号
void task1(void * params) {

  uint32_t task1_cnt = 0;
	BaseType_t xReturn;
  while(1) {
		// 此函数会自动阻塞，直到：1.有人give信号，2.等待超时 portMAX_DELAY
		xReturn = xSemaphoreTake(xSemaphore, portMAX_DELAY);
		
    printf("task1: %d return: %d\n", task1_cnt++, (uint8_t)xReturn);
//    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// 不等待信号，直接执行
void task2(void * params) {
  uint32_t task2_cnt = 0;
  while(1) {
    printf("task2: %d\n", task2_cnt++);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }

}

void sys_init() {
  GPIO_init();
  USART0_init();
}
void start_task(void) {
  // 初始化各种外设
  sys_init();

  printf("system init\n");
	
	// 创建二值信号量
	xSemaphore = xSemaphoreCreateBinary();
	if(xSemaphore != NULL){
		printf("xSemaphore create successful!\n");
	}
	
  // 进入临界区
  taskENTER_CRITICAL();
  // 开启多个其他任务
  xTaskCreate( task1, "task1", 64, NULL, 2, &xTask1_Handler );
  xTaskCreate( task2, "task2", 64, NULL, 3, &xTask2_Handler );
  xTaskCreate( task_key, "task_key", 64, NULL, 3, &xTaskKey_Handler );

  // 销毁启动任务，节省内存
  vTaskDelete(xStartTask_Handler);
  // 退出临界区, 所有任务才一起开始执行
  taskEXIT_CRITICAL();

}


int main(void)
{
  // 全局优先级分配规则：4抢占[0,15], 0响应
  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

  BaseType_t result = xTaskCreate(
                        (TaskFunction_t)   start_task,				// 函数的指针，函数名
                        (const char * )   "start_task",				// 函数的名称，最大长度由configMAX_TASK_NAME_LEN决定
                        (uint16_t)			  128,								// 任务栈大小，最大值65535
                        (uint8_t *)				NULL,			  			  // 任务函数的参数，通常NULL
                        (UBaseType_t)     1,				    			// 任务优先级，数值越大，优先级越高，最大值为 configMAX_PRIORITIES - 1
                        (TaskHandle_t *)  &xStartTask_Handler	// 任务句柄
                      );
  //	根据返回结果可以知道是否创建成功 pdPASS, pdFAIL

  vTaskStartScheduler();

  while(1);
}
