#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H

#include "defines.h"


struct SpiController_InitTypeDef
{
	GPIO_TypeDef*		miso_gpio;
	GPIO_TypeDef*		mosi_gpio;
	GPIO_TypeDef*		sck_gpio;
	uint32_t 			mosi_pin;
	uint32_t 			miso_pin;
	uint32_t 			sck_pin;
	uint32_t			af_mode;
	SPI_TypeDef*		spi;
};


class SpiController
{
	public:
		SpiController();
		~SpiController();
		void Init(SpiController_InitTypeDef);
		uint8_t Transmit(uint8_t);
		
	
	private:
		void InitGPIO();
		void InitSPI();
		SpiController_InitTypeDef core;
};

#endif
