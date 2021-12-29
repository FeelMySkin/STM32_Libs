#include "usart_controller.h"

void UsartController::Init(UsartController_InitTypeDef ini)
{
	this->usart = ini;
	#if defined(USE_CYCLIC)
	
	#else
		this->received = new uint8_t[usart.buffer_size];
		tail = 0;
	#endif
	
	InitGPIO();
	InitUSART();
	InitDMA(true, true);
}
//

void UsartController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = usart.usart_af;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_NO;
	
	gpio.Pin = usart.rx_pin;
	LL_GPIO_Init(usart.rx_gpio,&gpio);
	
	gpio.Pin = usart.tx_pin;
	LL_GPIO_Init(usart.tx_gpio,&gpio);
}
//

void UsartController::InitUSART()
{
	LL_USART_Disable(usart.usart);
	LL_USART_InitTypeDef usrt;
	usrt.BaudRate = usart.baud;
	usrt.DataWidth = LL_USART_DATAWIDTH_8B;
	usrt.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	usrt.OverSampling = LL_USART_OVERSAMPLING_8;
	usrt.Parity = LL_USART_PARITY_NONE;
	usrt.StopBits = LL_USART_STOPBITS_1;
	usrt.TransferDirection = LL_USART_DIRECTION_TX_RX;
	LL_USART_Init(usart.usart,&usrt);
	
	
	LL_USART_EnableDMAReq_TX(usart.usart);
	LL_USART_EnableDMAReq_RX(usart.usart);
	
	/*if(usart.use_dma_rx || usart.use_dma_tx)
	{
		InitDMA();
		if(usart.use_dma_tx) LL_USART_EnableDMAReq_TX(usart.usart);
		if(usart.use_dma_rx) LL_USART_EnableDMAReq_RX(usart.usart);
	}
	
	if(usart.use_rx_irq)
	{
		LL_USART_EnableIT_RXNE(usart.usart);
		EnableUartIRQn(usart.usart,1);
	}*/
	
	LL_USART_Enable(usart.usart);
}
//

void UsartController::InitDMA(bool tx, bool rx)
{
	LL_DMA_InitTypeDef dma;
	//TX
	if(tx)
	{
		
		dma.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
		dma.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
		dma.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
		dma.MemBurst = LL_DMA_MBURST_SINGLE;
		dma.PeriphBurst = LL_DMA_PBURST_SINGLE;
		dma.Mode = LL_DMA_MODE_NORMAL;
		dma.PeriphOrM2MSrcAddress = (uint32_t)&(usart.usart->DR);
		dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
		dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
		dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
		dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;	
		dma.MemoryOrM2MDstAddress = (uint32_t)send_buffer;
		dma.NbData = 0;
		dma.Channel = usart.dma_channel;
		dma.Priority = LL_DMA_PRIORITY_MEDIUM;
		LL_DMA_Init(usart.dma,usart.dma_tx_stream,&dma);
		//LL_DMA_EnableStream(usart.dma,usart.dma_tx_stream);
	}
	
	//RX
	if(rx)
	{
		dma.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
		dma.FIFOMode = LL_DMA_FIFOMODE_DISABLE;		
		dma.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
		dma.MemBurst = LL_DMA_MBURST_SINGLE;
		dma.PeriphBurst = LL_DMA_PBURST_SINGLE;
		dma.Mode = LL_DMA_MODE_CIRCULAR;
		dma.PeriphOrM2MSrcAddress = (uint32_t)&usart.usart->DR;
		dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
		dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
		dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
		dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;	
		#if defined(USE_CYCLIC)
		dma.MemoryOrM2MDstAddress = (uint32_t) received.GetBufferPointer();
		#else
		dma.MemoryOrM2MDstAddress = (uint32_t)received;
		#endif
		dma.Channel = usart.dma_channel;
		dma.Priority = LL_DMA_PRIORITY_MEDIUM;
		dma.NbData = usart.buffer_size;
		LL_DMA_Init(usart.dma,usart.dma_rx_stream,&dma);
		LL_DMA_EnableStream(usart.dma,usart.dma_rx_stream);
	}
	
}
//

