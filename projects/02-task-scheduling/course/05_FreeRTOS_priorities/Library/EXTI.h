#ifndef __EXTI_H__
#define __EXTI_H__

#include "gd32f4xx.h"
#include "EXTI_config.h"

void EXTI_init();

extern void EXTI_0_on_trig();
extern void EXTI_1_on_trig();
extern void EXTI_5_on_trig();
extern void EXTI_6_on_trig();

#endif