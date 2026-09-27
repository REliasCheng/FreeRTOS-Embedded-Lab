#include "PCF8563.h"
#include <stdio.h>

#define NUMBER_TD	7

void PCF8563_init() {

}

void PCF8563_set_clock(Clock_t c) {

    u8 p[NUMBER_TD];				// 用于存储时间字节数组
    u8 Cent; // 世纪：0代表本世纪20, 1代表下个世纪，年99->00时交替

    // 将数值转成一个字节表达 (BCD)

    // 秒：VL 1 1 1 - 0 0 0 0
    p[0] = ((c.second / 10) << 4) + (c.second % 10);

    // 分： X 1 1 1 - 0 0 0 0
    p[1] = ((c.minute / 10) << 4) + (c.minute % 10);

    // 时： X X 1 1 - 0 0 0 0
    p[2] = ((c.hour / 10) << 4) + (c.hour % 10);

    // 天： X X 1 1 - 0 0 0 0
    p[3] = ((c.day / 10) << 4) + (c.day % 10);

    // 周： X X X X - X 0 0 0
    p[4] = c.week;

    // 世纪 20xx , 21xx
    Cent = (c.year >= 2100 ? 1 : 0);
    // 月： C X X 1 - 0 0 0 0
    p[5] = (Cent << 7) + ((c.month / 10) << 4) + (c.month % 10);

    // 年： 1 1 1 1 - 0 0 0 0   2023, 2036
    p[6] = ((c.year % 100 / 10) << 4) + (c.year % 10);

    // 设置数据Write
    I2C_WRITE(PCF8563_ADDR, PCF8563_REG_TD, p, NUMBER_TD);
}

void PCF8563_get_clock(Clock_t *c) {

    u8 Cent; // 世纪：0代表本世纪20, 1代表下个世纪，年99->00时交替
    u8 p[NUMBER_TD];				// 用于存储时间字节数组
    // 循环读取Read: 秒、分、时、星期、日、月、年、世纪
    I2C_READ(PCF8563_ADDR, PCF8563_REG_TD, p, NUMBER_TD);

//			for(i = 0; i < NUMBER; i++){
//				printf("%d-> %d \n", (int)i, (int)p[i]);
//			}
//			printf("----------------\n");
    // 秒：VL 1 1 1 - 0 0 0 0 转成十进制
    c->second = ((p[0] >> 4) & 0x07) * 10 + (p[0] & 0x0F);

    // 分： X 1 1 1 - 0 0 0 0 转成十进制
    c->minute = ((p[1] >> 4) & 0x07) * 10 + (p[1] & 0x0F);

    // 时： X X 1 1 - 0 0 0 0 转成十进制
    c->hour	 = ((p[2] >> 4) & 0x03) * 10 + (p[2] & 0x0F);

    // 天： X X 1 1 - 0 0 0 0 转成十进制
    c->day 	 = ((p[3] >> 4) & 0x03) * 10 + (p[3] & 0x0F);

    // 周： X X X X - X 0 0 0 转成十进制
    c->week 	 = p[4] & 0x07;

    // 世纪
    // 月： C X X 1 - 0 0 0 0
    c->month	 = ((p[5] >> 4) & 0x01) * 10 + (p[5] & 0x0F);
    Cent 		 = p[5] >> 7;		// 0->20xx年 1->21xx年

    // 年： 1 1 1 1 - 0 0 0 0   1969, 2023
    c->year   = ((p[6] >> 4) & 0x0F) * 10 + (p[6] & 0x0F);
    c->year	+= Cent == 0 ? 2000 : 2100;
}


void PCF8563_set_alarm(Alarm_t alarm) {
    // 想将某个类型关闭掉，传一个负数
    // 或者多传1个字段，低4位，根据01决定是否启动对应类型 0000 0011

    u8 a[4];
//		a = 0;	// 分钟
//		I2C_WRITE(PCF8563_ADDR, 0x09, &a, 1);
//
//		a = 0;	// 小时
//		I2C_WRITE(PCF8563_ADDR, 0x0A, &a, 1);

    // 分 M	1 1 1 - 0 0 0 0			enable->0, diabled->0x80
    a[0] = ((alarm.minute / 10) << 4) + (alarm.minute % 10) + 0;

    // 时 H	x 1 1 - 0 0 0 0			enable->0, diabled->0x80
    a[1] = ((alarm.hour / 10) << 4) + (alarm.hour % 10) + 0;

    // 天 D	x 1 1 - 0 0 0 0			enable->0, diabled->0x80
    a[2] = ((alarm.day / 10) << 4) + (alarm.day % 10) + 0;

    // 周 W x x x - x 0 0 0			enable->0, diabled->0x80
    a[3] = alarm.weekday + 0;

    I2C_WRITE(PCF8563_ADDR, 0x09, a, 4);
}

