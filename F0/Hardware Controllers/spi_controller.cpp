#include "spi_controller.h"

SPIController::SPIController()
{
	
}
//

void SPIController::Init(SPI_Init spi)
{
	this->spi = spi;
	InitGPIO();
	InitSPI();
}
//

void SPIController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = spi.spi_af;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
	gpio.Pin = spi.miso_pin;
	LL_GPIO_Init(spi.miso_gpio,&gpio);
	
	gpio.Pin = spi.mosi_pin;
	LL_GPIO_Init(spi.mosi_gpio,&gpio);
	
	gpio.Pin = spi.sck_pin;
	LL_GPIO_Init(spi.sck_gpio,&gpio);
}
//

void SPIController::InitSPI()
{
	LL_SPI_InitTypeDef spi_ini;
	spi_ini.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128;
	spi_ini.BitOrder = LL_SPI_MSB_FIRST;
	spi_ini.ClockPhase = LL_SPI_PHASE_1EDGE;
	spi_ini.ClockPolarity = LL_SPI_POLARITY_LOW;
	spi_ini.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	spi_ini.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	spi_ini.Mode = LL_SPI_MODE_MASTER;
	spi_ini.NSS = LL_SPI_NSS_SOFT;
	spi_ini.TransferDirection = LL_SPI_FULL_DUPLEX;
	LL_SPI_Init(spi.spi,&spi_ini);
	LL_SPI_Enable(spi.spi);
	
	LL_SPI_SetRxFIFOThreshold(spi.spi,LL_SPI_RX_FIFO_TH_QUARTER);
	
}
//

void SPIController::Transmit(uint8_t byte)
{
	LL_SPI_TransmitData8(spi.spi,byte);
	while(!LL_SPI_IsActiveFlag_RXNE(spi.spi)) asm("NOP");
	LL_SPI_ReceiveData8(spi.spi);
}
//

uint8_t SPIController::Receive()
{
	
	LL_SPI_TransmitData8(spi.spi,0xFF);
	while(!LL_SPI_IsActiveFlag_RXNE(spi.spi)) asm("NOP");
	return LL_SPI_ReceiveData8(spi.spi);
}
//
