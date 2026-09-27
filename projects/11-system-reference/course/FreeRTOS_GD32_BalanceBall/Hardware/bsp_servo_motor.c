#include "bsp_servo_motor.h"
#include "TIMER.h"
#include <math.h>

void Servo_motor_init(){

  
}

static float current_angle = 0;

float fix_raio = 0.99;
/**********************************************************
 * @brief 设置舵机角度
 * @param angle: float -> [0, 180]
 **********************************************************/
void Servo_motor_set_angle(float angle){
  angle = CLIP_VALUE(angle, 0, 180);
  
  current_angle = angle;
  
  // 按比例修正角度
  angle *= fix_raio;
  // [0, 180] -> [500/20000, 2500/20000]
  // [0, 1.0]
  // [0, 2000]
  // [500, 2500]
  // [2.5%, 12.5%]
  float duty = (500 + (angle / 180) * 2000) / 20000; 
  
  TIMER_channel_update(TIMER3, TIMER_CH_3, duty * 100);

}

float Servo_motor_get_angle(void){
  return current_angle;
}