#include "spi_controller.h"

SpiController::SpiController()
{
}
//

SpiController::~SpiController()
{
	
}
//

void SpiController::Init(SpiController_InitTypeDef spi)
{
	this->core = spi;
	InitGPIO();
	InitSPI();
}
//

void SpiController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = core.af_mode;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
		
	gpio.Pin = core.miso_pin;
	LL_GPIO_Init(core.miso_gpio,&gpio);
	
	gpio.Pin = core.mosi_pin;
	LL_GPIO_Init(core.mosi_gpio,&gpio);
	
	gpio.Pin = core.sck_pin;
	LL_GPIO_Init(core.sck_gpio,&gpio);
}
//

void SpiController::InitSPI()
{	
	

	LL_SPI_InitTypeDef spi_ini;
	spi_ini.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
	spi_ini.BitOrder = LL_SPI_MSB_FIRST;
	spi_ini.ClockPhase = LL_SPI_PHASE_1EDGE;
	spi_ini.ClockPolarity = LL_SPI_POLARITY_LOW;
	spi_ini.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	spi_ini.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	spi_ini.Mode = LL_SPI_MODE_MASTER;
	spi_ini.NSS = LL_SPI_NSS_SOFT;
	spi_ini.TransferDirection = LL_SPI_FULL_DUPLEX;
	LL_SPI_Init(core.spi,&spi_ini);
	
	LL_SPI_Enable(core.spi);
	LL_SPI_SetNSSMode(SPI1,LL_SPI_NSS_SOFT);
}
//

uint8_t SpiController::Transmit(uint8_t byte)
{
	LL_SPI_TransmitData8(core.spi,byte);
	while(!LL_SPI_IsActiveFlag_RXNE(core.spi)) asm("NOP");
	return LL_SPI_ReceiveData8(core.spi);	
}
//
