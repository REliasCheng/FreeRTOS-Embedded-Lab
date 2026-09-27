#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"

/******************************
创建1个子任务

1. 任务 扫描key：PA0, 按下：关闭所有中断，再按下，开启所有中断
2. 开启Timer5 （中断优先级>=5），通过Timer中断函数每1秒打印日志

修改Timer的中断抢占优先级，观察按键关闭中断能否生效

*********************************/
TaskHandle_t 			xStartTask_Handler;
//TaskHandle_t 			xTask1_Handler;
//TaskHandle_t 			xTask2_Handler;
TaskHandle_t 			xTaskKey_Handler;

void USART0_on_recv(uint8_t* buffer, uint32_t len){
	printf("recv[%d]-> %s\n", len, buffer);
}

void GPIO_input_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin){
  rcu_periph_clock_enable(rcu);
  gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin);
}

void GPIO_init() {
	// 按钮浮空输入（下拉输入）
	GPIO_input_config(RCU_GPIOA, GPIOA, GPIO_PIN_0);
}


void task_key(void * params){
	
	FlagStatus pre_stat = RESET;
	uint8_t flag = 0;
	while(1){
		
		FlagStatus cur_stat = gpio_input_bit_get(GPIOA, GPIO_PIN_0);
		if(cur_stat != pre_stat){
			pre_stat = cur_stat;
			
			if(cur_stat){ // 按下
				
				if(flag == 0){
					printf("关闭中断disable\n");
					// 关闭所有优先级>=5中断 (取决于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
					portDISABLE_INTERRUPTS();
				}else {
					printf("启用中断disable\n");
					portENABLE_INTERRUPTS();
				
				}
				
				flag = !flag;
				
			}
		}
		
		// 防止抖动，减少CPU负荷
//		vTaskDelay(pdMS_TO_TICKS(10));
		delay_1ms(10);
	}
}



#define PRESCALER	10000
#define FREQ	    1

static void TIMER_config() {
    // 时钟配置
    rcu_periph_clock_enable(RCU_TIMER5);

    // 复位定时器
    timer_deinit(TIMER5);

    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    timer_parameter_struct tps;
    timer_struct_para_init(&tps);
    tps.prescaler = PRESCALER - 1; // 分频系数  240 000 000
    tps.period = SystemCoreClock / PRESCALER / FREQ - 1; // 周期计数值 1Hz

    timer_init(TIMER5, &tps);
    nvic_irq_enable(TIMER5_DAC_IRQn, 5, 0); // >= 5
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);
    timer_enable(TIMER5);
}

uint32_t cnt = 0;
void TIMER5_DAC_IRQHandler(void) {
	
    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_UP)) {
			printf("timer: %d\r\n", cnt++);
			//清除中断标志位
			timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);
    }
}


void sys_init(){
	
  GPIO_init();
	USART0_init();
	TIMER_config();
}

void start_task(void){
	// 初始化各种外设
	sys_init();
	
	printf("system init\n");
	// 进入临界区
	taskENTER_CRITICAL();
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
