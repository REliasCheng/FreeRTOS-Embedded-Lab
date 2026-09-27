#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t            StartTask_Handler;
TaskHandle_t            Task1_Handler;
TaskHandle_t            Task2_Handler;

void task1(void *pvParameters) {
  while(1) {
    vTaskDelay(250);
    gpio_bit_set(GPIOD, GPIO_PIN_8);
    vTaskDelay(250);
    gpio_bit_reset(GPIOD, GPIO_PIN_8);
		
		printf("task1\n");
  }
}

void task2(void *pvParameters) {
  while(1) {
    vTaskDelay(1000);
    gpio_bit_set(GPIOD, GPIO_PIN_9);
    vTaskDelay(1000);
    gpio_bit_reset(GPIOD, GPIO_PIN_9);
		
		printf("task2\n");
  }
}

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

void sys_init(){
  GPIO_init();
	USART0_init();
	
}

void start_task(void *pvParameters) {
	
	sys_init();

  taskENTER_CRITICAL();

  xTaskCreate((TaskFunction_t)task1,
              (const char*   )"task1",
              50,
              NULL,
              2,
              (TaskHandle_t*  )&Task1_Handler);
  xTaskCreate((TaskFunction_t)task2,
              (const char*   )"task2",
              50,
              NULL,
              2,
              (TaskHandle_t*  )&Task2_Handler);
	
  // 删除自己，释放内存
  vTaskDelete(StartTask_Handler);

  taskEXIT_CRITICAL();
}


int main(void)
{

  // 全局优先级分配规则：4抢占[0,15], 0响应
  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

	// 创建启动根任务
  xTaskCreate((TaskFunction_t)start_task,
              (const char*   )"start_task",
              128,
              NULL,
              1,
              (TaskHandle_t*  )&StartTask_Handler);
  vTaskStartScheduler();

  while(1);
}
