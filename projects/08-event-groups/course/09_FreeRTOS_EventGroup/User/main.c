#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "event_groups.h"

#include "bsp_keys.h"

/******************************
创建3个子任务

1. task1 关心信号2 （天气）
2. task2 关心信号0和1 （锅炉温度，压力）
3. KeyTask 监听扫描扩展板4个独立按键

-------------------------- 事件组
1. 创建事件组
2. 任务1和2中等待监听事件组信号
3. 按键里发送事件组信号
		按钮0：发送信号0
		按钮1：发送信号1
		按钮2：发送信号2
		按钮3：清理0，1，2信号

*********************************/
TaskHandle_t 			xStartTask_Handler;
TaskHandle_t 			xTask1_Handler;
TaskHandle_t 			xTask2_Handler;
TaskHandle_t 			xTaskKey_Handler;

#define	BIT_N(N)		(1 << N)

#define BIT_0 ( 1 << 0 )
#define BIT_1 ( 1 << 1 )
#define BIT_2 ( 1 << 2 )
#define BIT_3 ( 1 << 3 )
#define BIT_4 ( 1 << 4 )

EventGroupHandle_t eventgroup_handle;

void USART0_on_recv(uint8_t* buffer, uint32_t len){
	printf("recv[%d]-> %s\n", len, buffer);
	
}


void task_key(void * params){
	
	while(1){
		Keys_scan();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void Keys_on_key_down(uint8_t key){
//	EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup,
//                                const EventBits_t uxBitsToSet )
	switch(key){
		case 0: 
			printf("SetBits->0\n");
			xEventGroupSetBits(eventgroup_handle, BIT_0);
			break;
		case 1: 
			printf("SetBits->1\n");
			xEventGroupSetBits(eventgroup_handle, BIT_1);
			break;
		case 2: 
			printf("SetBits->2\n");
			xEventGroupSetBits(eventgroup_handle, BIT_2);
			break;
		case 3: 
			printf("ClearBits->0,1,2\n");
			xEventGroupClearBits(eventgroup_handle, BIT_0 | BIT_1 | BIT_2);
			break;
		default:
			break;
	}
}
void Keys_on_key_up(uint8_t key){

}

void task1(void * params){ // 等待信号2
	
	EventBits_t uxBits;
	
	while(1){
		printf("task1 waiting...\n");
//		EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
//                                 const EventBits_t uxBitsToWaitFor,
//                                 const BaseType_t xClearOnExit,
//                                 const BaseType_t xWaitForAllBits,
//                                 TickType_t xTicksToWait )
		uxBits = xEventGroupWaitBits(
			eventgroup_handle, // 事件组句柄
			BIT_2,						 // 关心的事件标志位，多个用 | 或 + 连在一起
			pdTRUE,						 // 退出时，是否清理标记(清0)，只会清理关心的标志位
			pdTRUE, 					 // 所有关心的标记为1时，才解除阻塞 
			portMAX_DELAY			 // 等待时长：一直等待 
		);
		
		// 0xFF FF FF
		printf("task1 ----------------------> 0x%06X\n", uxBits);
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task2(void * params){ // 等待信号0和1
	
	EventBits_t uxBits;
	while(1){
		printf("task2 waiting...\n");
		uxBits = xEventGroupWaitBits(
			eventgroup_handle, // 事件组句柄
			BIT_0 | BIT_1,		 // 关心的事件标志位，多个用 | 或 + 连在一起
			pdFALSE,					 // 退出时，是否清理标记(清0)，只会清理关心的标志位
			pdTRUE, 					 // 所有关心的标记为1时，才解除阻塞 
			portMAX_DELAY			 // 等待时长：一直等待 
		);
		
		// 0xFF FF FF
		printf("task2 ----------------------> 0x%06X\n", uxBits);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void task3(void * params){ // 等待信号0和1，2
	
	EventBits_t uxBits;
	while(1){
		printf("task3 waiting...\n");
//		EventBits_t xEventGroupSync( EventGroupHandle_t xEventGroup,
//						 const EventBits_t uxBitsToSet,
//						 const EventBits_t uxBitsToWaitFor,
//						 TickType_t xTicksToWait )
		
		// 1. 一上来就会把uxBitsToSet标志位设置为1
		// 2. 等待所有的uxBitsToWaitFor标志位都是1，才解除阻塞
		// 3. 触发成功后，把所有的标志位清0
		
		uxBits = xEventGroupSync(
			eventgroup_handle,     // 事件组句柄
			BIT_4,			  				 // 事件触发时要设置的位
			BIT_0 | BIT_1 | BIT_2, // 要等待的位
			portMAX_DELAY
		);
		
		// 0xFF FF FF
		printf("task3 ----------------------> 0x%06X\n", uxBits);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void sys_init(){
	
	USART0_init();
	Keys_init();
}

void start_task(void){
	// 初始化各种外设
	sys_init();
	
	printf("system init\n");
	
	eventgroup_handle = xEventGroupCreate();
	
	// 进入临界区
	taskENTER_CRITICAL();
	
	// 开启多个其他任务
//	xTaskCreate( task1, "task1", 64, NULL, 2, &xTask1_Handler );
	xTaskCreate( task2, "task2", 64, NULL, 3, &xTask2_Handler );
	xTaskCreate( task3, "task3", 64, NULL, 3, &xTask2_Handler );
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
