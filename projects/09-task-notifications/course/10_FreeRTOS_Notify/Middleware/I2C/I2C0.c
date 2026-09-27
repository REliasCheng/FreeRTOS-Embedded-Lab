#include "I2C0.h"
#include "systick.h"

#if I2C0_SOFT

// -----------------------------------------------------------------------------软实现

// 设置SCL电平状态
#define SCL(bit)		gpio_bit_write(I2C0_SCL_PORT, I2C0_SCL_PIN, bit == 1 ? SET : RESET);
// 设置SDA电平状态
#define SDA(bit)		gpio_bit_write(I2C0_SDA_PORT, I2C0_SDA_PIN, bit == 1 ? SET : RESET);

// 100k -> 5us
//#define i2cx_speed	100k, 400k
// 设置每个电平变化的延时(用于控制发送速度) 1bit/10us -> 5us   4bit/10us -> 2us
#if I2C0_SPEED < 200000
#define DELAY()			delay_1us(5);
#else
#define DELAY()			delay_1us(2);
#endif

// 主机获取SDA控制权
#define I2C0_SDA_OUT()		gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN)
// 主机释放SDA控制权
#define I2C0_SDA_IN()		gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN)
// 读取SDA电平状态
#define I2C0_SDA_STATE() gpio_input_bit_get(I2C0_SDA_PORT, I2C0_SDA_PIN)

static void start();
static void send(uint8_t data);
static uint8_t wait_ack();
static uint8_t recv();
static void send_nack();
static void send_ack();
static void stop();

// init gpio
void I2C0_init(){
	// 初始化GPIO
	// 修改寄存器：软实现，CPU实现
	// 使用AF复用：硬实现，外设电路实现
	
	// I2C传输速率：100Kbits/s  400Kbits/s  1M, 5M 比特
	
	// 100k/s => 100 000bit / 1000 000us => 1bit/10us	-> 高低电平5us
	// 400k/s => 400 000bit / 1000 000us => 4bit/10us	-> 高低电平2us
	
	// PB6 SCL
	// rcu
	rcu_periph_clock_enable(I2C0_SCL_RCU);
	// gpio
	gpio_mode_set(I2C0_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SCL_PIN);
	// gpio output
	gpio_output_options_set(I2C0_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C0_SCL_PIN);
	
	// PB7 SDA
	rcu_periph_clock_enable(I2C0_SDA_RCU);
	// gpio
	gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN);
	// gpio output
	gpio_output_options_set(I2C0_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C0_SDA_PIN);
}


/**
I2C写数据 write
return 0:写成功, 其他:失败
*/
uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	// 写地址是设备地址0x51左移1位 		0x51 -> 0xA2 write
	uint8_t write_addr = (addr << 1) | 0;
	
	// 开始
	start();
	
	// 发送设备写地址
	send(write_addr); // 0xA2 (8bit)
	// 等待响应
	if(wait_ack()){ return I2C_ERR_DEVICE_ADDR; }
	
	// 发送寄存器地址
	send(reg);				// 0x02
	// 等待响应
	if(wait_ack()){ return I2C_ERR_REGISTER; }
	
	for(uint32_t i = 0; i < len; i++){
		// 发送数据
		send(data[i]);
		// 等待响应
		if(wait_ack()){ return I2C_ERR_WRITE; }
	}
	
	// 停止
	stop();
	
	return I2C_ERR_OK;
}

/**
I2C写数据 write
return 0:写成功, 其他:失败
*/
uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len){
	// 写地址是设备地址0x51左移1位 		0x51 -> 0xA2 write
	uint8_t write_addr = (addr << 1) | 0;
	
	// 开始
	start();
	
	// 发送设备写地址
	send(write_addr); // 0xA2 (8bit)
	// 等待响应
	if(wait_ack()){ return I2C_ERR_DEVICE_ADDR; }
	
	// 发送寄存器地址
	send(reg);				// 0x02
	// 等待响应
	if(wait_ack()){ return I2C_ERR_REGISTER; }
	
	for(uint32_t i = 0; i < len; i++){
		// 发送数据
		send(data[i * offset]);
		// 等待响应
		if(wait_ack()){ return I2C_ERR_WRITE; }
	}
	
	// 停止
	stop();
	
	return I2C_ERR_OK;
}

/**
I2C写数据 read
return 0:读成功, 其他:失败
*/
uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	// 写地址是设备地址0x51左移1位 		0x51 -> 0xA2 write
	uint8_t write_addr = (addr << 1) | 0;
	// 读地址是设备地址0x51左移1位 + 1 0x51 -> 0xA3 read
	uint8_t read_addr  = (addr << 1) | 1;
	
	// 开始 -------------------------------------------
	start();
	
	// 发送设备地址(写地址)
	send(write_addr);
	// 等待响应
	if(wait_ack()){ return I2C_ERR_DEVICE_ADDR; }
	
	// 发送寄存器地址
	send(reg);				// 0x02
	// 等待响应
	if(wait_ack()){ return I2C_ERR_REGISTER; }
	
	// 开始 -------------------------------------------
	start();
	
	// 发送设备地址(读地址)
	send(read_addr);
	// 等待响应
	if(wait_ack()){ return I2C_ERR_DEVICE_ADDR; }
	
	/******** 接收数据 *********/
	for(uint32_t i = 0; i < len; i++){
		// 接收数据
		data[i] = recv();
		// 发送响应
		if( i == len - 1 ){
			// 最后一条，发送NACK
			send_nack();
		}else {
			// 非最后一条，发送ACK
			send_ack();
		}
	}
	/**************************/
	
	// 停止
	stop();
	
	return I2C_ERR_OK;
}

