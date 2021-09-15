#include "periph_functions.h"

void EnableExtiIRQn(uint32_t exti, uint8_t prio)
{
	switch(exti)
	{
		case LL_EXTI_LINE_0:
		case LL_EXTI_LINE_1:
			NVIC_EnableIRQ(EXTI0_1_IRQn);
			NVIC_SetPriority(EXTI0_1_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_2:
		case LL_EXTI_LINE_3:
			NVIC_EnableIRQ(EXTI2_3_IRQn);
			NVIC_SetPriority(EXTI2_3_IRQn,prio);
		break;
		
		case LL_EXTI_LINE_4:
		case LL_EXTI_LINE_5:
		case LL_EXTI_LINE_6:
		case LL_EXTI_LINE_7:
		case LL_EXTI_LINE_8:
		case LL_EXTI_LINE_9:
		case LL_EXTI_LINE_10:
		case LL_EXTI_LINE_11:
		case LL_EXTI_LINE_12:
		case LL_EXTI_LINE_13:
		case LL_EXTI_LINE_14:
		case LL_EXTI_LINE_15:
			NVIC_EnableIRQ(EXTI4_15_IRQn);
			NVIC_SetPriority(EXTI4_15_IRQn,prio);
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
	if(tim == TIM1)
	{
		NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
		NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,priority);
	}
	#ifdef TIM2
	else if(tim == TIM2)
	{
		NVIC_EnableIRQ(TIM2_IRQn);
		NVIC_SetPriority(TIM2_IRQn,priority);
	}
	#endif
	else if(tim == TIM3)
	{
		NVIC_EnableIRQ(TIM3_IRQn);
		NVIC_SetPriority(TIM3_IRQn,priority);
	}
	#ifdef TIM4
	else if(tim == TIM4)
	{
		NVIC_EnableIRQ(TIM4_IRQn);
		NVIC_SetPriority(TIM4_IRQn,priority);
	}
	#endif
	#ifdef TIM5
	else if(tim == TIM5)
	{
		NVIC_EnableIRQ(TIM5_IRQn);
		NVIC_SetPriority(TIM5_IRQn,priority);
	}
	#endif
	#ifdef TIM6
	else if(tim == TIM6)
	{
		NVIC_EnableIRQ(TIM6_IRQn);
		NVIC_SetPriority(TIM6_IRQn,priority);
	}
	#endif
	#ifdef TIM7
	else if(tim == TIM7)
	{
		NVIC_EnableIRQ(TIM7_IRQn);
		NVIC_SetPriority(TIM7_IRQn,priority);
	}
	#endif
	#ifdef TIM9
	else if(tim == TIM9)
	{
		NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
		NVIC_SetPriority(TIM1_BRK_TIM9_IRQn,priority);
	}
	#endif
	else if(tim == TIM14)
	{
		NVIC_EnableIRQ(TIM14_IRQn);
		NVIC_SetPriority(TIM14_IRQn,priority);
	}
	else if(tim == TIM15)
	{
		NVIC_EnableIRQ(TIM15_IRQn);
		NVIC_SetPriority(TIM15_IRQn,priority);
	}
	else if(tim == TIM16)
	{
		NVIC_EnableIRQ(TIM16_IRQn);
		NVIC_SetPriority(TIM16_IRQn,priority);
	}
	else if(tim == TIM17)
	{
		NVIC_EnableIRQ(TIM17_IRQn);
		NVIC_SetPriority(TIM17_IRQn,priority);
	}


}
//

void DisableExtiIRQn(uint32_t exti_line)
{
	switch(exti_line)
	{
		case LL_EXTI_LINE_0:
		case LL_EXTI_LINE_1:
			NVIC_DisableIRQ(EXTI0_1_IRQn);
		break;
		
		case LL_EXTI_LINE_2:
		case LL_EXTI_LINE_3:
			NVIC_DisableIRQ(EXTI2_3_IRQn);
		break;
		
		case LL_EXTI_LINE_4:
		case LL_EXTI_LINE_5:
		case LL_EXTI_LINE_6:
		case LL_EXTI_LINE_7:
		case LL_EXTI_LINE_8:
		case LL_EXTI_LINE_9:
		case LL_EXTI_LINE_10:
		case LL_EXTI_LINE_11:
		case LL_EXTI_LINE_12:
		case LL_EXTI_LINE_13:
		case LL_EXTI_LINE_14:
		case LL_EXTI_LINE_15:
			NVIC_DisableIRQ(EXTI4_15_IRQn);
		break;
	}
}
//

void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t channel,uint8_t priority)
{
	if(dma == DMA1)
	{
		switch(channel)
		{
			case LL_DMA_CHANNEL_1:
				NVIC_EnableIRQ(DMA1_Ch1_IRQn);
				NVIC_SetPriority(DMA1_Ch1_IRQn,priority);
			break;
			
			case LL_DMA_CHANNEL_2:
			case LL_DMA_CHANNEL_3:
				NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
				NVIC_SetPriority(DMA1_Ch2_3_DMA2_Ch1_2_IRQn,priority);
			break;
			
			case LL_DMA_CHANNEL_4:
			case LL_DMA_CHANNEL_5:
			#ifdef LL_DMA_CHANNEL6
			case LL_DMA_CHANNEL_6:
			case LL_DMA_CHANNEL_7:
			#endif
				NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
				NVIC_SetPriority(DMA1_Ch4_7_DMA2_Ch3_5_IRQn,priority);
			break;
		}
	}
	#ifdef DMA2
	if(dma == DMA2)
	{
		switch(channel)
		{
			case LL_DMA_CHANNEL_1:
			case LL_DMA_CHANNEL_2:
				NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
				NVIC_SetPriority(DMA1_Ch2_3_DMA2_Ch1_2_IRQn,priority);
			break;
			
			case LL_DMA_CHANNEL_3:
			case LL_DMA_CHANNEL_4:
			case LL_DMA_CHANNEL_5:
			#ifdef LL_DMA_CHANNEL6
			case LL_DMA_CHANNEL_6:
			case LL_DMA_CHANNEL_7:
			#endif
				NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
				NVIC_SetPriority(DMA1_Ch4_7_DMA2_Ch3_5_IRQn,priority);
			break;
		}
	}
	#endif
}
//


