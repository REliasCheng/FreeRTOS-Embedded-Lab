#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#include "bsp_keys.h"

/******************************
创建1个子任务

1. 创建并开启FreeRTOS Timer1，通过中断函数每1秒打印一次日志
2. 创建并开启FreeRTOS Timer2，通过中断函数每0.5秒打印一次日志
3. 任务1：扫描独立按键Key，按下：
	a. 停止Timer
	b. 启用Timer
	c. 删除Timer
*********************************/
TaskHandle_t 			xStartTask_Handler;
//TaskHandle_t 			xTask1_Handler;
//TaskHandle_t 			xTask2_Handler;
TaskHandle_t 			xTaskKey_Handler;

TimerHandle_t timer1_handle;
TimerHandle_t timer2_handle;

void USART0_on_recv(uint8_t* buffer, uint32_t len){
	printf("recv[%d]-> %s\n", len, buffer);
	
	if(buffer[0] == 0x00){
		printf("timer_stop from ISR\n");
		xTimerStopFromISR(timer2_handle, NULL);
	}else if(buffer[0] == 0x01){
		printf("timer_start from ISR\n");
		xTimerStartFromISR(timer2_handle, NULL);
	}
	
}


void GPIO_init() {

}


void task_key(void * params){
	
	while(1){
		Keys_scan();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void Keys_on_key_down(uint8_t key){
	switch(key){
		case 0: // 停止
			printf("timer_stop\n");
			xTimerStop(timer2_handle, portMAX_DELAY);// pdMS_TO_TICKS(1000)
			break;
		case 1: // 启用
			printf("timer_start\n");
			xTimerStart(timer2_handle, 0);
			break;
		case 2: // 删除
			printf("timer_delete\n");
			if(timer2_handle != NULL){
				xTimerDelete(timer2_handle, 0);
				timer2_handle = NULL;
			}
			break;
	}
}
void Keys_on_key_up(uint8_t key){

}

void sys_init(){
	
  GPIO_init();
	USART0_init();
	Keys_init();
}

uint32_t timer1_cnt = 0;
uint32_t timer2_cnt = 0;
void timer_cb(TimerHandle_t xTimer ){
	
	// 获取xTimer的pvTimerID
	int timerId = (int)pvTimerGetTimerID(xTimer);
	
	if(timerId == 1){
		printf("timer1: %d\n", timer1_cnt++);
	}else if(timerId == 2){
		printf("timer2: %d\n", timer2_cnt++);
	}
	
}

void start_task(void){
	// 初始化各种外设
	sys_init();
	
	printf("system init\n");
	
	// 进入临界区
	taskENTER_CRITICAL();
	
	// 开启FreeRTOS Timer (configUSE_TIMERS = 1)
	
//	TimerHandle_t xTimerCreate( 
//			const char * const pcTimerName,
//			const TickType_t xTimerPeriodInTicks,
//			const BaseType_t xAutoReload,
//			void * const pvTimerID,
//			TimerCallbackFunction_t pxCallbackFunction )
	
	timer1_handle = xTimerCreate(
		"timer1",  							// timer名称
		pdMS_TO_TICKS(1000),		// 间隔时间，单位ticks, 通过pdMS_TO_TICKS把ms转成ticks
		pdTRUE,								  // 自动重载 pdTRUE, pdFALSE
		(void *)1,							// TimerID
		timer_cb								// 回调函数
	);
	timer2_handle = xTimerCreate(
		"timer2",  							// timer名称
		pdMS_TO_TICKS(500),		  // 间隔时间，单位ticks, 通过pdMS_TO_TICKS把ms转成ticks
		pdTRUE,								  // 自动重载 pdTRUE, pdFALSE
		(void *)2,							// TimerID
		timer_cb								// 回调函数
	);
	
	// 启动timer
	// 参数1：Timer句柄
	// 参数2：当Timer任务列表已满，最大等待时长
	xTimerStart(timer1_handle, 0);
	xTimerStart(timer2_handle, 0);
	
	// 开启多个其他任务
//	xTaskCreate( task1, "task1", 64, NULL, 2, &xTask1_Handler );
//	xTaskCreate( task2, "task2", 64, NULL, 3, &xTask2_Handler );
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
