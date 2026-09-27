#ifndef __I2C_H__
#define	__I2C_H__

#include "I2C_config.h"

#if USE_I2C0
#include "I2C0.h"
#endif

void I2C_init();


#endif