/*void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream)
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
}*/
//

void EnableUsartIrqn(USART_TypeDef* usart,uint8_t priority)
{
	if(usart == USART1)
	{
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn,priority);
	}
}
//

void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t channel)
{
	if(dma == DMA1)
	{
		switch(channel)
		{
			case LL_DMA_CHANNEL_1:
				NVIC_DisableIRQ(DMA1_Ch1_IRQn);
			break;
			
			case LL_DMA_CHANNEL_2:
			case LL_DMA_CHANNEL_3:
				NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
			break;
			
			case LL_DMA_CHANNEL_4:
			case LL_DMA_CHANNEL_5:
			#ifdef LL_DMA_CHANNEL6
			case LL_DMA_CHANNEL_6:
			case LL_DMA_CHANNEL_7:
			#endif
				NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
			break;
		}
	}
	#ifdef DMA2
	if(dma == DMA2)
	{
		switch(channel)
		{
			case LL_DMA_CHANNEL_1:
			case LL_DMA_CHANNEL_2:
				NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
			break;
			
			case LL_DMA_CHANNEL_3:
			case LL_DMA_CHANNEL_4:
			case LL_DMA_CHANNEL_5:
			#ifdef LL_DMA_CHANNEL6
			case LL_DMA_CHANNEL_6:
			case LL_DMA_CHANNEL_7:
			#endif
				NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
			break;
		}
	}
	#endif
}
//

uint32_t GetTimOC(TIM_TypeDef* tim,uint32_t channel)
{
	if(channel == LL_TIM_CHANNEL_CH1) return LL_TIM_OC_GetCompareCH1(tim);
	if(channel == LL_TIM_CHANNEL_CH2) return LL_TIM_OC_GetCompareCH2(tim);
	if(channel == LL_TIM_CHANNEL_CH3) return LL_TIM_OC_GetCompareCH3(tim);
	if(channel == LL_TIM_CHANNEL_CH4) return LL_TIM_OC_GetCompareCH4(tim);
	return 0;
}
//

void RemapDmaUsart(USART_TypeDef* uart,uint32_t channel)
{
	if(uart == USART1)
	{
		if(channel == LL_DMA_CHANNEL_2) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1TX_RMP_DMA1CH2);
		else if(channel == LL_DMA_CHANNEL_3) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1RX_RMP_DMA1CH3);
		else if(channel == LL_DMA_CHANNEL_4) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1TX_RMP_DMA1CH4);
		else if(channel == LL_DMA_CHANNEL_5) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1RX_RMP_DMA1CH5);
	}
	else if(uart == USART2)
	{
		if(channel == LL_DMA_CHANNEL_5 || channel == LL_DMA_CHANNEL_4) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART2_RMP_DMA1CH54);
		else if( channel == LL_DMA_CHANNEL_6 || channel == LL_DMA_CHANNEL_7) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART2_RMP_DMA1CH67);
	}
	else if(uart == USART3)
	{
		if(channel == LL_DMA_CHANNEL_2 || channel == LL_DMA_CHANNEL_3) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART3_RMP_DMA1CH32);
		else if( channel == LL_DMA_CHANNEL_6 || channel == LL_DMA_CHANNEL_7) LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART3_RMP_DMA1CH67);
	}
}
//
