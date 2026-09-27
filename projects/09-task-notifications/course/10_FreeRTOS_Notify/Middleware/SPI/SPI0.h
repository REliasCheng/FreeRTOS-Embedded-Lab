#ifndef __SPI0_H__
#define __SPI0_H__

#include "gd32f4xx.h"
#include "SPI_config.h"

// ≥ı ºªØGPIO£¨ SPI
void SPI0_init();

// write
void SPI0_write(uint8_t dat);

// read
uint8_t SPI0_read();

#endif