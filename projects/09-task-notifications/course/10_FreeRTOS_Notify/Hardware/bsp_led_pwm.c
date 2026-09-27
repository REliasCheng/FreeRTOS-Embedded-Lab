#include "bsp_led_pwm.h"

// PWM （确保 FREQ * PRESCALER > 3662)
#define	PRESCALER			1
#define FREQ					10000
#define PERIOD				SystemCoreClock / (FREQ * PRESCALER)


static void GPIO_config(){
	// PA3 
	rcu_periph_clock_enable(RCU_GPIOA);
	// output
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
	// output mode
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	
	// LED总开关拉低开启
	gpio_bit_reset(GPIOA, GPIO_PIN_3);
}

#define TIMER_GPIO_CONFIG(gpio_rcu, gpio_port, gpio_pin, gpio_af)	               \
	rcu_periph_clock_enable(gpio_rcu);                                             \
	/*output */                                                                    \
	gpio_mode_set(gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_pin);               \
	/* output mode */                                                              \
	gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_pin); \
	/* af */                                                                       \
	gpio_af_set(gpio_port, gpio_af, gpio_pin);                                      \


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

static void timer_channel_config(uint32_t timer_periph, uint16_t channel){
	timer_oc_parameter_struct ocpara;
	/* 初始化TIMER通道输出参数 initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&ocpara);
	// 启用输出通道(OP)
	ocpara.outputstate  = TIMER_CCX_ENABLE;
	// 启用输出通道(ON)
	ocpara.outputnstate = TIMER_CCXN_ENABLE;
	// 极性 OP
	ocpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
	// 极性 ON
	ocpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
	
	/* 配置TIMER通道输出函数 configure TIMER channel output function */
	timer_channel_output_config(timer_periph, channel, &ocpara);
	/* 配置TIMER通道输出的比较模式 configure TIMER channel output compare mode */
	timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
	/* 配置TIMER通道输出脉冲值 (占空比) configure TIMER channel output pulse value */
	timer_channel_output_pulse_value_config(timer_periph, channel, 0);
}


// LED1: TM0CH0: PA7 	AF1
// LED2: TM0CH0: PA8 	AF1
// LED3: TM7CH2: PB15 AF3
// LED4: TM7CH2: PC8 	AF3
// LED5: TM7CH1: PB14 AF3
// LED6: TM7CH1: PC7 	AF3
// LED7: TM7CH0: PA5 	AF3
// LED8: TM7CH0: PC6 	AF3
void LED_pwm_init(){

	GPIO_config();
	
	// GPIO -------------------------------------------------------------------
	TIMER_GPIO_CONFIG(RCU_GPIOA, GPIOA, GPIO_PIN_7, 	GPIO_AF_1);
	TIMER_GPIO_CONFIG(RCU_GPIOA, GPIOA, GPIO_PIN_8, 	GPIO_AF_1);
	TIMER_GPIO_CONFIG(RCU_GPIOB, GPIOB, GPIO_PIN_15, 	GPIO_AF_3);
	TIMER_GPIO_CONFIG(RCU_GPIOC, GPIOC, GPIO_PIN_8, 	GPIO_AF_3);
	TIMER_GPIO_CONFIG(RCU_GPIOB, GPIOB, GPIO_PIN_14, 	GPIO_AF_3);
	TIMER_GPIO_CONFIG(RCU_GPIOC, GPIOC, GPIO_PIN_7, 	GPIO_AF_3);
	TIMER_GPIO_CONFIG(RCU_GPIOA, GPIOA, GPIO_PIN_5, 	GPIO_AF_3);
	TIMER_GPIO_CONFIG(RCU_GPIOC, GPIOC, GPIO_PIN_6, 	GPIO_AF_3);
	
	
	// TIMER -------------------------------------------------------------------
	// 升级频率
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
	timer_init_config(RCU_TIMER0, TIMER0, PRESCALER, PERIOD); // 与通道无关
	timer_init_config(RCU_TIMER7, TIMER7, PRESCALER, PERIOD); // 与通道无关
	
	// CHANNEL------------------------------------------------------------------
	// timer
	timer_channel_config(TIMER_CH_LED1);
	timer_channel_config(TIMER_CH_LED3);
	timer_channel_config(TIMER_CH_LED5);
	timer_channel_config(TIMER_CH_LED7);
	
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
	/* 配置Break，configure TIMER break function */
	timer_break_config(TIMER0,  &breakpara);
	/* 使能TIMER0的break enable TIMER break function */
	timer_break_enable(TIMER0);
	
	/* configure TIMER break function */
	timer_break_config(TIMER7,  &breakpara);
	/* 使能TIMER7的break enable TIMER break function */
	timer_break_enable(TIMER7);
}

/**
duty: 占空比 [0.0, 100.0] -> [0, PERIOD]
*/
void LED_pwm_update(uint32_t timer_periph, uint16_t channel, float duty){

	if (duty < 0) duty = 0;
	else if (duty > 100) duty = 100;
	
	// [0, PERIOD]
	uint32_t pulse = PERIOD * duty / 100 ;
	timer_channel_output_pulse_value_config(timer_periph, channel, pulse);
	
}