#include "Protocol.h"
#include "circular_queue.h"
#include <string.h>

/***********************
协议处理器：

1. Input:  用户收到数据
2. Output: 解析成功的结果返回给用户

***********************/
// 预定数据包最小和最大的个数
#define DATA_PACKAGE_LEN_MIN    6
#define DATA_PACKAGE_LEN_MAX    30

// 最大环形队列个数：最大包长度 * N + 1
#define BUFFER_MAX_LEN          91

#define FRAME_HEAD  0xAA // 帧头
#define FRAME_TAIL  0xBB // 帧尾

#define CMD_CURRENT 0xB0 // 当前小球位置
#define CMD_PID     0xB1 // pid参数


static QueueType_t g_recv_queue;
static uint8_t     g_recv_buffer[BUFFER_MAX_LEN];

uint8_t add8(uint8_t* data, uint32_t len) {
  uint8_t sum = 0x00;
  for(int i = 0; i < len; i++) {
    sum += data[i];
  }
  return sum & 0xFF;
}

uint8_t xor8(uint8_t* data, uint32_t len) {
  uint8_t sum = 0x00;
  for(int i = 0; i < len; i++) {
    sum ^= data[i];
  }
  return sum & 0xFF;
}

// 将float转换为字节数组
void floatToBytes(float f, uint8_t bytes[], uint8_t big) {
  FloatBytes fb;
  fb.floatValue = f;
  if(big){
    // 解析大端模式数据(高位在前)
    for (int i = 0; i < 4; i++) {
      bytes[i] = fb.bytesValue[3 - i];
    }
  } else {
    for (int i = 0; i < 4; i++) {
      bytes[i] = fb.bytesValue[i];
    }
  }
}

// 将字节数组转换为float
float bytesToFloat(uint8_t bytes[], uint8_t big) {
  FloatBytes fb;
  
  if(big){
    // 解析大端模式数据(高位在前)
    for (int i = 0; i < 4; i++) {
      fb.bytesValue[i] = bytes[3 - i];
    }
  }else {
    for (int i = 0; i < 4; i++) {
      fb.bytesValue[i] = bytes[i];
    }
  }
  return fb.floatValue;
}


void Protocol_init(){
  queue_init(&g_recv_queue, g_recv_buffer, BUFFER_MAX_LEN);
}

// 接收数据(一组)
void Protocol_recv_byte(uint8_t byte){
  queue_push(&g_recv_queue, byte);
}

// 接收数据(一组)
void Protocol_recv_arr(uint8_t* arr, uint32_t len){
  // 将收到的数据放到环形队列里
  queue_push_array(&g_recv_queue, arr, len);
}
/***********************************
处理接收到的数据

格式：  帧头   命令位  长度       数据位       校验位    帧尾
        0  1     2      3         4  
-------------------------------------------------------------
Target：AA AA    F0    02     00 91             83       BB
    Kp：AA AA    E0    05     00 FF FF FF FF    XX       BB
    Ki：AA AA    E0    05     01 FF FF FF FF    XX       BB
    Kd：AA AA    E0    05     02 FF FF FF FF    XX       BB
1. 丢弃不符合协议的数据（非帧头）
2. 直到找到完整数据包
3. 对数据包进行校验（数据长度，帧位，校验码）
************************************/

// 0  1     2      3     4  5               6       7
// AA AA    F0    02     00 91             83       BB
uint8_t read_buf[DATA_PACKAGE_LEN_MAX];
// 已经接收到的包字节个数
uint32_t g_recv_cnt = 0;

// 要接收的数据个数 （数据个数 + 6 == 包字节个数）
uint32_t g_recv_data_cnt = 0;

static void reset_recv_info(){
  g_recv_cnt = 0;
  g_recv_data_cnt = 0;
}

// 执行解析任务
void Protocol_task(){
  
  // 只要循环队列里有数据，就取出来解析
  while(queue_data_count(&g_recv_queue) > 0){
    // 将数据取一个到协议缓存池里, 更新位置，往下一个走
    if(queue_pop(&g_recv_queue, &read_buf[g_recv_cnt++]) != QUEUE_OK){
      // 如果没收到数据，继续
      continue;
    }
    
    // ==================== 内容判定 ======================
    // 判定0位是否是AA
    if(g_recv_cnt == 1 && read_buf[0] != FRAME_HEAD){
      // 继续找，重置个数
      reset_recv_info();
      continue;
    }
    // 判定1位是否是AA
    if(g_recv_cnt == 2 && read_buf[1] != FRAME_HEAD){
      // 继续找，重置个数
      reset_recv_info();
      continue;
    }
    if(g_recv_cnt == 3){
      // 命令位
    }
    
    // 记录要接收的数据长度
    if(g_recv_cnt == 4){
      g_recv_data_cnt = read_buf[3];
      // 要求不能超过最大包长度
      if(g_recv_data_cnt + 6 > DATA_PACKAGE_LEN_MAX){
        // 超出最大包长度，重置
        reset_recv_info();
        continue;
      }
    }
    
    // 判定数据个数是否符合目标（pack_len包字节数）
    int pack_len = g_recv_data_cnt + 6;
    if (g_recv_cnt < pack_len){
      // 数量不够，继续等
      continue;
    }
    
    // ===================== 数据校验 =====================
    // 检查帧尾是否是FRAME_TAIL
    if (read_buf[pack_len - 1] != FRAME_TAIL){
        reset_recv_info();
        continue;
    }
    
    // 检查校验码
    uint8_t expect_checksum = read_buf[pack_len - 2];
    uint8_t actual_checksum = add8(&read_buf[2], pack_len - 4);
    if(expect_checksum != actual_checksum){
      // 校验失败，重置
      printf("校验失败: excepted: %02X actual: %02X\n",
          expect_checksum, actual_checksum
      );
      reset_recv_info();
      continue;
    }
    
    
    // ===================== 校验通过 =====================
    printf("check passed[%d]->", pack_len);
    for(int i = 0; i < pack_len; i++) {
      printf("%02X ", read_buf[i]);
    }
    printf("\n");
    
    // 解析通过，把结果返回给调用者
    on_data_parsed(read_buf + 2, pack_len - 4);
    
    // 重置解析缓存
    reset_recv_info();
  }
  
}


// ---------------------------------------
void Protocol_gen_current(int current, uint8_t *data, uint32_t len){

  data[0] = FRAME_HEAD;
  data[1] = FRAME_HEAD;
  data[2] = CMD_CURRENT;
  data[3] = 0x02;
  // 将current放到data[4]开始的位置，2个字节
  data[4] = (current >> 8) & 0xFF;
  data[5] = current & 0xFF;
  data[6] = add8(&data[2], 4);
  data[7] = FRAME_TAIL;
}

void Protocol_gen_pid(uint8_t* bytes, uint8_t *data, uint32_t len){
  data[0] = FRAME_HEAD;
  data[1] = FRAME_HEAD;
  data[2] = CMD_PID;
  data[3] = 0x0C;
  // 将pid放到data[4]开始的位置，12个字节
  memcpy(&data[4], bytes, 12);
  data[16] = add8(&data[2], 14);
  data[17] = FRAME_TAIL;
}
  












