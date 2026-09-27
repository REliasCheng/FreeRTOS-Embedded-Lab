#include "USART0.h"
#include <stdio.h>
#include <string.h>

#define USART0_DATA_ADDR					(uint32_t)&USART_DATA(USART0)

uint8_t 	g_rx_buffer[RX_BUFFER_LEN]; // \0
uint32_t 	g_rx_cnt = 0;


static void USART0_DMA_TX_config(){
	// 数据的发送，搬运方向：内存 -> 外设
	
	// 传输：动态内存 -> USART0外设 DMA1,CH7 子集100 -> USART0_TX
	// SRC: memory0_addr
	// DST: periph_addr   
	
  // RCU ---------------------------------------------
  rcu_periph_clock_enable(USART0_TX_DMA_RCU);
	
  // DMA ---------------------------------------------
  /* 重置DMA和通道 deinitialize DMA a channel registers */
  dma_deinit(USART0_TX_DMA_PERIPH_CH);

  dma_single_data_parameter_struct init_struct;
  /* 初始化结构体参数 initialize the DMA single data mode parameters struct with the default values */
  dma_single_data_para_struct_init(&init_struct);
  // 配置拷贝方向
  init_struct.direction           = DMA_MEMORY_TO_PERIPH;

  // 配置源头地址
	//  init_struct.memory0_addr        = (uint32_t)dst;
  init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;

  // 配置目标地址
  init_struct.periph_addr         = USART0_DATA_ADDR;
  init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;

  // 数据的个数，宽度（每个数据的BIT数）
	//  init_struct.number              = sizeof(src) / sizeof(src[0]);
  init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;

  // 循环模式（关闭）
  init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
  // 优先级
  init_struct.priority            = DMA_PRIORITY_LOW;

  /* 执行DMA的初始化 DMA single data mode initialize */
  dma_single_data_mode_init(USART0_TX_DMA_PERIPH_CH, &init_struct);
	
	/* 设置通道子集 100 DMA channel peripheral select */
  dma_channel_subperipheral_select(USART0_TX_DMA_PERIPH_CH, USART0_TX_DMA_CH_SUB);

}

static void USART0_DMA_RX_config(){

	// 数据的发送，搬运方向：外设 -> 内存
	
	// 传输：USART0外设 -> 固定内存
	
	// DMA1,CH2 子集100 -> USART0_RX
	
	// SRC: periph_addr	：USART0_DATA_ADDR
	// DST: memory0_addr：g_rx_buffer
	
  // RCU ---------------------------------------------
  rcu_periph_clock_enable(USART0_RX_DMA_RCU);
	
  // DMA ---------------------------------------------
  /* 重置DMA和通道 deinitialize DMA a channel registers */
  dma_deinit(USART0_RX_DMA_PERIPH_CH);

  dma_single_data_parameter_struct init_struct;
  /* 初始化结构体参数 initialize the DMA single data mode parameters struct with the default values */
  dma_single_data_para_struct_init(&init_struct);
  // 配置拷贝方向
  init_struct.direction           = DMA_PERIPH_TO_MEMORY;

  // 配置源头地址
  init_struct.periph_addr         = USART0_DATA_ADDR;
  init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;

  // 配置目标地址
	init_struct.memory0_addr        = (uint32_t)g_rx_buffer;
  init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;

  // 数据的个数，宽度（每个数据的BIT数）
	init_struct.number              = RX_BUFFER_LEN;
  init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;

  // 循环模式（关闭）
  init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
  // 优先级
  init_struct.priority            = DMA_PRIORITY_LOW;

  /* 执行DMA的初始化 DMA single data mode initialize */
  dma_single_data_mode_init(USART0_RX_DMA_PERIPH_CH, &init_struct);
	
	/* 设置通道子集 100 DMA channel peripheral select */
  dma_channel_subperipheral_select(USART0_RX_DMA_PERIPH_CH, USART0_RX_DMA_CH_SUB);
	
	/* 启动用DMA接收，通知DMA去USART0_DATA搬运数据 */
	dma_channel_enable(USART0_RX_DMA_PERIPH_CH);

}

