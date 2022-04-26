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

/*uint32_t GetExtiLine(uint32_t pin)
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
}*/
//

void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority)
{
	if(tim == TIM1 )
	{
		NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
		NVIC_EnableIRQ(TIM1_CC_IRQn);
		NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,priority);
		NVIC_SetPriority(TIM1_CC_IRQn,priority);
	}
	else if(tim == TIM3)
	{
		NVIC_EnableIRQ(TIM3_IRQn);
		NVIC_SetPriority(TIM3_IRQn,priority);
	}
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
	else if(tim == TIM14)
	{
		NVIC_EnableIRQ(TIM14_IRQn);
		NVIC_SetPriority(TIM14_IRQn,priority);
	}

}
//

void EnableUsartIRQn(USART_TypeDef* usart,uint8_t priority)
{
	if(usart == USART1)
	{
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn,priority);
	}
	else if(usart == USART2)
	{
		NVIC_EnableIRQ(USART2_IRQn);
		NVIC_SetPriority(USART2_IRQn,priority);
	}
}

void DisableExtiIRQn(uint32_t exti_line)
{
	switch(exti_line)
	{
		case LL_EXTI_LINE_0:
		case LL_EXTI_LINE_1:
			NVIC_DisableIRQ(EXTI0_1_IRQn);
		break;
		
		case LL_EXTI_LINE_3:
		case LL_EXTI_LINE_2:
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
	
	switch(channel)
	{
		case LL_DMA_CHANNEL_1:
			NVIC_EnableIRQ(DMA1_Channel1_IRQn);
			NVIC_SetPriority(DMA1_Channel1_IRQn,priority);
			return;
		
		case LL_DMA_CHANNEL_2:
		case LL_DMA_CHANNEL_3:
			NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
			NVIC_SetPriority(DMA1_Channel2_3_IRQn,priority);
		return;
		
		case LL_DMA_CHANNEL_4:
		case LL_DMA_CHANNEL_5:
		#ifdef LL_DMA_CHANNEL_6
		case LL_DMA_CHANNEL_6:
		case LL_DMA_CHANNEL_7:
			NVIC_EnableIRQ(DMA1_Ch4_7_DMAMUX1_OVR_IRQn);
			NVIC_SetPriority(DMA1_Ch4_7_DMAMUX1_OVR_IRQn,priority);
		#else
			NVIC_EnableIRQ(DMA1_Ch4_5_DMAMUX1_OVR_IRQn);
			NVIC_SetPriority(DMA1_Ch4_5_DMAMUX1_OVR_IRQn,priority);
		#endif
		return;
	}

	
}
//

void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t channel)
{
	switch(channel)
	{
		case LL_DMA_CHANNEL_1:
			NVIC_DisableIRQ(DMA1_Channel1_IRQn);
			return;
		
		case LL_DMA_CHANNEL_2:
		case LL_DMA_CHANNEL_3:
			NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
		return;
		
		case LL_DMA_CHANNEL_4:
		case LL_DMA_CHANNEL_5:
		#ifdef LL_DMA_CHANNEL_6
		case LL_DMA_CHANNEL_6:
		case LL_DMA_CHANNEL_7:
			NVIC_DisableIRQ(DMA1_Ch4_7_DMAMUX1_OVR_IRQn);
		#else
			NVIC_DisableIRQ(DMA1_Ch4_5_DMAMUX1_OVR_IRQn);
		#endif
			return;
	}
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

void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line)
{
	uint16_t syscfg_exti_source;
	uint32_t syscfg_exti_line;
	if(gpio == GPIOA) syscfg_exti_source = LL_EXTI_CONFIG_PORTA;
	else if(gpio == GPIOB) syscfg_exti_source = LL_EXTI_CONFIG_PORTB;
	else if(gpio == GPIOC) syscfg_exti_source = LL_EXTI_CONFIG_PORTC;
	else if(gpio == GPIOD) syscfg_exti_source = LL_EXTI_CONFIG_PORTD;
	#ifdef GPIOE
	else if(gpio == GPIOE) syscfg_exti_source = LL_EXTI_CONFIG_PORTE;
	#endif
	
	switch(exti_line)
	{
		case LL_GPIO_PIN_0:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE0;
		break;
		
		case LL_GPIO_PIN_1:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE1;
		break;
		
		case LL_GPIO_PIN_2:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE2;
		break;
		
		case LL_GPIO_PIN_3:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE3;
		break;
		
		case LL_GPIO_PIN_4:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE4;
		break;
		
		case LL_GPIO_PIN_5:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE5;
		break;
		
		case LL_GPIO_PIN_6:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE6;
		break;
		
		case LL_GPIO_PIN_7:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE7;
		break;
		
		case LL_GPIO_PIN_8:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE8;
		break;
		
		case LL_GPIO_PIN_9:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE9;
		break;
		
		case LL_GPIO_PIN_10:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE10;
		break;
		
		case LL_GPIO_PIN_11:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE11;
		break;
		
		case LL_GPIO_PIN_12:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE12;
		break;
		
		case LL_GPIO_PIN_13:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE13;
		break;
		
		case LL_GPIO_PIN_14:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE14;
		break;
		
		case LL_GPIO_PIN_15:
			syscfg_exti_line = LL_EXTI_CONFIG_LINE15;
		break;
	}
	
	LL_EXTI_SetEXTISource(syscfg_exti_source,syscfg_exti_line);
	
}
//
