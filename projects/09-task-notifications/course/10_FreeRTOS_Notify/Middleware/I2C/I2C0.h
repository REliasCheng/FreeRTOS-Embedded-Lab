#ifndef __I2C0_H__
#define __I2C0_H__

#include "gd32f4xx.h"
#include "I2C_config.h"

typedef enum {

	// 成功，没有错误
	I2C_ERR_OK = 0,
	// 设备地址错误 1
	I2C_ERR_DEVICE_ADDR,
	// 寄存器地址错误 2
	I2C_ERR_REGISTER,
	// 写操作失败 3
	I2C_ERR_WRITE,
	// 读操作失败 4
	I2C_ERR_READ,
	
} I2C_ERROR_T;

// init gpio
void I2C0_init();

/**
I2C写数据 write
addr: 设备地址 7
reg:  寄存器地址
data: 要写的字节数组
len:  数组长度

return 0:写成功, 其他:失败
*/
uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);
uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len);

/**
I2C写数据 read
addr: 设备地址 7
reg:  寄存器地址
data: 要写的字节数组
len:  数组长度

return 0:读成功, 其他:失败
*/
uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);


#endif