// --------------------------------------------------------------------
static void start(){
	// 获取SDA控制权
	I2C0_SDA_OUT();
	
	// SDA 拉高
	SDA(1);
	DELAY();
	// SCL 拉高
	SCL(1);
	DELAY();
	
	// SDA 拉低（在SCL为高电平时，产生下降沿）
	SDA(0);
	DELAY();
	// SCL 拉低
	SCL(0);
	DELAY();
}

static void send(uint8_t data){
	// 获取SDA控制权
	I2C0_SDA_OUT();
	// 写8bit数据 （先发高位）
	// 1101 0010
	// 1000 0000	0x80
	// data << 1
	
	// 101 0010
	// 1000 0000	0x80
	for(uint8_t i = 0; i < 8; i++){
		// SDA配置高低电平
		if (data & 0x80){
			SDA(1);
		}else {
			SDA(0);
		}
		DELAY();
		
		SCL(1);
		DELAY();
		SCL(0);
		DELAY();
		
		// 数据左移一位
		data <<= 1;
	}
	
}
static uint8_t wait_ack(){
	// 获取SDA控制权
	I2C0_SDA_OUT();
	// SDA拉高
	SDA(1);
	DELAY();
	
	// SCL拉高
	SCL(1);
	DELAY();
	
	// 将SDA设置位输入模式，释放SDA权限，等待从设备拉低SDA
	I2C0_SDA_IN();
	DELAY();
	
	// 检测SDA电平（如果从设备有响应，会被拉低）
	if(I2C0_SDA_STATE() == RESET){
		// ACK响应成功, SCL拉低
		SCL(0);
		// 收回控制权
		I2C0_SDA_OUT();
		
		DELAY();
		
	}else {
		// 收回控制权
		I2C0_SDA_OUT();
		
		// ACK响应失败
		stop();
		return 1;
	}
	
	return 0;
	
}
static void stop(){
	// 获取SDA控制权
	I2C0_SDA_OUT();
	
	// 将SDA和SCL拉低
	SDA(0);
	SCL(0);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉高SDA
	SDA(1);
	DELAY();
}

// 循环接收1个字节的8位数据
static uint8_t recv(){
	uint8_t i = 8;
	uint8_t data = 0x00;
	// 主机释放SDA控制权，进入输入模式
	I2C0_SDA_IN();
	// 1个字节中的高位先收到
	while(i--){ // 8, 7, 6, 5 .... 1
		// SCL拉低，等待从设备准备数据
		SCL(0);
		DELAY();
		
		SCL(1); // 设置数据有效性！！！
		// 0000 0000 -> 1011 1111
		
		// 0000 0000
		// 0000 0001 i==8
		// 0000 0010 i==7
		// 0000 0101 i==6
		// ...
		// 1011 1111 i==1
		
		data <<= 1;
		if(I2C0_SDA_STATE()) data++;
		
		// SCL在高电平等一会
		DELAY();
	}
	
	SCL(0);// 最后一次低电平
	
	return data;
}


static void send_nack(){
	// 主机发送NACK响应
	
	// 主机获取SDA控制权，进入输出模式
	I2C0_SDA_OUT();
	// 拉高SDA
	SDA(1);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉低SCL
	SCL(0);
	DELAY();

}
static void send_ack(){
	// 主机发送ACK响应
	
	// 主机获取SDA控制权，进入输出模式
	I2C0_SDA_OUT();
	// 拉低SDA
	SDA(0);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉低SCL
	SCL(0);
	DELAY();
	
}


#else

// -----------------------------------------------------------------------------硬实现


