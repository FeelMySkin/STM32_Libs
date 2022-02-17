#include "periph_functions.h"

void EnableExtiIRQn(uint32_t exti, uint8_t prio)
{
	switch(exti)
	{
		case LL_EXTI_LINE_0:
			NVIC_EnableIRQ(EXTI0_IRQn);
			NVIC_SetPriority(EXTI0_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_1:
			NVIC_EnableIRQ(EXTI1_IRQn);
			NVIC_SetPriority(EXTI1_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_2:
			NVIC_EnableIRQ(EXTI2_IRQn);
			NVIC_SetPriority(EXTI2_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_3:
			NVIC_EnableIRQ(EXTI3_IRQn);
			NVIC_SetPriority(EXTI3_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_4:
			NVIC_EnableIRQ(EXTI4_IRQn);
			NVIC_SetPriority(EXTI4_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_5:
		case LL_EXTI_LINE_6:
		case LL_EXTI_LINE_7:
		case LL_EXTI_LINE_8:
		case LL_EXTI_LINE_9:
			NVIC_EnableIRQ(EXTI9_5_IRQn);
			NVIC_SetPriority(EXTI9_5_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_10:
		case LL_EXTI_LINE_11:
		case LL_EXTI_LINE_12:
		case LL_EXTI_LINE_13:
		case LL_EXTI_LINE_14:
		case LL_EXTI_LINE_15:
			NVIC_EnableIRQ(EXTI15_10_IRQn);
			NVIC_SetPriority(EXTI15_10_IRQn,prio);
		break;
	}
}
//

void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line)
{
	uint16_t syscfg_exti_source;
	uint32_t syscfg_exti_line;
	
	if(gpio == GPIOA) syscfg_exti_source = LL_SYSCFG_EXTI_PORTA;
	else if(gpio == GPIOB) syscfg_exti_source = LL_SYSCFG_EXTI_PORTB;
	else if(gpio == GPIOC) syscfg_exti_source = LL_SYSCFG_EXTI_PORTC;
	else if(gpio == GPIOD) syscfg_exti_source = LL_SYSCFG_EXTI_PORTD;
	else if(gpio == GPIOE) syscfg_exti_source = LL_SYSCFG_EXTI_PORTE;
	
	switch(exti_line)
	{
		case LL_EXTI_LINE_0:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE0;
		break;
		
		case LL_EXTI_LINE_1:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE1;
		break;
		
		case LL_EXTI_LINE_2:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE2;
		break;
		
		case LL_EXTI_LINE_3:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE3;
		break;
		
		case LL_EXTI_LINE_4:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE4;
		break;
		
		case LL_EXTI_LINE_5:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE5;
		break;
		
		case LL_EXTI_LINE_6:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE6;
		break;
		
		case LL_EXTI_LINE_7:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE7;
		break;
		
		case LL_EXTI_LINE_8:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE8;
		break;
		
		case LL_EXTI_LINE_9:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE9;
		break;
		
		case LL_EXTI_LINE_10:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE10;
		break;
		
		case LL_EXTI_LINE_11:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE11;
		break;
		
		case LL_EXTI_LINE_12:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE12;
		break;
		
		case LL_EXTI_LINE_13:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE13;
		break;
		
		case LL_EXTI_LINE_14:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE14;
		break;
		
		case LL_EXTI_LINE_15:
			syscfg_exti_line = LL_SYSCFG_EXTI_LINE15;
		break;
	}
	
	LL_SYSCFG_SetEXTISource(syscfg_exti_source,syscfg_exti_line);
}
//

uint32_t GetExtiLine(uint32_t pin)
{
	switch(pin)
	{
		case LL_GPIO_PIN_0:
			return LL_EXTI_LINE_0;
		
		case LL_GPIO_PIN_1:
			return LL_EXTI_LINE_1;
		
		case LL_GPIO_PIN_2:
			return LL_EXTI_LINE_2;
		
		case LL_GPIO_PIN_3:
			return LL_EXTI_LINE_3;
		
		case LL_GPIO_PIN_4:
			return LL_EXTI_LINE_4;
		
		case LL_GPIO_PIN_5:
			return LL_EXTI_LINE_5;
		
		case LL_GPIO_PIN_6:
			return LL_EXTI_LINE_6;
		
		case LL_GPIO_PIN_7:
			return LL_EXTI_LINE_7;
		
		case LL_GPIO_PIN_8:
			return LL_EXTI_LINE_8;
		
		case LL_GPIO_PIN_9:
			return LL_EXTI_LINE_9;
		
		case LL_GPIO_PIN_10:
			return LL_EXTI_LINE_10;
		
		case LL_GPIO_PIN_11:
			return LL_EXTI_LINE_11;
		
		case LL_GPIO_PIN_12:
			return LL_EXTI_LINE_12;
		
		case LL_GPIO_PIN_13:
			return LL_EXTI_LINE_13;
		
		case LL_GPIO_PIN_14:
			return LL_EXTI_LINE_14;
		
		case LL_GPIO_PIN_15:
			return LL_EXTI_LINE_15;
	}
	
	return 0;
}
//

void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority)
{
	if(tim == TIM1 || tim == TIM10)
	{
		NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
		NVIC_SetPriority(TIM1_UP_TIM10_IRQn,priority);
	}
	else if(tim == TIM2)
	{
		NVIC_EnableIRQ(TIM2_IRQn);
		NVIC_SetPriority(TIM2_IRQn,priority);
	}
	else if(tim == TIM3)
	{
		NVIC_EnableIRQ(TIM3_IRQn);
		NVIC_SetPriority(TIM3_IRQn,priority);
	}
	else if(tim == TIM4)
	{
		NVIC_EnableIRQ(TIM4_IRQn);
		NVIC_SetPriority(TIM4_IRQn,priority);
	}
	else if(tim == TIM5)
	{
		NVIC_EnableIRQ(TIM5_IRQn);
		NVIC_SetPriority(TIM5_IRQn,priority);
	}
	#ifdef TIM8
	else if(tim == TIM8)
	{
		NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
		NVIC_SetPriority(TIM8_UP_TIM13_IRQn,priority);
	}
	#endif
	else if(tim == TIM9)
	{
		NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
		NVIC_SetPriority(TIM1_BRK_TIM9_IRQn,priority);
	}
	else if(tim == TIM11)
	{
		NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
		NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn,priority);
	}

}
//

void EnableUartIRQn(USART_TypeDef* uart, uint8_t priority)
{
	if(uart == USART1)
	{
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn,priority);
	}
	if(uart == USART2)
	{
		NVIC_EnableIRQ(USART2_IRQn);
		NVIC_SetPriority(USART2_IRQn,priority);
	}
	if(uart == USART6)
	{
		NVIC_EnableIRQ(USART6_IRQn);
		NVIC_SetPriority(USART6_IRQn,priority);
	}
}
//

void DisableExtiIRQn(uint32_t exti_line)
{
	switch(exti_line)
	{
		case LL_EXTI_LINE_0:
			NVIC_DisableIRQ(EXTI0_IRQn);
		break;
		
		case LL_EXTI_LINE_1:
			NVIC_DisableIRQ(EXTI1_IRQn);
		break;
		
		case LL_EXTI_LINE_2:
			NVIC_DisableIRQ(EXTI2_IRQn);
		break;
		
		case LL_EXTI_LINE_3:
			NVIC_DisableIRQ(EXTI3_IRQn);
		break;
		
		case LL_EXTI_LINE_4:
			NVIC_DisableIRQ(EXTI4_IRQn);
		break;
		
		case LL_EXTI_LINE_5:
		case LL_EXTI_LINE_6:
		case LL_EXTI_LINE_7:
		case LL_EXTI_LINE_8:
		case LL_EXTI_LINE_9:
			NVIC_DisableIRQ(EXTI9_5_IRQn);
		break;
		
		case LL_EXTI_LINE_10:
		case LL_EXTI_LINE_11:
		case LL_EXTI_LINE_12:
		case LL_EXTI_LINE_13:
		case LL_EXTI_LINE_14:
		case LL_EXTI_LINE_15:
			NVIC_DisableIRQ(EXTI15_10_IRQn);
		break;
	}
}
//

void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t stream,uint8_t priority)
{
	if(dma == DMA1)
	{
		if(stream == LL_DMA_STREAM_0)
		{		
			NVIC_EnableIRQ(DMA1_Stream0_IRQn);
			NVIC_SetPriority(DMA1_Stream0_IRQn,priority);
			return;
		}
		else if(stream == LL_DMA_STREAM_1)
		{		
			NVIC_EnableIRQ(DMA1_Stream1_IRQn);
			NVIC_SetPriority(DMA1_Stream1_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_2)
		{		
			NVIC_EnableIRQ(DMA1_Stream2_IRQn);
			NVIC_SetPriority(DMA1_Stream2_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_3)
		{		
			NVIC_EnableIRQ(DMA1_Stream3_IRQn);
			NVIC_SetPriority(DMA1_Stream3_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_4)
		{		
			NVIC_EnableIRQ(DMA1_Stream4_IRQn);
			NVIC_SetPriority(DMA1_Stream4_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_5)
		{		
			NVIC_EnableIRQ(DMA1_Stream5_IRQn);
			NVIC_SetPriority(DMA1_Stream5_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_6)
		{		
			NVIC_EnableIRQ(DMA1_Stream6_IRQn);
			NVIC_SetPriority(DMA1_Stream6_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_7)
		{		
			NVIC_EnableIRQ(DMA1_Stream7_IRQn);
			NVIC_SetPriority(DMA1_Stream7_IRQn,priority);
		}
	}
	#ifdef DMA2
	else if(dma == DMA2)
	{
		if(stream == LL_DMA_STREAM_0)
		{		
			NVIC_EnableIRQ(DMA2_Stream0_IRQn);
			NVIC_SetPriority(DMA2_Stream0_IRQn,priority);
			return;
		}
		else if(stream == LL_DMA_STREAM_1)
		{		
			NVIC_EnableIRQ(DMA2_Stream1_IRQn);
			NVIC_SetPriority(DMA2_Stream1_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_2)
		{		
			NVIC_EnableIRQ(DMA2_Stream2_IRQn);
			NVIC_SetPriority(DMA2_Stream2_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_3)
		{		
			NVIC_EnableIRQ(DMA2_Stream3_IRQn);
			NVIC_SetPriority(DMA2_Stream3_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_4)
		{		
			NVIC_EnableIRQ(DMA2_Stream4_IRQn);
			NVIC_SetPriority(DMA2_Stream4_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_5)
		{		
			NVIC_EnableIRQ(DMA2_Stream5_IRQn);
			NVIC_SetPriority(DMA2_Stream5_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_6)
		{		
			NVIC_EnableIRQ(DMA2_Stream6_IRQn);
			NVIC_SetPriority(DMA2_Stream6_IRQn,priority);
		}
		else if(stream == LL_DMA_STREAM_7)
		{		
			NVIC_EnableIRQ(DMA2_Stream7_IRQn);
			NVIC_SetPriority(DMA2_Stream7_IRQn,priority);
		}
	}
	#endif
}
//


void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream)
{
	switch(dma_stream)
	{
		case LL_DMA_STREAM_0:
			LL_DMA_ClearFlag_TC0(dma);
		break;
		
		case LL_DMA_STREAM_1:
			LL_DMA_ClearFlag_TC1(dma);
		break;
		
		case LL_DMA_STREAM_2:
			LL_DMA_ClearFlag_TC2(dma);
		break;
		
		case LL_DMA_STREAM_3:
			LL_DMA_ClearFlag_TC3(dma);
		break;
		
		case LL_DMA_STREAM_4:
			LL_DMA_ClearFlag_TC4(dma);
		break;
		
		case LL_DMA_STREAM_5:
			LL_DMA_ClearFlag_TC5(dma);
		break;
		
		case LL_DMA_STREAM_6:
			LL_DMA_ClearFlag_TC6(dma);
		break;
		
		case LL_DMA_STREAM_7:
			LL_DMA_ClearFlag_TC7(dma);
		break;
	}
}
//


uint8_t CheckDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream)
{
	switch(dma_stream)
	{
		case LL_DMA_STREAM_0:
			return LL_DMA_IsActiveFlag_TC0(dma);
		
		case LL_DMA_STREAM_1:
			return LL_DMA_IsActiveFlag_TC1(dma);
		
		case LL_DMA_STREAM_2:
			return LL_DMA_IsActiveFlag_TC2(dma);
		
		case LL_DMA_STREAM_3:
			return LL_DMA_IsActiveFlag_TC3(dma);
		
		case LL_DMA_STREAM_4:
			return LL_DMA_IsActiveFlag_TC4(dma);
		
		case LL_DMA_STREAM_5:
			return LL_DMA_IsActiveFlag_TC5(dma);
		
		case LL_DMA_STREAM_6:
			return LL_DMA_IsActiveFlag_TC6(dma);
		
		case LL_DMA_STREAM_7:
			return LL_DMA_IsActiveFlag_TC7(dma);
	}
	
	return 0;
}
//


uint8_t GetTimIcFlag(TIM_TypeDef* tim,uint32_t ch)
{
	switch(ch)
	{
		case LL_TIM_CHANNEL_CH1:
			return LL_TIM_IsActiveFlag_CC1(tim);
		break;
		
		case LL_TIM_CHANNEL_CH2:
			return LL_TIM_IsActiveFlag_CC2(tim);
		break;
		
		case LL_TIM_CHANNEL_CH3:
			return LL_TIM_IsActiveFlag_CC3(tim);
		break;
		
		case LL_TIM_CHANNEL_CH4:
			return LL_TIM_IsActiveFlag_CC4(tim);
		break;
	}
}
//

void ClearIcFlag(TIM_TypeDef* tim,uint32_t ch)
{
	switch(ch)
	{
		case LL_TIM_CHANNEL_CH1:
			LL_TIM_ClearFlag_CC1(tim);
		break;
		
		case LL_TIM_CHANNEL_CH2:
			LL_TIM_ClearFlag_CC2(tim);
		break;
		
		case LL_TIM_CHANNEL_CH3:
			LL_TIM_ClearFlag_CC3(tim);
		break;
		
		case LL_TIM_CHANNEL_CH4:
			LL_TIM_ClearFlag_CC4(tim);
		break;
	}
}
//

void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t stream)
{
	if(dma == DMA1)
	{
		if(stream == LL_DMA_STREAM_0)
		{		
			NVIC_DisableIRQ(DMA1_Stream0_IRQn);
			return;
		}
		else if(stream == LL_DMA_STREAM_1)
		{		
			NVIC_DisableIRQ(DMA1_Stream1_IRQn);
		}
		else if(stream == LL_DMA_STREAM_2)
		{		
			NVIC_DisableIRQ(DMA1_Stream2_IRQn);
		}
		else if(stream == LL_DMA_STREAM_3)
		{		
			NVIC_DisableIRQ(DMA1_Stream3_IRQn);
		}
		else if(stream == LL_DMA_STREAM_4)
		{		
			NVIC_DisableIRQ(DMA1_Stream4_IRQn);
		}
		else if(stream == LL_DMA_STREAM_5)
		{		
			NVIC_DisableIRQ(DMA1_Stream5_IRQn);
		}
		else if(stream == LL_DMA_STREAM_6)
		{		
			NVIC_DisableIRQ(DMA1_Stream6_IRQn);
		}
		else if(stream == LL_DMA_STREAM_7)
		{		
			NVIC_DisableIRQ(DMA1_Stream7_IRQn);
		}
	}
	else if(dma == DMA2)
	{
		if(stream == LL_DMA_STREAM_0)
		{		
			NVIC_DisableIRQ(DMA2_Stream0_IRQn);
			return;
		}
		else if(stream == LL_DMA_STREAM_1)
		{		
			NVIC_DisableIRQ(DMA2_Stream1_IRQn);
		}
		else if(stream == LL_DMA_STREAM_2)
		{		
			NVIC_DisableIRQ(DMA2_Stream2_IRQn);
		}
		else if(stream == LL_DMA_STREAM_3)
		{		
			NVIC_DisableIRQ(DMA2_Stream3_IRQn);
		}
		else if(stream == LL_DMA_STREAM_4)
		{		
			NVIC_DisableIRQ(DMA2_Stream4_IRQn);
		}
		else if(stream == LL_DMA_STREAM_5)
		{		
			NVIC_DisableIRQ(DMA2_Stream5_IRQn);
		}
		else if(stream == LL_DMA_STREAM_6)
		{		
			NVIC_DisableIRQ(DMA2_Stream6_IRQn);
		}
		else if(stream == LL_DMA_STREAM_7)
		{		
			NVIC_DisableIRQ(DMA2_Stream7_IRQn);
		}
	}
}
//
