#include "App.h"
#include "oled.h"
#include "task.h"
#include "FreeRTOS.h"
#include "semphr.h"

xSemaphoreHandle xOLED_Refresh_Semaphore;

void App_OLED_refresh(uint8_t fromISR){
  if(fromISR){
    xSemaphoreGiveFromISR(xOLED_Refresh_Semaphore, NULL);
  }else {
    xSemaphoreGive(xOLED_Refresh_Semaphore);
  }
  
}

void App_OLED_init() {
  // 创建二值信号量，决定是否刷新界面
  xOLED_Refresh_Semaphore = xSemaphoreCreateBinary();

  OLED_Init();  // 初始化OLED
  
  // 初始化完毕先刷新一次界面
  xSemaphoreGive(xOLED_Refresh_Semaphore);
}

// 用于格式化字符串
char buffer[20];

void App_OLED_task() {
  while (1) {
    // 数据有变化的时候，再刷新界面(3s强制刷新一次)
    xSemaphoreTake(xOLED_Refresh_Semaphore, pdMS_TO_TICKS(3000));

    // 原有二维数组内容全部清除，但不刷新界面
    OLED_Clear(0);

    // 当前位置 C:xxx
    sprintf(buffer, "C:%3d", current);
    OLED_ShowString(0, 0, buffer, 8, 1);
    // 画横线
    OLED_DrawLine(32, 4, 127, 4, 1);
    // 画字符 [0, 300] -> [32, 124]
    uint16_t x = current / 300.0 * (124 - 32) + 32;
    OLED_ShowChar(x, 0, '@', 8, 1);

    // 目标位置 T:xxx
    sprintf(buffer, "T:%3d", target);
    OLED_ShowString(0, 8, buffer, 8, 1);
    // 画横线
    OLED_DrawLine(32, 12, 127, 12, 1);
    // 画字符 [0, 300] -> [32, 124]
    x = target / 300.0 * (124 - 32) + 32;
    OLED_ShowChar(x, 8, '*', 8, 1);

    // Kp, Ki, Kd
    sprintf(buffer, "Kp:%.3f", kp);
    OLED_ShowString(0, 16, buffer, 16, 1);
    sprintf(buffer, "Ki:%.3f", ki);
    OLED_ShowString(0, 32, buffer, 16, 1);
    sprintf(buffer, "Kd:%.3f", kd);
    OLED_ShowString(0, 48, buffer, 16, 1);
    
    if (!is_balance_enable){
      // 在右下角绘制两个长方形作为暂停标
      OLED_DrawRectangle(110, 50, 5, 13, 1, 1);
      OLED_DrawRectangle(120, 50, 5, 13, 1, 1);
    }

    // 通过I2C把内容刷新到屏幕
    OLED_Refresh();
  }
}
