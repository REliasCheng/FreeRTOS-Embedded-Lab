#include "SPI0.h"

#if USE_SPI0

#if SPI0_SOFT
// 软实现

//SCL
#define SCL(bit) 		gpio_bit_write(SPI0_SCL_PORT, SPI0_SCL_PIN, bit ? SET : RESET)
//SDA
#define MOSI(bit) 	gpio_bit_write(SPI0_MOSI_PORT, SPI0_MOSI_PIN, bit ? SET : RESET)
//MISO
#define MISO() 			gpio_input_bit_get(SPI0_MISO_PORT, SPI0_MISO_PIN)

// 初始化GPIO， SPI
void SPI0_init() {
	
	// SCL
	rcu_periph_clock_enable(SPI0_SCL_RCU);
	gpio_mode_set(SPI0_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI0_SCL_PIN);
	gpio_output_options_set(SPI0_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_SCL_PIN);
	// MOSI
	rcu_periph_clock_enable(SPI0_MOSI_RCU);
	gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI0_MOSI_PIN);
	gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_MOSI_PIN);
	
	// MISO
	rcu_periph_clock_enable(SPI0_MISO_RCU);
	gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
	
	SCL(1);
	MOSI(1);
//	GPIO_SetBits(MISO_PORT, MISO_PIN);
}

// write
void SPI0_write(uint8_t dat) {
  // CPOL = 1  (默认高电平)
  // CPHA = 1  (先修改再采样)
  // 拉低SCL，1edge 修改数据（传输数据）
  // 拉高SCL，2edge 采样数据（从设备）

  uint8_t i;
  for(i=0; i<8; i++)
  {
    SCL(0);
    MOSI(dat&0x80);
    SCL(1);
    dat<<=1;
  }
}

// read
uint8_t SPI0_read() {
  uint8_t i,read=0;
  for(i=0; i<8; i++)
  {
    SCL(0);
    read<<=1;
    if(MISO()) read++;
    SCL(1);
  }
  return read;
}

#else
// 硬实现
// 初始化GPIO， SPI
void SPI0_init(){
	
	// GPIO ----------------------------------------------------------------------------
	// rcu
	
	// SCL
	rcu_periph_clock_enable(SPI0_SCL_RCU);
	gpio_mode_set(SPI0_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_SCL_PIN);
	gpio_output_options_set(SPI0_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_SCL_PIN);
	gpio_af_set(SPI0_SCL_PORT, GPIO_AF_5, SPI0_SCL_PIN);
	
	// MOSI
	rcu_periph_clock_enable(SPI0_MOSI_RCU);
	gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_MOSI_PIN);
	gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_MOSI_PIN);
	gpio_af_set(SPI0_MOSI_PORT, GPIO_AF_5, SPI0_MOSI_PIN);
	
	// MISO
	rcu_periph_clock_enable(SPI0_MISO_RCU);
	gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
	gpio_af_set(SPI0_MISO_PORT, GPIO_AF_5, SPI0_MISO_PIN);
	
	
	// SPI ----------------------------------------------------------------------------
	// rcu
	rcu_periph_clock_enable(RCU_SPI0);
	
	// 重置
	spi_i2s_deinit(SPI0);

	spi_parameter_struct spi_struct;
	/* initialize the parameters of SPI struct with default values */
	spi_struct_para_init(&spi_struct);
	/* configure the structure with default value */
	spi_struct.device_mode          = SPI_MASTER;
	spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_struct.nss                  = SPI_NSS_SOFT;
	spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; // 时钟极性和相位 (1, 1)
	// 分频系数 120MHz / 2 = 60MHz 速度配置取决于从设备. 
	// 从设备可能接受不过来，通常要慢点发
	spi_struct.prescale             = SPI_PSC_16;	// 值越小，速度越快。 >=8 >=16		
	spi_struct.endian               = SPI_ENDIAN_MSB;
	/* 初始化spi initialize SPI parameter */
  spi_init(SPI0, &spi_struct);
	/* 启用 enable SPI */
	spi_enable(SPI0);
}

// write
void SPI0_write(uint8_t dat){
	// 循环等待发送区为空, 值变为SET，结束循环
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	// 通知外设电路干活
	spi_i2s_data_transmit(SPI0, dat);
	
//	// 循环等待接收区不为空, 值变为SET，结束循环
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
	// 通知外设电路接收数据
	spi_i2s_data_receive(SPI0);
}

// read
uint8_t SPI0_read(){
	// 循环等待发送区为空, 值变为SET，结束循环
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	// 通知外设电路干活
	spi_i2s_data_transmit(SPI0, 0x00);
	
	// 循环等待接收区不为空, 值变为SET，结束循环
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
	// 通知外设电路接收数据
	return spi_i2s_data_receive(SPI0);
}
#endif



#endif