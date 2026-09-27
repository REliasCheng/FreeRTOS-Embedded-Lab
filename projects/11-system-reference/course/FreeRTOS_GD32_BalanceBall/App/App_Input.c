#include "App.h"
#include "bsp_keys.h"
#include "bsp_servo_motor.h"

void App_Input_init() {
  // 按键初始化
  Keys_init();
}


void App_Input_task(void) {
  
  while(1) {
    // 按键扫描  -------------------------------
    Keys_scan();

    vTaskDelay(pdMS_TO_TICKS(20));
  }
  
//  vTaskDelete(NULL);
}


void Keys_on_key_up(uint8_t key){

}
void Keys_on_key_down(uint8_t key) {

  if(key == 2) {
    is_balance_enable = !is_balance_enable;

    App_OLED_refresh(0);

    printf("enable: %d\n", is_balance_enable);
    return;
  }

  float current_angle = Servo_motor_get_angle();

  if(key == 0) {
    // key0 -> 角度 -= 10
    current_angle -= 10;
  } else if (key == 1) {
    // key1 -> 角度 += 10
    current_angle += 10;
  }

  // [60, 165]
  current_angle = CLIP_VALUE(current_angle, 60, 165);

  printf("update angle: %.1f\n", current_angle);
  Servo_motor_set_angle(current_angle);
}