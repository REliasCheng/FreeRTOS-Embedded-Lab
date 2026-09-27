#include "SPI.h"

void SPI_init(){

#if USE_SPI0
	
	SPI0_init();
	
#endif

#if USE_SPI4
	
	SPI4_init();
	
#endif

	
	
}