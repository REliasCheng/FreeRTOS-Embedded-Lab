#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"


TaskHandle_t 			xStartTask_Handler;
TaskHandle_t 			xTask1_Handler;
TaskHandle_t 			xTask2_Handler;

StaticTask_t idle_task_tcb;
StackType_t  idle_task_stack[configMINIMAL_STACK_SIZE];

StaticTask_t timer_task_tcb;
StackType_t  timer_task_stack[configTIMER_TASK_STACK_DEPTH];

#define TASK_STACK_SIZE   128
StackType_t     task_stack[TASK_STACK_SIZE];
StaticTask_t    task_tcb;

#define TASK1_STACK_SIZE   64
StackType_t     task1_stack[TASK1_STACK_SIZE];
StaticTask_t    task1_tcb;

#define TASK2_STACK_SIZE   64
StackType_t     task2_stack[TASK2_STACK_SIZE];
StaticTask_t    task2_tcb;

void USART0_on_recv(uint8_t* buffer, uint32_t len){
	printf("recv[%d]-> %s\n", len, buffer);
}

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // 4. 默认输出电平
  gpio_bit_write(port, pin, RESET);
}

void GPIO_init() {

  GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);

  GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_8);
  GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_9);
}

void task1(void * params){
	while(1){
		gpio_bit_toggle(GPIOD, GPIO_PIN_8);
		vTaskDelay(pdMS_TO_TICKS(500));		
	}
}
void task2(void * params){
	while(1){
		gpio_bit_toggle(GPIOD, GPIO_PIN_9);
		vTaskDelay(pdMS_TO_TICKS(1000));		
	}
}

void sys_init(){
  GPIO_init();
	USART0_init();
}


void start_task(void){
	// 初始化各种外设
	sys_init();
	
	// 进入临界区
	taskENTER_CRITICAL();

	xTask1_Handler = xTaskCreateStatic(task1, "task1", 
		TASK1_STACK_SIZE, NULL, 2, task1_stack, &task1_tcb);
	
	xTask2_Handler = xTaskCreateStatic(task2, "task2", 
		TASK2_STACK_SIZE, NULL, 2, task2_stack, &task2_tcb);

	vTaskDelete(NULL);

	// 退出临界区
	taskEXIT_CRITICAL();
}


void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                   StackType_t ** ppxIdleTaskStackBuffer,
                                   uint32_t * pulIdleTaskStackSize )
{
    * ppxIdleTaskTCBBuffer = &idle_task_tcb;
    * ppxIdleTaskStackBuffer = idle_task_stack;
    * pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                    StackType_t ** ppxTimerTaskStackBuffer,
                                    uint32_t * pulTimerTaskStackSize )
{
    * ppxTimerTaskTCBBuffer = &timer_task_tcb;
    * ppxTimerTaskStackBuffer = timer_task_stack;
    * pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


int main(void)
{
  // 全局优先级分配规则：4抢占[0,15], 0响应
  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

	// 动态方式创建任务
//	BaseType_t result = xTaskCreate(
//		(TaskFunction_t)  start_task,		// 函数的指针，函数名
//		(const char * )   "start_task",	// 函数的名称，最大长度由configMAX_TASK_NAME_LEN决定
//		(uint16_t)			  128,					// 任务栈大小，最大值65535
//		(uint8_t *)				NULL,			  // 任务函数的参数，通常NULL
//		(UBaseType_t)     1,				    // 任务优先级，数值越大，优先级越高，最大值为 configMAX_PRIORITIES - 1
//		(TaskHandle_t *)  &xStartTask_Handler	// 任务句柄
//	);
	
	// 静态方式创建任务
	xStartTask_Handler = xTaskCreateStatic(
		(TaskFunction_t)  start_task,		// 函数的指针，函数名
		(const char * )   "start_task",	// 函数的名称，最大长度由configMAX_TASK_NAME_LEN决定
		(uint16_t)			  128,					// 任务栈大小，最大值65535
		(uint8_t *)				NULL,			    // 任务函数的参数，通常NULL
		(UBaseType_t)     1,				    // 任务优先级，数值越大，优先级越高，最大值为 configMAX_PRIORITIES - 1
		task_stack, 									  // 任务栈
		&task_tcb											  // 静态任务控制块
	);
		
  vTaskStartScheduler();
  while(1);
}