void PCF8563_enable_alarm() {
    u8 cs2 = 0; // 控制状态寄存器2
    // 配置 cs2 寄存器以启用Alarm -------------------------------------------------
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // printf("cs2: %d \n", (int)cs2);
    // 清除Alarm标记，AF设置为0，下一次闹钟到点时，才能触发
    cs2 &= ~0x08;
    // 开启Alarm中断, AIE为1，启用Alarm闹钟
    cs2 |= 0x02;
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}
/**
 * @brief 配置 cs2 寄存器以启用Alarm
 *
    1.读取0x01到cs2
    2.清除Alarm标记0x08，AF设置为0，下一次闹钟到点时，才能触发
    3.开启Alarm中断0x02, AIE设置为0，禁用Alarm闹钟
    4.写回数据
 * \return * void
 */
void PCF8563_disable_alarm() {
    u8 cs2 = 0; // 控制状态寄存器2
    // 1.读取0x01到cs2
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 2.清除Alarm标记0x08，AF设置为0，下一次闹钟到点时，才能触发
    cs2 &= ~0x08;
    // 3.开启Alarm中断0x02, AIE设置为0，禁用Alarm闹钟
    cs2 &= ~0x02;
    // 4.写回数据
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}
void PCF8563_alarm_clear_flag() {

    u8 cs2 = 0; // 控制状态寄存器2
    // 01H寄存器中AF位，会在Alarm触发时，被置为1.
    // 必须手动置为0，下一个Alarm才能触发。

    // 配置 cs2 寄存器
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
//		printf("cs2: %d \n", (int)cs2);
    // 清除Alarm标记，AF设置为0，下一次闹钟到点时，才能触发
    cs2 &= ~0x08;
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

void PCF8563_set_timer(TimerFreq freq,u8 countdown) {
    u8 p = 0;
    // 设置Timer运行频率、启用timer source clock
    p = 0x80 + freq; // 64Hz
    I2C_WRITE(PCF8563_ADDR, 0x0E, &p, 1);

    // 设置Timer计数值(每个周期)
    p = countdown;
    I2C_WRITE(PCF8563_ADDR, 0x0F, &p, 1);

}

void PCF8563_enable_timer() {
    u8 cs2 = 0; // 控制状态寄存器2
    // 通过cs2启用Timer
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 开启Timer中断：	 TIE
    cs2 |=  0x01;
    // 清除Timer flag； TF
    cs2 &= ~0x04;
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

void PCF8563_disable_timer() {
    u8 cs2 = 0; // 控制状态寄存器2
    // 通过cs2启用Timer
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 开启Timer中断：	 TIE
    cs2 &= ~0x01;
    // 清除Timer flag； TF
    cs2 &= ~0x04;
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

void PCF8563_timer_clear_flag() {
    u8 cs2 = 0; // 控制状态寄存器2
    // 通过cs2启用Timer
    I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
    // 清除Timer flag； TF
    cs2 &= ~0x04;
    I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}


// 当中断触发时，此函数会执行（Alarm、Timer）
void ext_int3_call() {
    u8 cs2;
    I2C_READ(PCF8563_ADDR, PCF8563_REG_CS2, &cs2, 1);
//		printf("cs2: %d \n", (int) cs2);

#if USE_ALARM
    // Alarm Flag && AIE
    if((cs2 & 0x08) && (cs2 & 0x02)) {
        // 清除Alarm标记，避免下次无法触发
        PCF8563_alarm_clear_flag();
        // 事件触发
        PCF8563_on_alarm();
    }
#endif 
	
#if USE_TIMER
    // Timer Flag && TIE
    if((cs2 & 0x04) && (cs2 & 0x01)) {
        // 清除Timer标记，避免下次无法触发
        PCF8563_timer_clear_flag();
        // 事件触发
        PCF8563_on_timer();
    }
#endif 
}