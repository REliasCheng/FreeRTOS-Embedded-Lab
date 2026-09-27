#include "bsp_buzzer.h"
#include "TIMER.h"

u16 FREQS[] = { // [523, 7904]
	523 * 1, 587 * 1, 659 * 1, 698 * 1, 784 * 1, 880 * 1, 988 * 1, 
	523 * 2, 587 * 2, 659 * 2, 698 * 2, 784 * 2, 880 * 2, 988 * 2, 
	523 * 4, 587 * 4, 659 * 4, 698 * 4, 784 * 4, 880 * 4, 988 * 4, 
	523 * 8, 587 * 8, 659 * 8, 698 * 8, 784 * 8, 880 * 8, 988 * 8, 
};
//			 		 C`	   D`     E`   F`	   G`	   A`	   B`    C``
//u16 hz[] = {1047, 1175, 1319, 1397, 1568, 1760, 1976, 2093};

//			   		C	 	 D    E 	 F	  G	   A	  B	   C`
//u16 hz[] = {523, 587, 659, 698, 784, 880, 988, 1047};


// 初始化蜂鸣器 PB9 TIMER1CH1
void Buzzer_init(){
	
}

// 按照指定频率播放
void Buzzer_play(u16 hz_val){
//	/* 根据最新的频率计算并设置周期计数值 */
//	uint32_t period = SystemCoreClock / (PERSCALER * hz_val);
//	timer_init_config(PERSCALER, period);
	uint32_t period = SystemCoreClock / (TM1_PRESCALER * hz_val);
	TIMER_period_update(TIMER1, TM1_PRESCALER, period);
	// 再次启用
  timer_enable(TIMER1);
//  /* 根据最新的period配置通道输出的脉冲值（占空比）configure TIMER channel output pulse value */
	TIMER_channel_update(TIMER1, TIMER_CH_1, 60);
}

// 根据索引取出对应的音调
void Buzzer_beep(u16 idx){
	u16 hz_value;
	if(idx == 0){	// 不发音
		Buzzer_stop();		
		return;
	}
	
	hz_value = FREQS[idx - 1];
	Buzzer_play(hz_value);
}


// 停止播放
void Buzzer_stop(){
  /* disable a TIMER */
  timer_disable(TIMER1);
}