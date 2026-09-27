#include "I2C0.h"

#if I2C0_SOFTWARE

#include "systick.h"

#define SDA(bit) 	gpio_bit_write(SDA_PORT, SDA_PIN, bit ? SET : RESET)
#define SCL(bit) 	gpio_bit_write(SCL_PORT, SCL_PIN, bit ? SET : RESET)

#if I2C_SPEED < 200000U
#define DELAY()  	delay_1us(5)
#else
#define DELAY()  	delay_1us(1)
#endif

// 设置SDA为输入模式
#define SDA_IN()	gpio_mode_set(SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SDA_PIN);
// 设置SDA为输出模式
#define SDA_OUT()	gpio_mode_set(SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN);
// 读取SDA电平
#define SDA_STATE() gpio_input_bit_get(SDA_PORT, SDA_PIN)

// SCL-PB6时钟线,开漏
// SDA-PB7数据线,开漏
void I2C0_init(){
	// 软实现：CPU操作GPIO
	// 硬实现：操作寄存器，AF复用，硬件电路实现数据收发
	
	// 标准模式 100Kbits/s， 快速模式 400Kbits/s
	// 100Kbits/s -> 100 000 bit / 1 000 000 us  -> 1bit/10us => 5us
	// 400Kbits/s -> 400 000 bit / 1 000 000 us  -> 4bit/10us => 1.+us
//	__NOP();
	// SCL-PB6 (默认低电平)
	rcu_periph_clock_enable(SCL_RCU);
	gpio_mode_set(SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SCL_PIN);
	gpio_output_options_set(SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SCL_PIN);
	
	// SDA-PB7 (默认低电平)
	rcu_periph_clock_enable(SDA_RCU);
	gpio_mode_set(SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN);
	gpio_output_options_set(SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SDA_PIN);
	
}

static void start();
static void send(uint8_t data);
static uint8_t recv();
static void send_ack();
static void send_nack();

static int8_t wait_ack();
static void stop();

/**********************************************************
 * @brief 写数据
 * @param addr: 设备地址 7bit
 * @param reg:  寄存器地址
 * @param data: 字节数组
 * @param len: 数据个数
 * @return 是否写成功0成功，其他失败
 **********************************************************/
int8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	
	// 开始
	start();
	
	// 发送设备地址（设备写地址）
	send((addr << 1) | 0);
	// 等待响应
	if(wait_ack() != 0) return 1;
	
	// 发送寄存器地址
	send(reg);
	// 等待响应
	if(wait_ack() != 0) return 2;
	
	// 循环(长度len)
	for(uint32_t i = 0; i < len; i++){
		// 写出1字节数据
		send(data[i]);
		// 等待响应
		if(wait_ack()) return 3;
	}
	
	// 停止
	stop();
	
	return 0;
}

/**********************************************************
 * @brief 读数据
 * @param addr: 设备地址 7bit
				举例： 0x51 -> 写0xA2, 读0xA3
			0x51 -> 0b01010001
			0xA2 ->  0b10100010		左移1位，末尾补0
			0xA3 ->  0b10100011		左移1位，末尾补1
 * @param reg:  寄存器地址
 * @param data: 字节数组
 * @param len:  数据个数
 * @return 是否读成功0成功，其他失败
 **********************************************************/
int8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	uint8_t addr_write = (addr << 1) | 0;	// 8bit设备的写地址 
	uint8_t addr_read  = (addr << 1) | 1;	// 8bit设备的读地址 
	
	// 开始 ---------------------------------- 通用开头
	start();
	
	// 发送设备写地址
	send(addr_write);
	// 等待响应
	if(wait_ack()) return 1;
	
	// 发送寄存器地址（要读的寄存器首地址）
	send(reg);
	// 等待响应
	if(wait_ack()) return 2;
	
	// 开始 ---------------------------------- 读取开头
	start();
	
	// 发送设备读地址
	send(addr_read);
	// 等待响应
	if(wait_ack()) return 3;
	
	/************** 循环接收数据 **************/
	for(uint32_t i = 0;i < len; i++){ // 一个循环接收一个字节
		// 接收一个字节
		data[i] = recv();
		
		if (i != len - 1){
			// 发送响应 (非最后一条)
			send_ack();			
		}else {
			// 发送空响应
			send_nack();
		}
		
	}
	/*****************************************/
	
	// 停止
	stop();
	return 0;
}

static void start(){
	
	// 将SDA和SCL拉高
	SDA(1);
	DELAY();
	SCL(1);
	DELAY();
	
	// 将SDA和SCL依次拉低
	SDA(0);
	DELAY();
	SCL(0);
	DELAY();
}


