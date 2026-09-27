#include "bsp_buzzer.h"

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

#define TIMER_RCU		  RCU_TIMER1
#define TIMER_PERIPH	TIMER1
#define TIMER_CH			TIMER_CH_1
#define TIMER_IRQn		TIMER1_IRQn

#define PERSCALER			10			  // 预分频系数 [1-65536]
#define FREQ					1000		  // 频率
// 保证分母 (PERSCALER * FREQ) >= 2564 即可正常运行（PERIOD结果 <= 65536）
#define PERIOD				SystemCoreClock / (PERSCALER * FREQ)

// GPIO 输出引脚--------------------------------- 
static void timer_gpio_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin, uint32_t alt_func_num) {
  rcu_periph_clock_enable(rcu);
  gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // af复用设置
  gpio_af_set(port, alt_func_num, pin);
}

// Timer定时器 ---------------------------------
static void	timer_init_config(uint16_t t_perscaler, uint32_t t_period){
  timer_parameter_struct initpara;
  /* initialize TIMER counter */
  timer_struct_para_init(&initpara);

  /* 配置预分频系数, 可以实现更小的Timer频率。 */
  initpara.prescaler = t_perscaler - 1;
  /* 根据需要配置计数值 Max: 65535U */
  initpara.period = t_period - 1;
  /* initialize TIMER counter */
  timer_init(TIMER_PERIPH, &initpara);
  /* enable a TIMER */
  timer_enable(TIMER_PERIPH);
}

// 初始化蜂鸣器 PB9 TIMER1CH1
void Buzzer_init(){
	// GPIO --------------------------------- 初始化AF复用模式 PB9
	timer_gpio_config(RCU_GPIOB, GPIOB, GPIO_PIN_9, GPIO_AF_1);
	
	// TIMER ---------------------------------
  rcu_periph_clock_enable(TIMER_RCU);
  /* deinit a TIMER */
  timer_deinit(TIMER_PERIPH);
  /* 升级所有Timer相关的时钟频率 */
  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
	
	// CHANNEL输出通道配置    ---------------------------------
  timer_oc_parameter_struct ocpara;
  /* 初始化结构体参数 initialize TIMER channel output parameter struct */
  timer_channel_output_struct_para_init(&ocpara);
  /* 启用TIMER3_CH2的正极(OP) */
  ocpara.outputstate  = TIMER_CCX_ENABLE;
  /* 配置通道输出参数 configure TIMER channel output function */
  timer_channel_output_config(TIMER_PERIPH, TIMER_CH, &ocpara);
  /* 配置通道输出比较模式 configure TIMER channel output compare mode */
  timer_channel_output_mode_config(TIMER_PERIPH, TIMER_CH, TIMER_OC_MODE_PWM0);
  /* 配置通道输出的脉冲值（占空比）configure TIMER channel output pulse value */
//  timer_channel_output_pulse_value_config(TIMER_PERIPH, TIMER_CH, (PERIOD - 1) * 0.6f);
	
}

// 按照指定频率播放
void Buzzer_play(u16 hz_val){
	
	/* 根据最新的频率计算并设置周期计数值 */
	uint32_t period = SystemCoreClock / (PERSCALER * hz_val);
	timer_init_config(PERSCALER, period);
	
  /* 根据最新的period配置通道输出的脉冲值（占空比）configure TIMER channel output pulse value */
  timer_channel_output_pulse_value_config(TIMER_PERIPH, TIMER_CH, (uint32_t)((period - 1) * 0.6f));
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
  timer_disable(TIMER_PERIPH);
}