bool UsartController::IsSent()
{
	if(LL_DMA_GetDataLength(usart.dma,usart.dma_tx_stream) == 0) return true;
	else return false;
}
//

void UsartController::SetBufferSize(uint16_t sz)
{
	#if defined(USE_CYCLIC)
		received.SetSize(sz);
	#else
		usart.buffer_size = sz;
		LL_DMA_DisableStream(usart.dma,usart.dma_rx_stream);
		delete[] received;
		received = new uint8_t[usart.buffer_size];
		InitDMA(false, true);
	#endif
}
//

uint16_t UsartController::GetPointer()
{
	return usart.buffer_size - LL_DMA_GetDataLength(usart.dma,usart.dma_rx_stream);
}
//

void UsartController::SetBaud(uint32_t baud)
{
	usart.baud = baud;
	InitUSART();
}
//

uint16_t UsartController::Length()
{
	#if defined(USE_CYCLIC)
		return received.length();
	#else
		if(tail<=GetPointer()) return GetPointer()-tail;
		else
		{
			return (usart.buffer_size+GetPointer())-tail;
		}
	#endif
}
//

uint8_t UsartController::GetChar(uint16_t ptr)
{
	#if defined(USE_CYCLIC)
		return received.get(ptr);
	#else
		uint16_t buf_tail = tail;
		buf_tail = (buf_tail+ptr)%usart.buffer_size;
		return received[buf_tail];
	#endif
}
//

void UsartController::SetBufferPtr(uint8_t* ptr)
{
	#if defined(USE_CYCLIC)
		received.SetBufferPointer(ptr);
	#else
		LL_DMA_DisableStream(usart.dma, usart.dma_rx_stream);
		delete[] received; 
		received = ptr; 
		tail = 0;
	#endif
	InitDMA(false,true);
}
//

void UsartController::Send(const char* txt)
{
	for(uint16_t i = 0;i<strlen(txt);++i) send_buffer[i] = txt[i];
	Transfer(strlen(txt));
}
//

void UsartController::Send(uint8_t* ptr,uint16_t len)
{
	for(uint16_t i = 0;i<len;++i) send_buffer[i] = ptr[i];
	Transfer(len);
}
//

void UsartController::Send(uint8_t byte)
{
	send_buffer[0] = byte;
	Transfer(1);
}
//

void UsartController::Transfer(uint16_t len)
{
	LL_DMA_DisableStream(usart.dma,usart.dma_tx_stream);
	ClearDmaTCFlag(usart.dma,usart.dma_tx_stream);
	LL_DMA_SetDataLength(usart.dma,usart.dma_tx_stream,len);
	LL_DMA_EnableStream(usart.dma,usart.dma_tx_stream);
	
}
//

void UsartController::ClearBuffer()
{
	#if defined(USE_CYCLIC)
		received.clear();
	#else
		tail = GetPointer();
	#endif
}
//

uint8_t UsartController::Pull()
{
	if(tail == GetPointer()) return 0;
	uint8_t a = received[tail];
	tail++;
	tail%=usart.buffer_size;
	return a;
}
//

void UsartController::ShiftTail(uint16_t shft)
{
	if(tail==GetPointer()) return;
	else if(tail>GetPointer())
	{
		if(tail+shft<usart.buffer_size)
		{
			tail+=shft;
		}
		else if((tail+shft)%usart.buffer_size>GetPointer())
		{
			tail=GetPointer();
		}
		else
		{
			tail = (tail+shft)%usart.buffer_size;
		}
	}
	else if(tail<GetPointer())
	{
		if(tail+shft>GetPointer())
		{
			tail=GetPointer();
		}
		else tail+=shft;
	}
}
//
