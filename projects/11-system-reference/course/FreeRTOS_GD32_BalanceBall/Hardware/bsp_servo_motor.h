#ifndef __BSP_SERVO_MOTOR_H__
#define __BSP_SERVO_MOTOR_H__

#include "gd32f4xx.h"
#include <math.h>

#define CLIP_VALUE(val, min, max) fmax(min, fmin(val, max))


void Servo_motor_init();

/**********************************************************
 * @brief ÉèÖÃ¶æ»ú½Ç¶È
 * @param angle: float -> [0, 180]
 **********************************************************/
void Servo_motor_set_angle(float angle);

float Servo_motor_get_angle(void);
#endif