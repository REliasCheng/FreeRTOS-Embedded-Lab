#include "TIMER.h"

uint16_t timer0_period = 0;
uint16_t timer1_period = 0;
uint16_t timer2_period = 0;
uint16_t timer3_period = 0;
uint16_t timer4_period = 0;
uint16_t timer5_period = 0;
uint16_t timer6_period = 0;
uint16_t timer7_period = 0;

static void timer_save_period(uint32_t timer_periph, uint32_t t_period){
  switch(timer_periph){
    case TIMER0:  timer0_period = t_period; break;
    case TIMER1:  timer1_period = t_period; break;
    case TIMER2:  timer2_period = t_period; break;
    case TIMER3:  timer3_period = t_period; break;
    case TIMER4:  timer4_period = t_period; break;
    case TIMER5:  timer5_period = t_period; break;
    case TIMER6:  timer6_period = t_period; break;
    case TIMER7:  timer7_period = t_period; break;
    default: break;
  }
}


static void timer_gpio_config(uint32_t gpio_rcu, uint32_t gpio_port, uint32_t gpio_pin, uint32_t gpio_af) {
  rcu_periph_clock_enable(gpio_rcu);
  /* 设置gpio模式 */
  gpio_mode_set(gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, gpio_pin);
  gpio_output_options_set(gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, gpio_pin);
  gpio_af_set(gpio_port, gpio_af, gpio_pin);
}

static void timer_init_config(rcu_periph_enum rcu_periph, uint32_t timer_periph, uint16_t t_prescaler, uint32_t t_period) {
  rcu_periph_clock_enable(rcu_periph);
  timer_deinit(timer_periph);
  /*初始化参数 */
  timer_parameter_struct initpara;
  /* initialize TIMER init parameter struct */
  timer_struct_para_init(&initpara);
  /* 根据需要配置值 分频系数 （可以实现更低的timer频率） */
  initpara.prescaler 	= t_prescaler - 1;
  /* 1个周期的计数(period Max: 65535) Freq > 3662  */
  initpara.period		= t_period - 1;
  /* initialize TIMER counter */
  timer_init(timer_periph, &initpara);
  /* enable a TIMER */
  timer_enable(timer_periph);
  
  /* 保存下来，方便更新占空比使用*/
  timer_save_period(timer_periph, t_period);
}

static void timer_channel_config(uint32_t timer_periph, uint16_t channel, timer_oc_parameter_struct* ocpara) {
  /* 配置输出参数 configure TIMER channel output function */
  timer_channel_output_config(timer_periph, channel, ocpara);
  /* 配置通道输出输出比较模式 configure TIMER channel output compare mode */
  timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
}

void TIMER_init() {

  /* 升级频率*/
  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

  /* TIMER 通道输出配置 */
  timer_oc_parameter_struct ocpara;

// -----------------------------------------------------------------------TIMER_0
#if USE_TIMER_0

  timer_init_config(RCU_TIMER0, TIMER0, TM0_PRESCALER, TM0_PERIOD); // 与通道无关

  /* initialize TIMER channel output parameter struct */
  timer_channel_output_struct_para_init(&ocpara);

#ifdef	TM0_CH0
  timer_gpio_config(TM0_CH0);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_0, &ocpara);
#endif

#ifdef	TM0_CH0_ON
  timer_gpio_config(TM0_CH0_ON);
  ocpara.outputnstate  = (uint16_t)TIMER_CCXN_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_0, &ocpara);
#endif


#ifdef	TM0_CH1
  timer_gpio_config(TM0_CH1);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_1, &ocpara);
#endif

#ifdef	TM0_CH1_ON
  timer_gpio_config(TM0_CH1_ON);
  ocpara.outputnstate  = (uint16_t)TIMER_CCXN_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_1, &ocpara);
#endif


#ifdef	TM0_CH2
  timer_gpio_config(TM0_CH2);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_2, &ocpara);
#endif

#ifdef	TM0_CH2_ON
  timer_gpio_config(TM0_CH2_ON);
  ocpara.outputnstate  = (uint16_t)TIMER_CCXN_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_2, &ocpara);
#endif

#ifdef	TM0_CH3
  timer_gpio_config(TM0_CH3);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER0, TIMER_CH_3, &ocpara);
#endif


#endif

// -----------------------------------------------------------------------TIMER_1
#if USE_TIMER_1
  
  timer_init_config(RCU_TIMER1, TIMER1, TM1_PRESCALER, TM1_PERIOD); // 与通道无关

  /* initialize TIMER channel output parameter struct */
  timer_channel_output_struct_para_init(&ocpara);
#ifdef	TM1_CH0
  timer_gpio_config(TM1_CH0);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER1, TIMER_CH_0, &ocpara);
#endif

#ifdef	TM1_CH1
  timer_gpio_config(TM1_CH1);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER1, TIMER_CH_1, &ocpara);
#endif

#endif

// -----------------------------------------------------------------------TIMER_2
#if USE_TIMER_2

  timer_init_config(RCU_TIMER2, TIMER2, TM2_PRESCALER, TM2_PERIOD); // 与通道无关

  /* initialize TIMER channel output parameter struct */
  timer_channel_output_struct_para_init(&ocpara);
#ifdef	TM2_CH0
  timer_gpio_config(TM2_CH0);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER2, TIMER_CH_0, &ocpara);
#endif

#ifdef	TM2_CH1
  timer_gpio_config(TM2_CH1);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER2, TIMER_CH_1, &ocpara);
