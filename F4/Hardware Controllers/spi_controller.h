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
	DMA_TypeDef*		dma;
	uint32_t			dma_chan;
	uint32_t			dma_tx;
	uint32_t			dma_rx;
};


class SpiController
{
	public:
		SpiController();
		~SpiController();
		void Init(SpiController_InitTypeDef, bool dma_en = false);
		uint8_t Transmit(uint8_t);
		void Transmit(uint8_t* data, uint8_t len);
		bool IsBusy();
		uint8_t  receive[128];
		
	
	private:
		void InitGPIO();
		void InitSPI();
		void InitDMA();
		void PreloadDMA(uint8_t len);
		SpiController_InitTypeDef core;
		bool dma_en;
	
		uint8_t transmit[128];
};

#endif
