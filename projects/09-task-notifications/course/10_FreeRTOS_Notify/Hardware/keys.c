#include "keys.h"


static void gpio_input_init(rcu_periph_enum periph, uint32_t port, uint32_t pin) {

  // 时钟初始化
  rcu_periph_clock_enable(periph);
  // 配置GPIO模式
  gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
}

void Keys_init() {

#ifdef KEY1_RCU
  gpio_input_init(KEY1_RCU, KEY1_GPIO);
#endif

#ifdef KEY2_RCU
  gpio_input_init(KEY2_RCU, KEY2_GPIO);
#endif

#ifdef KEY3_RCU
  gpio_input_init(KEY3_RCU, KEY3_GPIO);
#endif

#ifdef KEY4_RCU
  gpio_input_init(KEY4_RCU, KEY4_GPIO);
#endif
}

uint8_t states = 0xFF;

// 抬起1，按下0
#define IS_KEY_UP(pos)		((states & (1 << pos)) > 0)
#define IS_KEY_DOWN(pos)	((states & (1 << pos)) == 0)

#define SET_KEY_UP(pos)		(states |=  (1 << pos))
#define SET_KEY_DOWN(pos)	(states &= ~(1 << pos))

uint8_t Keys_is_key_down(uint8_t key){
	return IS_KEY_DOWN(key);
}
uint8_t Keys_is_key_up(uint8_t key){
	return IS_KEY_UP(key);
}

uint8_t Keys_scan() {

	FlagStatus state;
#ifdef KEY1_RCU
	
	state = gpio_input_bit_get(KEY1_GPIO);
	if(state == RESET && IS_KEY_UP(0)){
		// 按下
		SET_KEY_DOWN(0);
		Keys_on_key_down(0);
	}else if (state == SET && IS_KEY_DOWN(0)){
		// 抬起
		SET_KEY_UP(0);
		Keys_on_key_up(0);
	}
	
#endif

#ifdef KEY2_RCU
	state = gpio_input_bit_get(KEY2_GPIO);
	if(state == RESET && IS_KEY_UP(1)){
		// 按下
		SET_KEY_DOWN(1);
		Keys_on_key_down(1);
	}else if (state == SET && IS_KEY_DOWN(1)){
		// 抬起
		SET_KEY_UP(1);
		Keys_on_key_up(1);
	}
#endif

#ifdef KEY3_RCU
	state = gpio_input_bit_get(KEY3_GPIO);
	if(state == RESET && IS_KEY_UP(2)){
		// 按下
		SET_KEY_DOWN(2);
		Keys_on_key_down(2);
	}else if (state == SET && IS_KEY_DOWN(2)){
		// 抬起
		SET_KEY_UP(2);
		Keys_on_key_up(2);
	}
#endif

#ifdef KEY4_RCU
	state = gpio_input_bit_get(KEY4_GPIO);
	if(state == RESET && IS_KEY_UP(3)){
		// 按下
		SET_KEY_DOWN(3);
		Keys_on_key_down(3);
	}else if (state == SET && IS_KEY_DOWN(3)){
		// 抬起
		SET_KEY_UP(3);
		Keys_on_key_up(3);
	}
#endif
	
  return states;
}