#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H

#include "defines.h"

struct SPI_Init
{
	GPIO_TypeDef* miso_gpio;
	GPIO_TypeDef* mosi_gpio;
	GPIO_TypeDef* sck_gpio;
	uint32_t miso_pin;
	uint32_t mosi_pin;
	uint32_t sck_pin;
	uint32_t spi_miso_af;
	uint32_t spi_mosi_af;
	uint32_t spi_sck_af;
	SPI_TypeDef* spi;
	uint32_t	baudrate_prescaler; // LL_SPI_BAUDRATE_PRESCALER_*
};


class SPIController
{
	public:
		SPIController();
		void Init(SPI_Init);
		uint8_t Transmit(uint8_t byte);
	
	private:
		void InitGPIO();
		void InitSPI();
	
		SPI_Init spi;
		
};

#endif
