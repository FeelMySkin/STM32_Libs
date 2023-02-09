#include "spi_controller.h"

SpiController::SpiController()
{
}
//

SpiController::~SpiController()
{
	
}
//

void SpiController::Init(SpiController_InitTypeDef spi, bool dma_en)
{
	this->core = spi;
	this->dma_en = dma_en;
	InitGPIO();
	InitSPI();
	InitDMA();
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
	
	if(dma_en)
	{
		LL_SPI_EnableDMAReq_RX(core.spi);
		LL_SPI_EnableDMAReq_TX(core.spi);
	}
	
	LL_SPI_Enable(core.spi);
	LL_SPI_SetNSSMode(SPI1,LL_SPI_NSS_SOFT);
}
//

void SpiController::InitDMA()
{
	if(dma_en)
	{
		//RX
		LL_DMA_InitTypeDef dma;
		dma.Channel = core.dma_chan;
		dma.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
		dma.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
		dma.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
		dma.MemBurst = LL_DMA_MBURST_SINGLE;
		dma.MemoryOrM2MDstAddress = (uint32_t)receive;
		dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
		dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
		dma.Mode = LL_DMA_MODE_NORMAL;
		dma.NbData = 0;
		dma.PeriphBurst = LL_DMA_PBURST_SINGLE;
		dma.PeriphOrM2MSrcAddress = (uint32_t)&(core.spi->DR);
		dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
		dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
		dma.Priority = LL_DMA_PRIORITY_MEDIUM;
		LL_DMA_Init(core.dma,core.dma_rx,&dma);
		//LL_DMA_EnableStream(core.dma,core.dma_rx);
		
		//TX
		dma.Channel = core.dma_chan;
		dma.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
		dma.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
		dma.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
		dma.MemBurst = LL_DMA_MBURST_SINGLE;
		dma.MemoryOrM2MDstAddress = (uint32_t)transmit;
		dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
		dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
		dma.Mode = LL_DMA_MODE_NORMAL;
		dma.NbData = 0;
		dma.PeriphBurst = LL_DMA_PBURST_SINGLE;
		dma.PeriphOrM2MSrcAddress = (uint32_t)&(core.spi->DR);
		dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
		dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
		dma.Priority = LL_DMA_PRIORITY_MEDIUM;
		LL_DMA_Init(core.dma,core.dma_tx,&dma);
		//LL_DMA_EnableStream(core.dma,core.dma_tx);
	}
}
//

bool SpiController::IsBusy()
{
	if(dma_en)
	{
		if(LL_DMA_GetDataLength(core.dma,core.dma_rx) != 0) return true;
		else return false;
	}
	else return !LL_SPI_IsActiveFlag_RXNE(core.spi);
}
//

uint8_t SpiController::Transmit(uint8_t byte)
{
	if(!dma_en)
	{
		LL_SPI_TransmitData8(core.spi,byte);
		while(IsBusy()) asm("NOP");
		return LL_SPI_ReceiveData8(core.spi);	
	}
	else
	{
		transmit[0] = byte;
		PreloadDMA(1);
		while(IsBusy()) asm("NOP");
		return receive[0];
	}
}
//

void SpiController::PreloadDMA(uint8_t len)
{
	LL_DMA_DisableStream(core.dma,core.dma_rx);
	LL_DMA_DisableStream(core.dma,core.dma_tx);
	ClearDmaTCFlag(core.dma,core.dma_rx);
	ClearDmaTCFlag(core.dma,core.dma_tx);
	
	LL_DMA_SetDataLength(core.dma,core.dma_rx,len);
	LL_DMA_SetDataLength(core.dma,core.dma_tx,len);
	LL_DMA_EnableStream(core.dma,core.dma_rx);
	LL_DMA_EnableStream(core.dma,core.dma_tx);
}
//


void SpiController::Transmit(uint8_t *data, uint8_t len)
{
	if(dma_en)
	{
		for(int i = 0;i<len;++i) transmit[i] = data[i];
		PreloadDMA(len);
		
	}
	else
	{
		for(int i = 0;i<len;++i)
		{
			receive[i] = Transmit(data[i]);
		}
	}
}
//


void SpiController::InitCS(GPIO_TypeDef* cs_gpio,uint32_t cs_pin)
{
	LL_GPIO_SetOutputPin(cs_gpio,cs_pin);
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = cs_pin;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
	gpio.Alternate = 0;
	LL_GPIO_Init(cs_gpio,&gpio);
	LL_GPIO_SetOutputPin(cs_gpio,cs_pin);
}
//
