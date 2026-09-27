#include "TIMER.h"

static void timer_gpio_config(
			uint32_t gpio_rcu, 
			uint32_t gpio_port, 
			uint32_t gpio_pin, 
			uint32_t gpio_af){
	rcu_periph_clock_enable(gpio_rcu);                                             
	/*output */                                                                    
	gpio_mode_set(gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_pin);              
	/* output mode */                                                              
	gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_pin);
	/* af */                                                                       
	gpio_af_set(gpio_port, gpio_af, gpio_pin);        
}

static void timer_init_config(rcu_periph_enum timer_rcu, uint32_t timer_periph, uint16_t t_prescaler, uint32_t t_period){
	// 初始化时钟
	rcu_periph_clock_enable(timer_rcu);
	
	timer_deinit(timer_periph);
	// rcu
	timer_parameter_struct initpara;
	
	/* 初始化参数结构体 initialize TIMER init parameter struct */
	timer_struct_para_init(&initpara);
	// 预分频系数 (降低可用频率范围)
	initpara.prescaler         = t_prescaler - 1;
	// 1个周期的计数值 (16bit Max: 65535U)  10000Hz -> 24000
	// 频率值 > 3662.165		
	initpara.period            = t_period - 1;
	
	/* 初始化Timer initialize TIMER counter */
	timer_init(timer_periph, &initpara);
	
	/* enable a TIMER */
	timer_enable(timer_periph);
}
static void timer_channel_config(uint32_t timer_periph, uint16_t channel, timer_oc_parameter_struct ocpara){
	/* 配置TIMER通道输出函数 configure TIMER channel output function */
	timer_channel_output_config(timer_periph, channel, &ocpara);
	/* 配置TIMER通道输出的比较模式 configure TIMER channel output compare mode */
	timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
}
void TIMER_init(){

	// 统一升级频率
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
	
	timer_oc_parameter_struct ocpara;
	
/****************************** TIMER_0 ******************************************/
#if USE_TIMER_0
	// 启用TIMER0
	timer_init_config(RCU_TIMER0, TIMER0, TM0_PRESCALER, TM0_PERIOD); // 与通道无关
	
	/* 初始化TIMER通道输出参数 initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&ocpara);
	// 配置TIMER0的所有通道 GPIO
	
#ifdef TM0_CH0
	timer_gpio_config(TM0_CH0);
	// 启用输出通道(OP)
	ocpara.outputstate  = TIMER_CCX_ENABLE;
	timer_channel_config(TIMER0, TIMER_CH_0, ocpara);
#endif
	
#ifdef TM0_CH0_ON
	timer_gpio_config(TM0_CH0_ON);
	// 启用输出通道(ON)
	ocpara.outputnstate = TIMER_CCXN_ENABLE;
	timer_channel_config(TIMER0, TIMER_CH_0, ocpara);
#endif

#endif
/****************************** TIMER_7 ******************************************/
#if USE_TIMER_7
	// 启用TIMER7
	timer_init_config(RCU_TIMER7, TIMER7, TM7_PRESCALER, TM7_PERIOD); // 与通道无关
	
	/* 初始化TIMER通道输出参数 initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&ocpara);
	// 配置TIMER0的所有通道 GPIO
	
#ifdef TM7_CH2
	timer_gpio_config(TM7_CH2);
	// 启用输出通道(OP)
	ocpara.outputstate  = TIMER_CCX_ENABLE;
	timer_channel_config(TIMER7, TIMER_CH_2, ocpara);
#endif
	
#ifdef TM7_CH2_ON
	timer_gpio_config(TM7_CH2_ON);
	// 启用输出通道(ON)
	ocpara.outputnstate = TIMER_CCXN_ENABLE;
	timer_channel_config(TIMER7, TIMER_CH_2, ocpara);
#endif

#endif


#if USE_TIMER_0 || USE_TIMER_7
	// Break Timer0/7---------------------------------------------------------------
	timer_break_parameter_struct breakpara;
	/* initialize TIMER break parameter struct */
	timer_break_struct_para_init(&breakpara);
	/* break输入极性 HIGH */
	breakpara.breakpolarity   = TIMER_BREAK_POLARITY_HIGH;
	/* 开启自动输出 */
	breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
	/* 启用Break */
	breakpara.breakstate      = TIMER_BREAK_ENABLE;
	
#if USE_TIMER_0
	/* 配置Break，configure TIMER break function */
	timer_break_config(TIMER0,  &breakpara);
	/* 使能TIMER0的break enable TIMER break function */
	timer_break_enable(TIMER0);
#endif
#if USE_TIMER_7
	/* 配置Break，configure TIMER break function */
	timer_break_config(TIMER7,  &breakpara);
	/* 使能TIMER0的break enable TIMER break function */
	timer_break_enable(TIMER7);
#endif

#endif
	
}

/**
	更新指定通道的PWM
*/
static void timer_channel_update(
	uint32_t period, uint32_t timer_periph, uint16_t channel, float duty){

	if (duty < 0) duty = 0;
	else if (duty > 100) duty = 100;
	
	// [0, PERIOD]
	uint32_t pulse = period * duty / 100 ;
	timer_channel_output_pulse_value_config(timer_periph, channel, pulse);
	
}


#if USE_TIMER_0
void TIMER_0_channel_update(uint16_t channel, float duty){
	timer_channel_update(TM0_PERIOD, TIMER0, channel, duty);
}
#endif

#if USE_TIMER_7
void TIMER_7_channel_update(uint16_t channel, float duty){
	timer_channel_update(TM7_PERIOD, TIMER7, channel, duty);
}
#endif