#include "App.h"
#include "bsp_VL53L0X.h"
#include "bsp_servo_motor.h"

extern VL53L0X_Dev_t vl53l0x_dev;  // 设备I2C数据参数

// 当前舵机角度
float init_angle = 130;

// 目标距离
int target = 150;  // mm

// 当前距离
int current = 150;


// 启用调平
uint8_t is_balance_enable = 1;
// pid参数 （较好）
// float kp = 0.1;
// float kd = 0.15;
// float ki = 0.002;

// 待修正
float kp = 0.122;  // 0.135 error
float kd = 0.231;
float ki = 0.004;

VL53L0X_Error Status = VL53L0X_ERROR_NONE;  // 工作状态
// 激光测距模块初始化
uint8_t mode = 0;  // 0：默认;1:高精度;2:长距离;3:高速

void App_Balance_init() {
  // 舵机初始化
  Servo_motor_init();


//  while (vl53l0x_init(&vl53l0x_dev))  // vl53l0x初始化
//  {
//    printf("VL53L0X Error!!!\n\r");
//    vTaskDelay(pdMS_TO_TICKS(200));
//  }

//  while (vl53l0x_set_mode(&vl53l0x_dev, mode))  // 配置测量模式
//  {
//    printf("Mode Set Error\r\n");
//    vTaskDelay(pdMS_TO_TICKS(200));
//  }

  // ------------------------- 默认状态初始化
  Servo_motor_set_angle(init_angle);
}

uint8_t init_state = 0;
uint8_t err_cnt = 0;

void App_Balance_task() {
  //    INTERVAL_CHECK(task_tick, TASK_INTERVAL);

  static int cte = 0, last_cte = 0, integral_cte = 0;  // Cross Track Error

  float p = 0, i = 0, d = 0;
  while (1) {
    
    if(!init_state) {
      if(vl53l0x_init(&vl53l0x_dev) == VL53L0X_ERROR_NONE) {
        vl53l0x_set_mode(&vl53l0x_dev, mode);
        printf("VL53L0X OK\r\n");
        init_state = 1;      
      }
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    // 小球平衡 20ms -------------------------------
    Status = VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev, &vl53l0x_data);
    if (Status != VL53L0X_ERROR_NONE) {
      // 执行单次测距并获取测距测量数据
      printf("VL53L0X error: %d\r\n", Status);
      if(err_cnt++ >= 10){
        // 重新初始化
        init_state = 0;
      }
      
      vTaskDelay(pdMS_TO_TICKS(200));
      continue;
    }
    current = vl53l0x_data.RangeMilliMeter;  // 小球的距离 [20mm, 2000mm]

    // 发送当前距离, 用于上位机显示, 如遇阻塞则丢弃
    xQueueSend(xProtocolSendQueue, &current, 0);
    App_OLED_refresh(0);

//    printf("current: %4dmm\n", current);

#if 1
    // Kp, Kd, Ki
    // 计算本次误差（当前位置和目标的差值）Proportional
    cte = current - target;
    // 负值：小球太近，正值：小球太远 100mm -> 150mm , -50mm
    p = kp * cte;

    // 计算前两次误差的变化 Derivative
    d = kd * (cte - last_cte);
    last_cte = cte;

    // 计算历史误差的和 （消除稳态误差） Integral
    integral_cte += cte;
    i = ki * integral_cte;

    // 避免过多误差累计
    integral_cte = CLIP_VALUE(integral_cte, -10000, 10000);

    float pid = p + d + i;
    // 将运动角度限定到[60, 165]
    float angle = init_angle + pid;
    angle = CLIP_VALUE(angle, 60, 165);

#if !PROTOCOL_ENABLE
    printf("C: %4imm T: %4imm cte: %4dmm i_cte: %4dmm \n",
           current,target, cte, integral_cte);//打印测量距离 Max: 2000mm
#endif

    if (is_balance_enable) {
      Servo_motor_set_angle(angle);
    }
#endif

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}