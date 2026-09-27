#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "USART0.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/******************************
创建3个子任务

1. task1: 阻塞式等待消息（基本数据）
2. task2: 阻塞式等待消息（复杂数据）
3. taskKey: 监听PA0按钮，按下，通过消息队列发送消息

---------------- 消息队列
1. 创建消息队列 Queue
2. 在任务里等待接收队列消息Receive 
3. 按钮按下时发送消息 Send

*********************************/
TaskHandle_t 			xStartTask_Handler;
TaskHandle_t 			xTask1_Handler;
TaskHandle_t 			xTask2_Handler;
//TaskHandle_t 			xTask3_Handler;
TaskHandle_t 			xTaskKey_Handler;

typedef struct MyData{
	char buffer[64];
	uint8_t age;
	uint32_t len;
} MyData_t;

QueueHandle_t xQueue1, xQueue2;

void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);
	
}

void GPIO_init() {
  // 按钮浮空输入（下拉输入）
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
}


void task_key(void * params) {

	uint32_t data_num = 0;
  FlagStatus pre_stat = RESET;
  uint8_t flag = 0;
	BaseType_t xReturn;
  while(1) {

    FlagStatus cur_stat = gpio_input_bit_get(GPIOA, GPIO_PIN_0);
    if(cur_stat != pre_stat) {
      pre_stat = cur_stat;

      if(cur_stat) { // 按下
        // 发送消息
				printf("Send\n");
				// ----------------------------------- quque1
				xReturn = xQueueSend(xQueue1, &data_num, portMAX_DELAY);
				data_num++;
				if(xReturn != pdTRUE){
					printf("Queue1 send error\n");
				}
				
				// ----------------------------------- quque2
				MyData_t data;
//				data.buffer = "abc123"; // char * buffer
				sprintf(data.buffer, "struct_%d", data_num);
				data.len = strlen(data.buffer);
				data.age = 18;
				xReturn = xQueueSend(xQueue2, &data, portMAX_DELAY);
				if(xReturn != pdTRUE){
					printf("Queue2 send error\n");
				}
      }
    }

    // 防止抖动，减少CPU负荷
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// task1 基本数据类型
void task1(void * params) {
	uint32_t data_num = 0;
	BaseType_t xReturn;
	while(1) {
		printf("task1 waiting...\n");
		xReturn = xQueueReceive(xQueue1, &data_num, portMAX_DELAY);
		if(xReturn == pdTRUE){
			printf("xQueue1 读成功 data_num: %d\n", data_num);
		}else {
			printf("xQueue1 读失败");
		}
		
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// task2 复杂数据类型 
void task2(void * params) {
	
	MyData_t data;
	BaseType_t xReturn;
	while(1) {
		printf("task2 waiting...\n");
		xReturn = xQueueReceive(xQueue2, &data, portMAX_DELAY);
		if(xReturn == pdTRUE){
			printf("xQueue2 读成功 buffer[%d]: %s %d\n", data.len, data.buffer, data.age);
		}else {
			printf("xQueue2 读失败");
		}
		
    vTaskDelay(pdMS_TO_TICKS(1000));
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
	
	
	// 创建消息队列
	// uxQueueLength 队列的最大长度
	// uxItemSize 每个消息占用的字节数量
	xQueue1 = xQueueCreate(3, sizeof(uint32_t));
	if(xQueue1 != NULL){
		printf("xQueue1 create successful!\n");
	}
	
	xQueue2 = xQueueCreate(32, sizeof(MyData_t));
	if(xQueue1 != NULL){
		printf("xQueue1 create successful!\n");
	}
	
  // 进入临界区
  taskENTER_CRITICAL();
  // 开启多个其他任务
  xTaskCreate( task1, "task1", 	// 任务函数，函数名称
			128, 											// 任务栈大小，最大值65535，字节数 N x 4 (字Word)
			NULL, 										// 任务参数，通常NULL											
			2, 												// 任务优先级，数值越大，优先级越高，最大值为 configMAX_PRIORITIES - 1
			&xTask1_Handler 					// 任务句柄
	);
  xTaskCreate( task2, "task2",  // 任务函数，函数名称
			128,                      // 任务栈大小，最大值65535，字节数 N x 4 (字Word)
			NULL,                     // 任务参数，通常NULL											
			3,                        // 任务优先级，数值越大，优先级越高，最大值为 configMAX_PRIORITIES - 1
			&xTask2_Handler           // 任务句柄
	);
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
