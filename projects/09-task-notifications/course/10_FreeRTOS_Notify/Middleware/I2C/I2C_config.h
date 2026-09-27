#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

#define USE_I2C0	1
#define USE_I2C1	0
#define USE_I2C2	0

/************************ I2C0 *******************/
#if USE_I2C0

// 软实现1， 硬实现0
#define I2C0_SOFT		0

// PB6,PB8	
#define I2C0_SCL_RCU		RCU_GPIOB
#define I2C0_SCL_PORT		GPIOB
#define I2C0_SCL_PIN		GPIO_PIN_6

// PB7,PB9
#define I2C0_SDA_RCU		RCU_GPIOB
#define I2C0_SDA_PORT		GPIOB
#define I2C0_SDA_PIN		GPIO_PIN_7

// 100k -> 5us
#define I2C0_SPEED	400000 
 

#endif


/************************ I2C1 *******************/
#if USE_I2C1

#endif

#endif