#endif

#ifdef	TM2_CH2
  timer_gpio_config(TM2_CH2);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER2, TIMER_CH_2, &ocpara);
#endif

#endif
// -----------------------------------------------------------------------TIMER_3
#if USE_TIMER_3

  timer_init_config(RCU_TIMER3, TIMER3, TM3_PRESCALER, TM3_PERIOD); // 与通道无关

  /* initialize TIMER channel output parameter struct */
  timer_channel_output_struct_para_init(&ocpara);
#ifdef	TM3_CH0
  timer_gpio_config(TM3_CH0);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER3, TIMER_CH_0, &ocpara);
#endif

#ifdef	TM3_CH1
  timer_gpio_config(TM3_CH1);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER3, TIMER_CH_1, &ocpara);
#endif

#ifdef	TM3_CH2
  timer_gpio_config(TM3_CH2);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER3, TIMER_CH_2, &ocpara);
#endif

#ifdef	TM3_CH3
  timer_gpio_config(TM3_CH3);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER3, TIMER_CH_3, &ocpara);
#endif

#endif

// -----------------------------------------------------------------------TIMER_7
#if USE_TIMER_7

  timer_init_config(RCU_TIMER7, TIMER7, TM7_PRESCALER, TM7_PERIOD); // 与通道无关

  /* initialize TIMER channel output parameter struct */
  timer_channel_output_struct_para_init(&ocpara);

#ifdef	TM7_CH0
  timer_gpio_config(TM7_CH0);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_0, &ocpara);
#endif

#ifdef	TM7_CH0_ON
  timer_gpio_config(TM7_CH0_ON);
  ocpara.outputnstate  = (uint16_t)TIMER_CCXN_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_0, &ocpara);
#endif


#ifdef	TM7_CH1
  timer_gpio_config(TM7_CH1);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_1, &ocpara);
#endif

#ifdef	TM7_CH1_ON
  timer_gpio_config(TM7_CH1_ON);
  ocpara.outputnstate  = (uint16_t)TIMER_CCXN_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_1, &ocpara);
#endif


#ifdef	TM7_CH2
  timer_gpio_config(TM7_CH2);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_2, &ocpara);
#endif

#ifdef	TM7_CH2_ON
  timer_gpio_config(TM7_CH2_ON);
  ocpara.outputnstate  = (uint16_t)TIMER_CCXN_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_2, &ocpara);
#endif


#ifdef	TM7_CH3
  timer_gpio_config(TM7_CH3);
  ocpara.outputstate  = (uint16_t)TIMER_CCX_ENABLE;
  timer_channel_config(TIMER7, TIMER_CH_3, &ocpara);
#endif

#endif


#if USE_TIMER_0 || USE_TIMER_7
  // Break --------------------------------------------------
  // break 只针对高级定时器TIMER0 & TIMER7，打开互补保护电路
  /* TIMER通道互补保护电路 */
  timer_break_parameter_struct breakpara;
  /* 初始化TIMER break参数结构体 */
  timer_break_struct_para_init(&breakpara);
  /* break输入的极性 HIGH */
  breakpara.breakpolarity   = TIMER_BREAK_POLARITY_HIGH;
  /* 输出自动的启用 */
  breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
  /* bread输入的启用*/
  breakpara.breakstate     = TIMER_BREAK_ENABLE;
#if USE_TIMER_0
  /* 配置TIMER0 break */
  timer_break_config(TIMER0, &breakpara);
  /* 启用TIMER0 break */
  timer_break_enable(TIMER0);
#endif

#if USE_TIMER_7
  /* 配置TIMER7 break */
  timer_break_config(TIMER7, &breakpara);
  /* 启用TIMER7 break */
  timer_break_enable(TIMER7);
#endif

#endif

}

void TIMER_period_update(uint32_t timer_periph, uint16_t t_prescaler, uint32_t t_period) {
  
  /*初始化参数 */
  timer_parameter_struct initpara;
  /* initialize TIMER init parameter struct */
  timer_struct_para_init(&initpara);
  /* 根据需要配置值 分频系数 （可以实现更低的timer频率） */
  initpara.prescaler 	= t_prescaler - 1;
  /* 1个周期的计数(period Max: 65535) Freq > 3662  */
  initpara.period		= t_period - 1;
  /* initialize TIMER counter */
  timer_init(timer_periph, &initpara);
  /* 保存下来，方便更新占空比使用*/
  timer_save_period(timer_periph, t_period);
}

/**********************************************************
 * @brief 更新指定Timer通道的duty占空比
 * @param duty: float[0, 100]
 * @return 
 **********************************************************/
int8_t TIMER_channel_update(uint32_t timer, uint16_t channel, float duty) {
  uint16_t period = 0;
  switch(timer){
    case TIMER0:  period = timer0_period; break;
    case TIMER1:  period = timer1_period; break;
    case TIMER2:  period = timer2_period; break;
    case TIMER3:  period = timer3_period; break;
    case TIMER4:  period = timer4_period; break;
    case TIMER5:  period = timer5_period; break;
    case TIMER6:  period = timer6_period; break;
    case TIMER7:  period = timer7_period; break;
    default: return 0;
  }
  
  if(duty > 100) duty = 100;
  else if(duty < 0) duty = 0;
  uint32_t pulse = period * duty / 100.0f;
  timer_channel_output_pulse_value_config(timer, channel, pulse);
  
  return 1;
}

