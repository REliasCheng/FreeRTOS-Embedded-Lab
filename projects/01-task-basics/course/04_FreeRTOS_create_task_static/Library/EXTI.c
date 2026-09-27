#include "EXTI.h"

static void EXTI_config(
		rcu_periph_enum rcu, uint32_t port, uint32_t pin, uint32_t pull_up_down,
		uint8_t exti_port, uint8_t exti_pin,
		exti_line_enum linex, exti_trig_type_enum trig_type,
		uint8_t nvic_irq, uint8_t nvic_irq_pre_priority,
                      uint8_t nvic_irq_sub_priority
){
		// GPIO -------------------------------------------
	rcu_periph_clock_enable(rcu);
	// 配置GPIO为输入模式
	gpio_mode_set(port, GPIO_MODE_INPUT, pull_up_down, pin);
	
	// EXTI -------------------------------------------
	rcu_periph_clock_enable(RCU_SYSCFG);
	// 把外部中断line连接到PA0 connect key EXTI line to key GPIO pin 
	syscfg_exti_line_config(exti_port, exti_pin);
	// 初始化并监听上升下降沿 
	exti_init(linex, EXTI_INTERRUPT, trig_type);
	
	// NVIC ------------------------------------------- 中断
	// 配置EXTI0优先级
	nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
	// 启用EXTI0的外部中断
	exti_interrupt_enable(linex);
	// 清理一次信号标记，避免无法触发，或者一上来就触发
	exti_interrupt_flag_clear(linex);
}

static void EXTI_config_soft(
		exti_line_enum linex, 
		uint8_t nvic_irq, uint8_t nvic_irq_pre_priority,
                      uint8_t nvic_irq_sub_priority
){
	
	// EXTI -------------------------------------------
	// 初始化并监听上升下降沿 
	exti_init(linex, EXTI_INTERRUPT, EXTI_TRIG_NONE);
	// NVIC ------------------------------------------- 中断
	// 配置EXTI0优先级
	nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
	// 启用EXTI0的外部中断
	exti_interrupt_enable(linex);
	// 清理一次信号标记，避免无法触发，或者一上来就触发
	exti_interrupt_flag_clear(linex);
}


void EXTI_init(){

#if USE_EXTI0
	// EXTI0
	EXTI_config(
		EXTI0_RCU, EXTI0_PORT, GPIO_PIN_0, EXTI0_PUPD, 			 // GPIO
		EXTI0_SOURCE, EXTI_SOURCE_PIN0, // syscfg line -> source
		EXTI_0, EXTI0_TRIG_TYPE,							 // EXTI
		EXTI0_IRQn, EXTI0_PRIORITY   									 // NVIC
	);
#endif
	
#if USE_EXTI1
	// EXTI1
	EXTI_config(
		EXTI1_RCU, EXTI1_PORT, GPIO_PIN_1, EXTI1_PUPD, 			 // GPIO
		EXTI1_SOURCE, EXTI_SOURCE_PIN1, // syscfg line -> source
		EXTI_1, EXTI1_TRIG_TYPE,							 // EXTI
		EXTI1_IRQn, EXTI1_PRIORITY   									 // NVIC
	);
#endif
	
#if USE_EXTI5
	// EXTI1
	EXTI_config_soft(
		EXTI_5,							 					// EXTI
		EXTI5_9_IRQn, EXTI5_PRIORITY  // NVIC
	);
#endif
	
}

#if USE_EXTI0
// 添加外部中断函数
void EXTI0_IRQHandler(){
	// 判断标记，清理标记
	if(SET == exti_interrupt_flag_get(EXTI_0)){
		exti_interrupt_flag_clear(EXTI_0);
		
		EXTI_0_on_trig();
	}
}
#endif

#if USE_EXTI1
// 添加外部中断函数
void EXTI1_IRQHandler(){
	// 判断标记，清理标记
	if(SET == exti_interrupt_flag_get(EXTI_1)){
		exti_interrupt_flag_clear(EXTI_1);
		
		EXTI_1_on_trig();
	}
}
#endif

void EXTI5_9_IRQHandler(){
#if USE_EXTI5
	if(SET == exti_interrupt_flag_get(EXTI_5)){
		exti_interrupt_flag_clear(EXTI_5);
		EXTI_5_on_trig();
	}
#endif
#if USE_EXTI6
	if(SET == exti_interrupt_flag_get(EXTI_6)){
		exti_interrupt_flag_clear(EXTI_6);
		EXTI_6_on_trig();
	}
#endif
}
