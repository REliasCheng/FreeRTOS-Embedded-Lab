#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_keys.h"

/******************************
创建三个子任务

1. 任务1：LED1-PD8 闪烁
2. 任务2：LED2-PD9 闪烁
3. 任务key：不断扫描按钮PA0

按钮KEY1: 挂起任务1
按钮KEY2: 删除任务1
按钮KEY3: 恢复任务1

按钮4：配置外部中断 EXTI_3, 下降沿：恢复任务1

*********************************/
TaskHandle_t 			xStartTask_Handler;
TaskHandle_t 			xTask1_Handler;
TaskHandle_t 			xTask2_Handler;
TaskHandle_t 			xTaskKey_Handler;

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

void GPIO_input_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin){
  rcu_periph_clock_enable(rcu);
  gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin);
}

void GPIO_init() {

  GPIO_config(RCU_GPIOC, GPIOC, GPIO_PIN_6);

  GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_8);
  GPIO_config(RCU_GPIOD, GPIOD, GPIO_PIN_9);
	
	// 按钮浮空输入（下拉输入）
	GPIO_input_config(RCU_GPIOA, GPIOA, GPIO_PIN_0);
}

void task1(void * params){
	uint8_t cnt = 0;
	while(1){
		printf("task1: %d\n", cnt++);
		gpio_bit_toggle(GPIOD, GPIO_PIN_8);
		vTaskDelay(pdMS_TO_TICKS(500));		
	}
	// 销毁启动任务，节省内存
	vTaskDelete(NULL);	
}

void task2(void * params){
	uint8_t cnt = 0;
	while(1){
		printf("task2: %d\n", cnt++);
		gpio_bit_toggle(GPIOD, GPIO_PIN_9);
		vTaskDelay(pdMS_TO_TICKS(1000));	
	}	
	// 销毁启动任务，节省内存
	vTaskDelete(NULL);	
}

void task_key(void * params){
	
	while(1){
		Keys_scan();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void Keys_on_key_down(uint8_t key){
	switch(key){
		case 0: // 挂起
			printf("task1_suspend\n");
			vTaskSuspend(xTask1_Handler);
			break;
		case 1: // 删除
			printf("task1_delete\n");
			if(xTask1_Handler != NULL){
				// 重复删除会导致系统崩溃
				vTaskDelete(xTask1_Handler);
				xTask1_Handler = NULL;
			}
			break;
		case 2: // 恢复
			printf("task1_resume11\n");
			vTaskResume(xTask1_Handler);
			break;
	}
}
void Keys_on_key_up(uint8_t key){

}

// 在优先级低于等于5（数字>=5）的中断函数里，才能调用FreeRTOS的API
// 这个界限取决于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
void EXTI_3_on_trig(){
	printf("task1_resume from isr\n");
//	vTaskResume(xTask1_Handler);
	if(xTask1_Handler != NULL){
		BaseType_t xYieldRequired = xTaskResumeFromISR(xTask1_Handler);
		portYIELD_FROM_ISR(xYieldRequired);	
	}
}

#include "EXTI.h"

void sys_init(){
	
	EXTI_init();
	USART0_init();
	
  GPIO_init();
	Keys_init();
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