void USART0_init() {
	
#if USART0_TX_DMA_ENABLE
	// 初始化DMA TX 发送---------------------------------------
	USART0_DMA_TX_config();
#endif

#if USART0_RX_DMA_ENABLE
	// 初始化DMA RX 接收---------------------------------------
	USART0_DMA_RX_config();
#endif
	
  // GPIO 初始化 -----------------------------------------TX PB6, RX PB7 -> AF

  // 启用GPIO时钟
  rcu_periph_clock_enable(USART0_TX_RCU);
  // 引脚模式配置 
  gpio_mode_set(USART0_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART0_TX_PIN);
  // 设置输出选项
  gpio_output_options_set(USART0_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART0_TX_PIN);
  // 设置AF复用功能
  gpio_af_set(USART0_TX_PORT, GPIO_AF_7, USART0_TX_PIN);

  rcu_periph_clock_enable(USART0_RX_RCU);
  // 引脚模式配置 
  gpio_mode_set(USART0_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART0_RX_PIN);
  // 设置输出选项
  gpio_output_options_set(USART0_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART0_RX_PIN);
  // 设置AF复用功能
  gpio_af_set(USART0_RX_PORT, GPIO_AF_7, USART0_RX_PIN);


  // USART 初始化 ----------------------------------------
  // 启用USART时钟
  rcu_periph_clock_enable(RCU_USART0);
  // 重置
  usart_deinit(USART0);
  // 配置串口参数：波特率*，数据位，校验位，停止位
  /* configure usart baud rate value 波特率 */
  usart_baudrate_set(USART0, USART0_BAUDRATE);
  /* configure usart word length 数据位长度 */
  usart_word_length_set(USART0, USART_WL_8BIT);
  /* configure usart parity function 校验位 */
  usart_parity_config(USART0, USART_PM_NONE);
  /* configure usart stop bit length 停止位*/
  usart_stop_bit_set(USART0, USART_STB_1BIT);
  /* 先发送高位还是低位 LSB低位先发*/
  usart_data_first_config(USART0, USART_MSBF_LSB);
		
  /* configure USART transmitter 启用发送功能 */
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	/* configure USART receiver 启用接收功能 */
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	
#if USART0_TX_DMA_ENABLE
	/* 启用DMA发送 configure USART DMA for transmission */
	usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
#endif

#if USART0_RX_DMA_ENABLE
	/* 启用DMA接收 configure USART DMA for reception */
	usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
#endif

	/* 配置中断优先级 */ 
	nvic_irq_enable(USART0_IRQn, 2, 2);
	
	/* 启用串口接收中断 */
	// 接收缓冲区不为空中断
	usart_interrupt_enable(USART0, USART_INT_RBNE);
	// 串口空闲中断
	usart_interrupt_enable(USART0, USART_INT_IDLE);

  /* enable usart 启用*/
  usart_enable(USART0);
	
//	printf("USART0_DATA_ADDR: 0x%X\n", USART0_DATA_ADDR); //0x40011004
}

// 发送一个字节数据
void USART0_send_data(uint8_t data) {
	// 传统：
  // 通过USART0发送
  usart_data_transmit(USART0, data);
  // 判断发送缓冲区是否为空 (不为空RESET则一直等待，直到为空SET)
//	FlagStatus status = usart_flag_get(USART0, USART_FLAG_TBE);
  while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	
}

// 发送多个字节
void USART0_send_array(uint8_t* data, uint32_t len) {
  // 传统：满足：1.data指针不为空 2. len长度不是0
  while(data && len--) {
    USART0_send_data(*data);
    data++;
  }
	
}


// 发送字符数组 （\0是字符串结束标记）
void USART0_send_string(char* data) {
  // 满足：1.data指针不为空 2. 要发送的数据不是\0
  while(data && *data) {
    USART0_send_data((uint8_t)(*data));
    data++;
  }
}

