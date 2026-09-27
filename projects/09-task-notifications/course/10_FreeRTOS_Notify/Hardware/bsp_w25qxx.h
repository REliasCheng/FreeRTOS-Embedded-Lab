#ifndef _BSP_W25QXX_H__
#define _BSP_W25QXX_H__

#include "gd32f4xx.h"
#include "SPI.h"

#define CS_RCU		RCU_GPIOF
#define CS_PORT		GPIOF
#define CS_PIN		GPIO_PIN_6

#define CS_SELECT()			gpio_bit_write(CS_PORT, CS_PIN, RESET);
#define CS_UNSELECT()		gpio_bit_write(CS_PORT, CS_PIN, SET);

#define SPI_WR(dat)			SPI4_write_read(dat)

void W25QXX_init_config(void);

uint16_t W25QXX_readID(void);

void W25QXX_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);

void W25QXX_read(uint8_t* buffer, uint32_t read_addr,uint16_t read_length) ;

#endif