#define i2cx				I2C0
#define i2cx_rcu		RCU_I2C0
// init gpio
void I2C0_init(){
	// 初始化GPIO
	// 修改寄存器：软实现，CPU实现
	// 使用AF复用：硬实现，外设电路实现
	
	// I2C传输速率：100Kbits/s  400Kbits/s  1M, 5M 比特
	
	// 100k/s => 100 000bit / 1000 000us => 1bit/10us	-> 高低电平5us
	// 400k/s => 400 000bit / 1000 000us => 4bit/10us	-> 高低电平2us
	
	// GPIO --------------------------------------------------------------------------
	// PB6 SCL
	// rcu
	rcu_periph_clock_enable(I2C0_SCL_RCU);
	// gpio
	gpio_mode_set(I2C0_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C0_SCL_PIN);
	// gpio output
	gpio_output_options_set(I2C0_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C0_SCL_PIN);
	// af
	gpio_af_set(I2C0_SCL_PORT, GPIO_AF_4, I2C0_SCL_PIN);
	
	// PB7 SDA
	rcu_periph_clock_enable(I2C0_SDA_RCU);
	// gpio
	gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C0_SDA_PIN);
	// gpio output
	gpio_output_options_set(I2C0_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C0_SDA_PIN);
	// af
	gpio_af_set(I2C0_SDA_PORT, GPIO_AF_4, I2C0_SDA_PIN);
	
	// I2C0 ---------------------------------------------------------------------------
	rcu_periph_clock_enable(i2cx_rcu);
	
	/* 重置reset I2C */
	i2c_deinit(i2cx);
	/* 配置速度 configure I2C clock */
	i2c_clock_config(i2cx, I2C0_SPEED, I2C_DTCY_2);
	/* 启用 enable I2C */
	i2c_enable(i2cx);
	
	/* 自动处理ACK whether or not to send an ACK */
	i2c_ack_config(i2cx, I2C_ACK_ENABLE);
}

#define	TIMEOUT	50000

static uint8_t I2C_wait(uint32_t flag) {
    uint16_t cnt = 0;

    while(!i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
    return 0;
}

static uint8_t I2C_waitn(uint32_t flag) {
    uint16_t cnt = 0;

    while(i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
		return 0;
}

/**
I2C写数据 write
return 0:写成功, 其他:失败
*/
uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	// 写地址是设备地址0x51左移1位 		0x51 -> 0xA2 write
	uint8_t write_addr = (addr << 1) | 0;
	
	// 开始
	/************* start ***********************/
	// 等待I2C闲置  （默认SET）
	if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
	// start
	i2c_start_on_bus(i2cx);
	// 等待I2C主设备成功发送起始信号 （默认RESET）
	if(I2C_wait(I2C_FLAG_SBSEND)) return 2;
		
	// 发送设备写地址
	/************* device address **************/
	// 发送设备地址
	i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
	// 等待地址发送完成
	if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
	// 需要clear
	i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
	
	// 发送寄存器地址
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
	uint32_t i;
	for(i = 0; i < len; i++) {
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
	// 可以省略
	if(I2C_waitn(I2C_CTL0(i2cx)&I2C_CTL0_STOP)) return 8;
	
	return I2C_ERR_OK;
}

/**
I2C写数据 write
return 0:写成功, 其他:失败
*/
uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len){
	// 写地址是设备地址0x51左移1位 		0x51 -> 0xA2 write
	uint8_t write_addr = (addr << 1) | 0;
	
	// 开始
	/************* start ***********************/
	// 等待I2C闲置  （默认SET）
	if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
	// start
	i2c_start_on_bus(i2cx);
	// 等待I2C主设备成功发送起始信号 （默认RESET）
	if(I2C_wait(I2C_FLAG_SBSEND)) return 2;
		
	// 发送设备写地址
	/************* device address **************/
	// 发送设备地址
	i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
	// 等待地址发送完成
	if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
	// 需要clear
	i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
	
	// 发送寄存器地址
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
	uint32_t i;
	for(i = 0; i < len; i++) {
			uint32_t d = data[i * offset];
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
	// 可以省略
	if(I2C_waitn(I2C_CTL0(i2cx)&I2C_CTL0_STOP)) return 8;
	
	return I2C_ERR_OK;
}


/**
I2C写数据 read
return 0:读成功, 其他:失败
*/
uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	// 写地址是设备地址0x51左移1位 		0x51 -> 0xA2 write
	uint8_t write_addr = (addr << 1) | 0;
	// 读地址是设备地址0x51左移1位 + 1 0x51 -> 0xA3 read
	uint8_t read_addr  = (addr << 1) | 1;
	
	/************* start ***********************/
	// 等待I2C忙碌状态busy，直到空闲
	if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
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
	// 接收一个数据后，自动发送ACK，NACK
	i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);
	// 确认ACK已启用
	if(I2C_wait(I2C_CTL0(i2cx) & I2C_CTL0_ACKEN)) return 11;

	// 读取数据
	uint8_t i;
	for (i = 0; i < len; i++) {
			if(i != len - 1) {
					// 等待接收缓冲区, 等待ACK数据发送完成
					if(I2C_wait(I2C_FLAG_BTC)) return 9;
			}

			// 等待接收缓冲区出现数据
			if(I2C_wait(I2C_FLAG_RBNE)) return 10;
			
			data[i] = i2c_data_receive(i2cx);

			if (i == len - 2) {
					// 在读取最后一个字节之前，禁用ACK，并发送停止信号
					// 配置自动 NACK
					i2c_ack_config(i2cx, I2C_ACK_DISABLE);
			}
	}
	
	/***************** stop ********************/
	i2c_stop_on_bus(i2cx);
	if(I2C_waitn(I2C_CTL0(i2cx)&I2C_CTL0_STOP)) return 8;
	
	return I2C_ERR_OK;
}

#endif