static void send(uint8_t data){
	// 发送8bit，先发高位
	// 1010 1000
	// 0101 0000
	// 101 00000
	
	for(uint8_t i = 0; i < 8; i++){
		// 取出最高位
		if(data & 0x80){
			SDA(1); // 高电平
		}else {
			SDA(0); // 低电平
		}
		DELAY();
		
		// 数据有效期
		SCL(1);
		DELAY();
		SCL(0);
		DELAY();
		
		// 内容向左移动一位
		data <<= 1;
	}
}

uint8_t recv(){
	// 释放SDA控制权，进入输入模式
	SDA_IN();
	uint8_t cnt = 8; 			// 1byte字节， 8bit比特
	uint8_t data = 0x00;	// 空字节容器接收数据
	
	while(cnt--){					// 接收一个bit(先收到的是高位)
		// SCL拉低
		SCL(0); // 等待从设备准备数据
		DELAY();
		
		SCL(1); // 设置数据有效性
		
		// 0000 0000 -> 1110 1011
		// 0000 0010
		
		// 0000 0001  8
		// 0000 0011  7
		// 0000 0111  6
		// 0000 1110  5
		// 0001 1101  4
		// 0011 1010  3
		// 0111 0101  2
		// 1110 1011  1
		
		// 整体左移
		data <<= 1;
		// 将最低位设置为1
//		if(SDA_STATE()) data |= 0x01;
		if(SDA_STATE()) data++;
		
		DELAY();
	}
	// 最后一次低电平，不能忘
	SCL(0);
	
	return data;
}

static void send_ack(){
	// 主机获取SDA控制权
	SDA_OUT();
	
	// 拉低SDA
	SDA(0);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉低SCL (从设备读取SDA上的电平)
	SCL(0);
	DELAY();
}
static void send_nack(){
	// 主机获取SDA控制权
	SDA_OUT();
	
	// 拉低SDA
	SDA(1);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉低SCL (从设备读取SDA上的电平)
	SCL(0);
	DELAY();

}

static int8_t wait_ack(){
	// 拉高SDA, 等待从设备拉低
	SDA(1);
	DELAY();
	
	// 拉高SCL，同时释放SDA权限（变成输入模式）
	SCL(1);
	SDA_IN();
	DELAY();
	
	if(SDA_STATE() == RESET){ // 低电平
		// 从设备拉低了SDA，应答成功
		SCL(0);
		// 获取控制权，变成输出模式
		SDA_OUT();
	}else {
		// 获取控制权，变成输出模式
		SDA_OUT();
		// 无人应答
		stop();
		return 1;
	}
	
	return 0;
}

static void stop(){
	// 获取控制权，变成输出模式
	SDA_OUT();
	
	// 将SCL和SDA拉低
	SCL(0);
	SDA(0);
	DELAY();
	
	SCL(1);
	DELAY();
	SDA(1);
	DELAY();
}

#else

#define I2C_RCU			RCU_I2C0
#define i2cx    		I2C0

