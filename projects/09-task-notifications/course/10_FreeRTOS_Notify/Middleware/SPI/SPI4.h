#ifndef __SPI4_H__
#define __SPI4_H__

#include "gd32f4xx.h"
#include "SPI_config.h"

// ≥ı ºªØGPIO£¨ SPI
void SPI4_init();

// write
void SPI4_write(uint8_t dat);

// read
uint8_t SPI4_read();


// write read
uint8_t SPI4_write_read(uint8_t dat);

#endif