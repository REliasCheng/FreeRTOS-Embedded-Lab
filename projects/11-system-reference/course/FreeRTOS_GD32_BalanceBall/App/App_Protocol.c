#include "App.h"
#include "Protocol.h"
#include "USART.h"
/************************************

通过协议进行数据的收发

按下扩展板独立按键 PC3 启用/禁用舵机调平

有两个消息队列
1. 发送消息队列 xProtocolSendQueue 
  如果需要给上位机发送消息, 则将消息放入队列
  消息格式：int32_t, 表示当前小球位置
  
2. 接收消息队列 xProtocolRecvQueue
  串口接收到的数据，放入队列，由任务处理
  消息格式：uint8_t, 表示接收到的数据

**************************************/

float bytes_to_float(uint8_t* bytes) {
  union {
    float f;
    uint8_t b[4];
  } u;

  // float转换，MSB转LSB
  u.b[0] = bytes[3];
  u.b[1] = bytes[2];
  u.b[2] = bytes[1];
  u.b[3] = bytes[0];

  return u.f;
}

/***********************************
处理接收到的数据

格式：  帧头   命令位  长度       数据位       校验位    帧尾
-------------------------------------------------------------
Target：AA AA    F0    02     00 91             83       BB
    Kp：AA AA    E0    05     00 FF FF FF FF    XX       BB
    Ki：AA AA    E0    05     01 FF FF FF FF    XX       BB
    Kd：AA AA    E0    05     02 FF FF FF FF    XX       BB

需要将收到的数据，缓存到循环队列。不断解析数据

1. 丢弃不符合协议的数据（非帧头）
2. 直到找到完整数据包
3. 对数据包进行校验（数据长度，帧位，校验码）
************************************/

void print_arr(uint8_t* data, uint32_t len) {
  printf("recv[%d]->", len);
  for (int i = 0; i < len; i++) {
    printf("%02X ", data[i]);
  }
  printf("\n");
}

// 消息发送处理队列
xQueueHandle xProtocolSendQueue;
// 消息接收处理队列
xQueueHandle xProtocolRecvQueue;

void USART0_on_byte_recv(uint8_t data) {
  // Protocol_recv_byte(data);
  // Protocol_task();
  // 交给任务处理, 避免在中断里耗时
  xQueueSendFromISR(xProtocolRecvQueue, &data, NULL);
}

void USART0_on_recv(uint8_t* data, uint32_t len) {
  //  printf("recv[%d]:%s\n", len, data);
  // Protocol_recv_arr(data, len);
  // Protocol_task();
  // 交给任务处理, 避免在中断里耗时

}

// 拿到解析成功并校验通过的数据
void on_data_parsed(uint8_t* data, uint32_t len) {
  uint8_t cmd = data[0];

  if (cmd == 0xF0) {  // 如果cmd是0xF0，将1-2数据转为uint16_t 赋值target
    target = (int)((data[2] << 8) | data[3]);
  } else if (cmd == 0xE0) {
    uint8_t type = data[2];
    float val = bytesToFloat(&data[3], 1);
    switch (type) {
    case 0x00:
      kp = val;
      break;
    case 0x01:
      ki = val;
      break;
    case 0x02:
      kd = val;
      break;
    default:
      break;
    }
  }

  // 刷新OLED
  App_OLED_refresh(0);

//  printf(">>> p: %4.3f, i: %4.3f, d: %4.3f, target:%4dmm\r\n", 
//      kp, ki, kd, target);
}

/**********************************************************
 * @brief 按照协议发送当前小球位置给上位机
 * @param current: 当前小球位置mm

  0  1     2         3        4-5      6          7
  帧头   命令位    数据长度    数据    校验位      帧尾
-------- ------  --------   ------   ------   --------
 AA AA     B0       02       XX XX    XX         BB

校验位：ADD8, XOR8, CRC8, CRC16
校验和: check_arithmetic = ADD8(命令位 + 数据长度 + 数据)
 **********************************************************/
void serial_send_current(int current) {
  // uint8_t data[4];
  // data[0] = 0xB0;
  // uint32_t a ;
  // data[1] = (current >> 8) & 0xFF;
  // data[2] = current & 0xFF;
  // data[3] = '\n';
  // USART0_send_data(data, 4);
  uint8_t data[8];

  Protocol_gen_current(current, data, 8);

  USART0_send_data(data, 8);
}

typedef union {
  struct {
    float kp;
    float ki;
    float kd;
  } pid_data;

  uint8_t bytes_data[12];

} PID_t;
/**********************************************************
 * @brief 按照协议发送当前小球位置给上位机
 * @param current: 当前小球位置mm

  0  1     2         3        4-15    16         17
  帧头   命令位    数据长度    数据    校验位      帧尾
-------- ------  --------   ------   ------   --------
 AA AA     B1       0C       XX XX    XX         BB

校验位：ADD8, XOR8, CRC8, CRC16
校验和: check_arithmetic = ADD8(命令位 + 数据长度 + 数据)
 **********************************************************/
// 将3个float转成12个字节：联合体 union
void serial_send_param_pid(float kp, float ki, float kd) {
  PID_t pid = {
    .pid_data = {kp, ki, kd}
  };

  uint8_t data[18];
  Protocol_gen_pid(pid.bytes_data, data, 18);
  USART0_send_data(data, 18);
}


void App_Protocol_init() {
  Protocol_init();

  xProtocolSendQueue = xQueueCreate(10, sizeof(int));
  xProtocolRecvQueue = xQueueCreate(31, sizeof(uint8_t));
}

uint16_t cnt = 0;
void App_Protocol_task() {
  int pre_position = 0;
  int current_position = 0;
  while (1) {
    // 阻塞等待队列消息, 1000ms超时
    BaseType_t rst = xQueueReceive(xProtocolSendQueue, &current_position, pdMS_TO_TICKS(1000));
    if (rst == pdTRUE){
      // 发送当前小球位置  -------------------------------
      serial_send_current(current_position);
      pre_position = current_position;
    }else {
      // 发送小球最后的位置  -------------------------------
      serial_send_current(pre_position);
    }
  }
}
void App_Protocol_pid_task() {
  while (1) {    
    // 发送当前kp,ki,kd参数 2000ms -------------------------------
    serial_send_param_pid(kp, ki, kd);

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
void App_Protocol_recv_task(){
  uint8_t data;
  while(1){
    if(xQueueReceive(xProtocolRecvQueue, &data, portMAX_DELAY) == pdTRUE){
      Protocol_recv_byte(data);
      Protocol_task();
    }
  }
}
