#include "bsp_buzzer.h"

//			   C	 D    E 	F	 G	 A	  B	   C`
//u16 hz[] = {523, 587, 659, 698, 784, 880, 988, 1047};

//            C			D			E			 F			G			A			B		  C`
u16 hz[] = { 1047, 1175, 1319,  1397, 1568, 1760, 1976, 2093 };


static void GPIO_config(
		rcu_periph_enum rcu_periph,
		uint32_t gpio_periph, uint32_t pin, 
    uint32_t alt_func_num 
	){

	// GPIO ------------------------------------------------- PA5
	rcu_periph_clock_enable(rcu_periph);
	// output
	gpio_mode_set(gpio_periph, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
	// output options
	gpio_output_options_set(gpio_periph, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
	// 设置引脚复用 (TIMER1_CH0)
	gpio_af_set(gpio_periph, alt_func_num, pin);
}
	
static void timer_init_config(uint32_t timer_periph, uint16_t t_prescaler, uint32_t t_period){
	// rcu
	timer_parameter_struct initpara;
	
	/* 初始化参数结构体 initialize TIMER init parameter struct */
	timer_struct_para_init(&initpara);
	// 预分频系数 (降低可用频率范围)
	initpara.prescaler         = t_prescaler - 1;
	// 1个周期的计数值 (16bit Max: 65535U)  10000Hz -> 24000
	// 频率值 > 3662.165		
	// 记忆方式：
	// 	10秒钟执行10000次：1秒钟执行1000次，1000Hz
	//  每数10个数，算1个。1秒钟执行1000次
	initpara.period            = t_period - 1;
	
	/* 初始化Timer initialize TIMER counter */
	timer_init(timer_periph, &initpara);
}

static void timer_channel_config(uint32_t timer_periph, uint16_t channel){
	timer_oc_parameter_struct ocpara;
	/* 初始化TIMER通道输出参数 initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&ocpara);
	// 启用输出通道(OP)
	ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
	/* 配置TIMER通道输出函数 configure TIMER channel output function */
	timer_channel_output_config(timer_periph, channel, &ocpara);
	/* 配置TIMER通道输出的比较模式 configure TIMER channel output compare mode */
	timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
	/* 配置TIMER通道输出脉冲值 (占空比) configure TIMER channel output pulse value */
	timer_channel_output_pulse_value_config(timer_periph, channel, 0);
}

void Buzzer_init(){
		// 初始化基本定时器 TIMER2 与引脚有关 
	// GPIO ------------------------------------------------
	GPIO_config(TIMER_GPIO);
	
	// TIMER ------------------------------------------------
	rcu_periph_clock_enable(TIMER_RCU);
	timer_deinit(TIMER_PERIPH);
	// 升级TIMER1频率 x4
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
	
	// TIMER 通道输出配置 ------------------------------------
	timer_channel_config(TIMER_PERIPH, TIMER_CH);

}

void Buzzer_beep(u8 hz_val_index){ // 1,2,3,4 ... 7
	
	u16 hz_val = hz[hz_val_index - 1];
	
	Buzzer_play(hz_val);
}

void Buzzer_play(u16 hz_val){
	
	uint32_t period = SystemCoreClock / (hz_val * PRESCALER);
	
	// 根据要播放的hz频率，设置PWM的PERIOD
	timer_init_config(TIMER_PERIPH, PRESCALER, period);
	
	/* enable a TIMER */
	timer_enable(TIMER_PERIPH);
	
	// 设置输出的脉冲值（占空比）范围是[0, PERIOD]
	timer_channel_output_pulse_value_config(TIMER_PERIPH, TIMER_CH, period * 0.5f);
}

void Buzzer_stop(){
	/* timer_disable a TIMER */
	timer_disable(TIMER_PERIPH);
}
