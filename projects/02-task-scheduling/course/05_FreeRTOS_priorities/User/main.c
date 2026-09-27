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
		printf("task1\n");
		gpio_bit_toggle(GPIOD, GPIO_PIN_8);
		vTaskDelay(pdMS_TO_TICKS(500));		
	}
	// 销毁启动任务，节省内存
	vTaskDelete(NULL);	
}

void task2(void * params){
	while(1){
		printf("task2\n");
		gpio_bit_toggle(GPIOD, GPIO_PIN_9);
		vTaskDelay(pdMS_TO_TICKS(500));	
	}	
	// 销毁启动任务，节省内存
	vTaskDelete(NULL);	
}

void sys_init(){
  GPIO_init();
	USART0_init();
}


void start_task(void){
	// 初始化各种外设
	sys_init();
	
	printf("system init\n");
	// 进入临界区
	taskENTER_CRITICAL();
	// 开启多个其他任务
	xTaskCreate( task1, "task1", 64, NULL, 2, &xTask1_Handler );
	xTaskCreate( task2, "task2", 64, NULL, 3, &xTask2_Handler );
	
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
