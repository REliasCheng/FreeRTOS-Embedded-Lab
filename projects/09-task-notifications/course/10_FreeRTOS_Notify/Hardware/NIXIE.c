#include "NIXIE.h"
#include <stdio.h>

#define GET_BIT_VAL(byte, pos)	(byte & (1 << pos))

//#define NOP_TIME() NOP40()	// 用于看logic分析仪
#define NOP_TIME() __NOP();__NOP()

// 锁存操作 - 多行宏定义
#define RCK_ACTION() 	\
		NIXIE_RCK(0);		\
		NOP_TIME();				\
		NIXIE_RCK(1);		\
		NOP_TIME();

// 索引对应表格参见：
// https://www.yuque.com/icheima/stc8h/kmz2mllvxs1uvdfy#lLhhp
u8 LED_TABLE[] = 
{
	// 0 	1	 2	-> 9	(索引012...9)
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
	// 0. 1. 2. -> 9.	(索引10,11,12....19)
  0x64,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
	// . -						(索引20,21)
	0x7F, 0xBF,
	// AbCdEFHJLPqU		(索引22,23,24....33)
	0x88,0x83,0xC6,0xA1,0x86,0x8E,0x89,0xF1,0xC7,0x8C,0x98,0xC1
};

	
void NIXIE_init(){
	
	// NIXIE_DI
	GPIO_CONFIG(RCU_GPIOA, NIXIE_DI_PORT, NIXIE_DI_PIN);
	// NIXIE_SCK
	GPIO_CONFIG(RCU_GPIOC, NIXIE_SCK_PORT, NIXIE_SCK_PIN);
	// NIXIE_RCK
	GPIO_CONFIG(RCU_GPIOA, NIXIE_RCK_PORT,NIXIE_RCK_PIN );

}


static void NIXIE_out(u8 dat){
		// char 无符号, 需要勾选Plain Char is Signed 或使用 int8_t, signed char 
		int8_t i;
		// 8bit，先发出去的会作为高位
		for(i = 7; i >= 0; i--){            // 0点亮
			NIXIE_DI(GET_BIT_VAL(dat, i));
			
			// 寄存器的移位操作
			NIXIE_SCK(0);
			NOP_TIME(); // 休眠一会儿
			NIXIE_SCK(1);
			NOP_TIME(); // 休眠一会儿
		}
}

// 1234xxxx
// xx14xx3x
// 2023xxx4
//void NIXIE_display_str(char* string){
//}

// 每次只显示一个
// 	\arg num 对应数字在数组里的位置（索引）
//  \arg id 显示在指定位置(0 -> 7)
void NIXIE_display(u8 num, u8 id){
	u8 a_dat = LED_TABLE[num];	// 0001 0010	字母位
	u8 b_idx = 1 << id;					// 0010 0000	数字位 5
	
	NIXIE_show(a_dat, b_idx);
}

// 每次可以显示多个，但是内容都是一样的a_dat
// 		u8 a_dat = 0x12;	// 0001 0010	字母位
//		u8 b_idx = 0x1F;	// 0001 1111	数字位
void NIXIE_show(u8 a_dat, u8 b_idx){
	
		// 显示 7.
		// 0111 1000
		// 先发字母位 (控制显示的内容)// 0点亮
		// 8bit，先发出去的会作为高位
		NIXIE_out(a_dat);
	
		// 0,1,2,3....7
		// 再发数字位 （控制显示哪几个） // 只要不是0，就是高电平
		// 1111 1011
		// 7.7.空7. 7.7.7.7.  -------------------与二级制是反向
		// 8bit，先发出去的会作为高位
		NIXIE_out(b_idx);
		
		// 锁存操作
		RCK_ACTION();
		
}