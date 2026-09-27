#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "keys.h"
#include "usart0.h"
#include "event_groups.h"

TaskHandle_t            start_handler;
TaskHandle_t            task_key_handler;
TaskHandle_t            task1_handler;
TaskHandle_t            task2_handler;

EventGroupHandle_t eventgroup_handler;

#define	EVENT_BIT(N)		(1 << N)
#define EVENT_BIT_0			EVENT_BIT(0)
#define EVENT_BIT_1			EVENT_BIT(1)
#define EVENT_BIT_2			EVENT_BIT(2)
#define EVENT_BIT_3			EVENT_BIT(3)
#define EVENT_BIT_4			EVENT_BIT(4)
#define EVENT_BIT_5			EVENT_BIT(5)
#define EVENT_BIT_6			EVENT_BIT(6)
#define EVENT_BIT_7			EVENT_BIT(7)
#define EVENT_BIT_8			EVENT_BIT(8)
#define EVENT_BIT_9			EVENT_BIT(9)
#define EVENT_BIT_10		EVENT_BIT(10)
#define EVENT_BIT_11		EVENT_BIT(11)
#define EVENT_BIT_12		EVENT_BIT(12)
#define EVENT_BIT_13		EVENT_BIT(13)
#define EVENT_BIT_14		EVENT_BIT(14)
#define EVENT_BIT_15		EVENT_BIT(15)
#define EVENT_BIT_16		EVENT_BIT(16)
#define EVENT_BIT_17		EVENT_BIT(17)
#define EVENT_BIT_18		EVENT_BIT(18)
#define EVENT_BIT_19		EVENT_BIT(19)
#define EVENT_BIT_20		EVENT_BIT(20)
#define EVENT_BIT_21		EVENT_BIT(21)
#define EVENT_BIT_22		EVENT_BIT(22)
#define EVENT_BIT_23		EVENT_BIT(23)

void task1(void *pvParameters) {
  printf("task1: start\n");
  while(1) {

// 参数：
//		BaseType_t xClearCountOnExit, pdTRUE把通知值清零，pdFALSE只是把通知值减一
//    TickType_t xTicksToWait			，等待时长
// 返回：
//		0接受失败，非0接受成功
		uint32_t result = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		if(result != 0){
			printf("task1-----------------------> 信号量 %#x\n", result);
		}else {
			printf("task1-----------------------> fail %#x\n", result);
		}
    
    vTaskDelay(1000);
  }
  vTaskDelete(NULL);
}


void USART0_on_recv(uint8_t *data, uint32_t len) {
  printf("recv: %s\r\n", data);
}


#define IS_BIT_SET(val, n) 		((val & ( 1 << n )) > 0)
#define IS_BIT_RESET(val, n) 	((val & ( 1 << n )) == 0)

void task2(void *pvParameters) {
  printf("task2: start\n");

	uint32_t notify_val;
  while(1) {
	
//    printf("task2-----------------------> %#x\n", result);
/**
		参数：
		uint32_t ulBitsToClearOnEntry,	旧值的指定bit位设为0，设置0则不清除
		uint32_t ulBitsToClearOnExit,   新值的指定bit位设为0，设置0则不清除
		uint32_t * pulNotificationValue,取出通知值的指针，不需要的设置NULL
		TickType_t xTicksToWait					等待时长
		返回：
		pdTRUE 等待成功
		pdFALSE 等待超时
		
		| B7 | B6 | B5 | B4 | B3 | B2 | B1 | B0 |
		   0    0    0    0    1    1    0    1
*/
		xTaskNotifyWait(0x00, 0xFFFFFFFF, &notify_val, portMAX_DELAY); // 退出时，所有位清零
//		xTaskNotifyWait(0x00, 0x00, &notify_val, portMAX_DELAY);		 // 退出时，所有位不清零
		
		printf("收到消息notify_val: %#x\n", notify_val);
		
		// 给事件组判定用 ---------------------------------------------
		if (IS_BIT_SET(notify_val, 3)){
			printf("单个标志触发成功-----------------------------!\n");
		}
		
		if (IS_BIT_SET(notify_val, 0) && IS_BIT_SET(notify_val, 2)){
			printf("两个标志触发成功-----------------------------!\n");
		}
		
    vTaskDelay(1000);
  }
}

void Keys_on_key_down(uint8_t key) {

//	printf("key: %d\n", key);
	// 轻量级 二值信号量，计数信号量，队列，事件组
	
	static uint32_t ulValue = 0; // 通知值	
  switch(key) {
  case 0: // ----------------------------------------------模拟信号量
    printf("xTaskNotifyGive 模拟信号量\n");  // 模拟二值信号量、计数型信号量
		xTaskNotifyGive(task1_handler);
    break;
  case 1: // ----------------------------------------------模拟通知（覆写） 队列
		printf("xTaskNotify 发送通知（覆写）,消息邮箱[%#x]\n", ulValue);
		xTaskNotify(task2_handler, ulValue++, eSetValueWithOverwrite); // 有数据也能写进去
    break;
  case 2: // ----------------------------------------------模拟通知（不覆写） 队列
		printf("xTaskNotify 发送通知（不覆写）,消息邮箱[%#x]\n", ulValue);
		xTaskNotify(task2_handler, ulValue++, eSetValueWithoutOverwrite); // 有数据则写不进去
    break;
  case 3: // ----------------------------------------------模拟事件组标志 
		printf("xTaskNotify 事件组[%d]\n", ulValue); // bit: 0, 1, 2, 3 -> 0x01, 0x02, 0x04, 0x08
		xTaskNotify(task2_handler, EVENT_BIT(ulValue++), eSetBits);	// 将指定bit位,设置1
//		xTaskNotify(task2_handler, EVENT_BIT(0) | EVENT_BIT(1), eSetBits);	// 将指定bit位,设置1
	
		// 接收方不清零的情况下，才能看到上一个值
//		uint32_t pulPreviousNotifyValue;
//		xTaskNotifyAndQuery(task2_handler, EVENT_BIT(ulValue++), eSetBits, &pulPreviousNotifyValue);
//		printf("pre_value: %#x\n", pulPreviousNotifyValue);
    break;
  default:		
    break;
  }


}
void Keys_on_key_up(uint8_t key) {
}

void task_key(void *pvParameters) {
  Keys_init();
	
  while(1) {
    Keys_scan();

    vTaskDelay(pdMS_TO_TICKS(20));
//    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void start_task(void *pvParameters) {
  USART0_init();
  printf("start\r\n");
	
  taskENTER_CRITICAL();
  xTaskCreate(task_key, "task_key", 64, NULL, 2, &task_key_handler);

  xTaskCreate(task1, "task1", 64, NULL, 3, &task1_handler);
  xTaskCreate(task2, "task2", 64, NULL, 4, &task2_handler);

  vTaskDelete(start_handler);

  taskEXIT_CRITICAL();
}

int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
  systick_config();
  xTaskCreate(start_task, "start_task", 128, NULL, 1, &start_handler);
  vTaskStartScheduler();

  while(1) {}
}