// SCL-PB6时钟线,开漏
// SDA-PB7数据线,开漏
void I2C0_init(){
	// 软实现：CPU操作GPIO
	// 硬实现：操作寄存器，AF复用，硬件电路实现数据收发
	
	// 标准模式 100Kbits/s， 快速模式 400Kbits/s
	// 100Kbits/s -> 100 000 bit / 1 000 000 us  -> 1bit/10us => 5us
	// 400Kbits/s -> 400 000 bit / 1 000 000 us  -> 4bit/10us => 1.+us
	
	// GPIO --------------------------------------------
	// SCL-PB6 (默认低电平)
	rcu_periph_clock_enable(SCL_RCU);
	gpio_mode_set(SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SCL_PIN);
	gpio_output_options_set(SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SCL_PIN);
	gpio_af_set(SCL_PORT, GPIO_AF_4, SCL_PIN);
	
	// SDA-PB7 (默认低电平)
	rcu_periph_clock_enable(SDA_RCU);
	gpio_mode_set(SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SDA_PIN);
	gpio_output_options_set(SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SDA_PIN);
	gpio_af_set(SDA_PORT, GPIO_AF_4, SDA_PIN);
	
	// I2C ---------------------------------------------
	/* rcu */
	rcu_periph_clock_enable(I2C_RCU);
	/* reset I2C */
	i2c_deinit(i2cx);
	/* 配置I2C速率 configure I2C0 clock */
	i2c_clock_config(i2cx, I2C_SPEED, I2C_DTCY_2);
	/* 配置I2C地址格式和自己的地址 configure I2C0 address */
	i2c_mode_addr_config(i2cx, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
	/* 启用I2C enable I2C0 */
	i2c_enable(i2cx);
	/* 收到数据时，开启自动ACK回复功能 enable acknowledge */
	i2c_ack_config(i2cx, I2C_ACK_ENABLE);
}
#define	TIMEOUT	50000
static uint8_t I2C_wait(uint32_t flag) {
    uint16_t cnt = 0;

		// 如果是RESET一直等，直到SET
    while(!i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
    return 0;
}

static uint8_t I2C_waitn(uint32_t flag) {
    uint16_t cnt = 0;

		// 如果是SET一直等，直到RESET
    while(i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
		return 0; // 成功
}
/**********************************************************
 * @brief 写数据
 * @param addr: 设备地址 7bit
 * @param reg:  寄存器地址
 * @param data: 字节数组
 * @param len: 数据个数
 * @return 是否写成功0成功，其他失败
 **********************************************************/
int8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	uint8_t write_addr = addr << 1;
	
	/************* start ***********************/
	// 等待I2C闲置
	if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
	// start
	i2c_start_on_bus(i2cx);
	// 等待I2C主设备成功发送起始信号
	if(I2C_wait(I2C_FLAG_SBSEND)) return 2;
	
	/************* device address **************/
	// 发送设备地址
	i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
	// 等待地址发送完成
	if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
	i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
	
	/************ register address ************/
	// 寄存器地址
	// 等待发送数据缓冲区为空
	if(I2C_wait(I2C_FLAG_TBE)) return 4;
	// 发送数据
	i2c_data_transmit(i2cx, reg);
	// 等待数据发送完成
	if(I2C_wait(I2C_FLAG_BTC)) return 5;
	
	/***************** data ******************/
	// 发送数据
	for(uint32_t i = 0; i < len; i++) {
			uint32_t d = data[i];
			// 等待发送数据缓冲区为空
			if(I2C_wait(I2C_FLAG_TBE)) return 6;
			// 发送数据
			i2c_data_transmit(i2cx, d);
			// 等待数据发送完成
			if(I2C_wait(I2C_FLAG_BTC)) return 7;
	}
	/***************** stop ********************/
	// stop
	i2c_stop_on_bus(i2cx);

	if(I2C_waitn(I2C_CTL0(i2cx)&I2C_CTL0_STOP)) return 8;
	
	return 0;
}

/**********************************************************
 * @brief 读数据
 * @param addr: 设备地址 7bit
				举例： 0x51 -> 写0xA2, 读0xA3
			0x51 -> 0b01010001
			0xA2 ->  0b10100010		左移1位，末尾补0
			0xA3 ->  0b10100011		左移1位，末尾补1
 * @param reg:  寄存器地址
 * @param data: 字节数组
 * @param len:  数据个数
 * @return 是否读成功0成功，其他失败
 **********************************************************/
int8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	uint8_t write_addr = addr << 1;
	uint8_t read_addr  = (addr << 1) + 1;
	
	// 等待I2C空闲
	if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
	
	/************* start ***********************/
	// 发送启动信号
	i2c_start_on_bus(i2cx);
	// 等待I2C主设备成功发送起始信号
	if(I2C_wait(I2C_FLAG_SBSEND)) return 2;
	
	/************* device address **************/
	// 发送从设备地址
	i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);

	if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
	i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
	
	/********** register address **************/
	// 等待发送缓冲区	
	if(I2C_wait(I2C_FLAG_TBE)) return 4;
	// 发送寄存器地址
	i2c_data_transmit(i2cx, reg);
	// 等待发送数据完成	
	if(I2C_wait(I2C_FLAG_BTC)) return 5;
	
	/************* start ***********************/
	// 发送再启动信号
	i2c_start_on_bus(i2cx);
	if(I2C_wait(I2C_FLAG_SBSEND)) return 7;
	
	/************* device address **************/
	// 发送从设备地址
	i2c_master_addressing(i2cx, read_addr, I2C_RECEIVER);
	if(I2C_wait(I2C_FLAG_ADDSEND)) return 8;
	i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
	
	/************* data **************/
	// ack
	i2c_ack_config(i2cx, I2C_ACK_ENABLE);
	// 设置位置，接收一个数据后，自动发送ACK
	i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);
	// 确认ACK已启用
	if(I2C_wait(I2C_CTL0(i2cx) & I2C_CTL0_ACKEN)) return 11;

	// 读取数据
	for (uint32_t i = 0; i < len; i++) {
			if (i == len - 1) {
					// 在读取最后一个字节之前，禁用ACK，配置为自动 NACK
					i2c_ack_config(i2cx, I2C_ACK_DISABLE);
			}

			// 等待接收缓冲区不为空
			if(I2C_wait(I2C_FLAG_RBNE)) return 10;
			
			// 接收数据并放到data数组里
			data[i] = i2c_data_receive(i2cx);
	}
	
	/***************** stop ********************/
	i2c_stop_on_bus(i2cx);
	if(I2C_waitn(I2C_CTL0(i2cx)&I2C_CTL0_STOP)) return 12;
	
	return 0;
}

#endif