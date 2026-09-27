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

1. task1: 等待接收信号, 优先级最低，3000ms
2. task2: 每1秒执行一次任务，优先级中等
3. task3: 等待接收信号，优先级最高，1000ms

---------------- 信号量
a. 创建互斥信号量
b. 默认发送一个信号 Give
c. Take得到信号的任务执行完成之后，释放信号Give

总结：

*********************************/
TaskHandle_t 			xStartTask_Handler;
TaskHandle_t 			xTask1_Handler;
TaskHandle_t 			xTask2_Handler;
TaskHandle_t 			xTask3_Handler;
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


//void task_key(void * params) {

//  FlagStatus pre_stat = RESET;
//  uint8_t flag = 0;
//  while(1) {

//    FlagStatus cur_stat = gpio_input_bit_get(GPIOA, GPIO_PIN_0);
//    if(cur_stat != pre_stat) {
//      pre_stat = cur_stat;

//      if(cur_stat) { // 按下
//        // 发送信号
//				printf("Give Semaphore\n");
//				xSemaphoreGive(xSemaphore);
//      }
//    }

//    // 防止抖动，减少CPU负荷
//    vTaskDelay(pdMS_TO_TICKS(10));
//  }
//}

// 好学生，等待接收信号 FF
void task1(void * params) {

  uint32_t task1_cnt = 0;
	while(1) {
		// 此函数会自动阻塞，直到：1.有人give信号，2.等待超时 portMAX_DELAY
		printf("task1_Low try take\n");
		xSemaphoreTake(xSemaphore, portMAX_DELAY); // pdMS_TO_TICKS(3000)
		printf("task1_Low running!\n");
		
    delay_1ms(3000); // 模拟耗时操作
		
		printf("task1_Low give!\n");
		xSemaphoreGive(xSemaphore);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// 坏同学, 优先级中等
void task2(void * params) {
  uint32_t task2_cnt = 0;
  while(1) {
		printf("task2: Mid play %d\n", task2_cnt++);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

}

// 老师，高优先级
void task3(void * params) {

  uint32_t task3_cnt = 0;
	while(1) {
		// 此函数会自动阻塞，直到：1.有人give信号，2.等待超时 portMAX_DELAY
		printf("task3_High try take\n");
		xSemaphoreTake(xSemaphore, portMAX_DELAY); // pdMS_TO_TICKS(3000)
		printf("task3_High running!\n");
		
    delay_1ms(1000); // 模拟耗时操作
		
		printf("task3_High give!\n");
		xSemaphoreGive(xSemaphore);
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
	
	// 创建二值信号量
//	xSemaphore = xSemaphoreCreateBinary();
//	xSemaphoreGive(xSemaphore);
	
	// 创建互斥型信号量
	xSemaphore = xSemaphoreCreateMutex();
	if(xSemaphore != NULL){
		printf("xSemaphore create successful!\n");
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
  xTaskCreate( task3, "task3",  // 任务函数，函数名称
			128,                      // 任务栈大小，最大值65535，字节数 N x 4 (字Word)
			NULL,                     // 任务参数，通常NULL											
			4,                        // 任务优先级，数值越大，优先级越高，最大值为 configMAX_PRIORITIES - 1
			&xTask3_Handler           // 任务句柄
	);
//  xTaskCreate( task_key, "task_key", 64, NULL, 3, &xTaskKey_Handler );

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