#if USART0_TX_DMA_ENABLE

// DMA发送一个字节数据
void USART0_dma_send_data(uint8_t data){
	// DMA：调用send_array
	USART0_send_array(&data, 1);
}
// DMA发送多个字节
void USART0_dma_send_array(uint8_t* data, uint32_t len) {
	// DMA: 设置数据源，设置数据个数，开始拷贝 (默认不阻塞)
		// 修改源地址(内存地址)
	dma_memory_address_config(USART0_TX_DMA_PERIPH_CH, DMA_MEMORY_0,(uint32_t) data);
	// 修改搬运个数
	dma_transfer_number_config(USART0_TX_DMA_PERIPH_CH, len);
	// 通知DMA开始拷贝
	dma_channel_enable(USART0_TX_DMA_PERIPH_CH);
	
	// 可以阻塞式地等待dma执行完成
	while(dma_flag_get(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF) == RESET);
	dma_flag_clear(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF);
}
// DMA发送字符数组 （\0是字符串结束标记）
void USART0_dma_send_string(char* data) {

	USART0_send_array((uint8_t*)data, strlen(data));
}
#endif

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
  USART0_send_data((uint8_t)ch);
  return ch;
}

/*********************************
中断函数：有中断事件，马上执行
1. 触发中断函数的原因（flag）有很多
2. 需要区分是哪个标记触发的中断
3. 通常需要手动清理标记
*********************************/
void USART0_IRQHandler(){
	
#if USART0_RX_DMA_ENABLE
	// 空闲中断（最后一个字节过一小会（us）就会执行）
	if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){
		// 只能通过再次接收一个数据，来清理标记
		usart_data_receive(USART0); // 必须读一次数据，读出来的数据不可用
		/* 获取剩余未传输的数据数量 get the number of remaining data to be transferred by the DMA */
		uint32_t remaining_cnt = dma_transfer_number_get(USART0_RX_DMA_PERIPH_CH);
		
//		printf("remaining_cnt: %d\n", remaining_cnt);
		g_rx_cnt = RX_BUFFER_LEN - remaining_cnt;

#if USART0_RECV_CALLBACK
		// 添加字符串结束标记，避免打印字符串出错
		g_rx_buffer[g_rx_cnt] = '\0';
		USART0_on_recv(g_rx_buffer, g_rx_cnt);
#endif
		
		// 停止搬运
		dma_channel_disable(USART0_RX_DMA_PERIPH_CH);
		// 清除标记FTF
		dma_flag_clear(USART0_RX_DMA_PERIPH_CH, DMA_FLAG_FTF);
		// 重启搬运
		dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
	}
#else
	// 接收缓冲区不为空中断标记（收到任何一个字节就执行）
	if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
		// 清理标记
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
//		printf("<RBNE>\n");
		// 获取寄存器里的数据
		uint8_t data = usart_data_receive(USART0);
//		send_data(data);
		// 把数据缓存到buffer
		g_rx_buffer[g_rx_cnt++] = data;
		
		// 避免缓冲区溢出（可选）
		if (g_rx_cnt >= RX_BUFFER_LEN) g_rx_cnt = 0;
	}
	
	// 空闲中断（最后一个字节过一小会（us）就会执行）
	if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){
//		printf("<IDLE>\n");
//		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE); // 无效方式
		// 只能通过再次接收一个数据，来清理标记
		usart_data_receive(USART0); // 必须读一次数据，读出来的数据不可用
		
#if USART0_RECV_CALLBACK
		// 添加字符串结束标记，避免打印字符串出错
		g_rx_buffer[g_rx_cnt] = '\0';
		USART0_on_recv(g_rx_buffer, g_rx_cnt);
#endif
		// 把缓冲区[0, g_rx_cnt)全部设置为0（可选）
//		memset(g_rx_buffer, 0, g_rx_cnt);		
		g_rx_cnt = 0; // 索引清零
	}
#endif
}
