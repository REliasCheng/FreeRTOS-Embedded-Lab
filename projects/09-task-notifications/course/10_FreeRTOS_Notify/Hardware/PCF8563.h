#ifndef __PCF8563_H__
#define __PCF8563_H__
#include "gd32f4xx.h"
#include "I2C0.h"

#ifndef u8
#define u8 uint8_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

// 启用设置为1	禁用设置为0
#define USE_ALARM	0
#define USE_TIMER	0

// 设备地址 7bit
#define		PCF8563_ADDR     0x51
// 存储地址：时间的存储地址开始位置
#define		PCF8563_REG_TD   0x02

// 控制寄存器2
#define		PCF8563_REG_CS2   0x01

// I2C写操作
#define 	I2C_WRITE(a, r, p, n)		I2C0_write(a, r, p, n)
// I2C读操作
#define 	I2C_READ(a, r, p, n)		I2C0_read(a, r, p, n)


// 秒、分、时、星期、日、月、年、世纪
// year = 2023, month = 2, day = 28, week = 4;
// hour = 23, minute = 59, second = 52;
typedef struct {

    u16 year;
    u8 month;
    u8 day;
    u8 week;
    u8 hour;
    u8 minute;
    u8 second;

} Clock_t;

// 警报、闹铃Alarm结构体
typedef struct {
    u8 minute;
    u8 hour;
    u8 day;
    u8 weekday;
} Alarm_t;

typedef enum {
	
	HZ4096 = 0,
	HZ64	 = 1,
	HZ1	 	 = 2,
	HZ1_60 = 3,
}TimerFreq;


void PCF8563_init();

void PCF8563_set_clock(Clock_t c);

void PCF8563_get_clock(Clock_t *c);

void PCF8563_set_alarm(Alarm_t alarm);
void PCF8563_enable_alarm();
void PCF8563_disable_alarm();
void PCF8563_alarm_clear_flag();

void PCF8563_set_timer(TimerFreq freq ,u8 countdown);
void PCF8563_enable_timer();
void PCF8563_disable_timer();
void PCF8563_timer_clear_flag();

#if USE_ALARM
extern void PCF8563_on_alarm();
#endif

#if USE_TIMER
extern void PCF8563_on_timer();
#endif


#endif