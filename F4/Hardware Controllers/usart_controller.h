#ifndef USART_CONTROLLER_H
#define USART_CONTROLLER_H

#include "defines.h"

#if defined(CYCLIC_BUFFER_H)
//#define USE_CYCLIC
	#include "cyclic_buffer.h"
#endif

struct UsartController_InitTypeDef
{
	USART_TypeDef*	usart;
	GPIO_TypeDef*	tx_gpio;
	GPIO_TypeDef*	rx_gpio;
	uint32_t		tx_pin;
	uint32_t		rx_pin;
	uint32_t		usart_af;
	DMA_TypeDef*	dma;
	uint32_t		dma_rx_channel;
	uint32_t		dma_tx_channel;
	uint32_t		dma_tx_stream;
	uint32_t		dma_rx_stream;
	uint32_t		baud;
	bool			use_dma_tx:1;
	bool			use_dma_rx:1;
	bool			use_rx_irq:1;
	uint16_t 		buffer_size;
};
//



class UsartController
{
	public:
		void Init(UsartController_InitTypeDef);
		void SetBaud(uint32_t baud);
		uint32_t GetBaud() {return usart.baud;}
		void SetBufferSize(uint16_t);
		void SetBufferPtr(uint8_t* ptr);
		void ClearBuffer();
		uint16_t Length();
		
		void Send(uint8_t*,uint16_t);
		void Send(const char*);
		void Send(uint8_t);
		void Reset();
		bool IsSent();
		uint16_t GetPointer();
		uint8_t GetChar(uint16_t ptr = 0);
		uint8_t Pull();
		void ShiftTail(uint16_t shft);
	
	private:
		void InitGPIO();
		void InitDMA(bool tx, bool rx);
		void InitUSART();
		void Transfer(uint16_t size);
		
		UsartController_InitTypeDef usart;
	
		#if defined(USE_CYCLIC)
			Cyclic_Buffer<uint8_t> received;
		#else
			uint8_t*	received;
			uint16_t tail;
		#endif
			uint8_t send_buffer[500];
		
};



#endif
