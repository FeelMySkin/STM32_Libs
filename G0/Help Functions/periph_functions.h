#ifndef PERIPH_FUNCTIONS_H
#define PERIPH_FUNCTIONS_H


#include "stdint.h"
#include "stdlib.h"
#include "stm32g0xx.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_rtc.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_pwr.h"
	
void EnableExtiIRQn(uint32_t exti_line,uint8_t priority);
void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority);
void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t channel,uint8_t priority);
void EnableUsartIRQn(USART_TypeDef* usart,uint8_t priority);
void DisableExtiIRQn(uint32_t exti_line);
void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t channel);

void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line);
//uint32_t GetExtiLine(uint32_t pin);
uint32_t GetTimOC(TIM_TypeDef* tim,uint32_t channel);
void ClearIcFlag(TIM_TypeDef* tim,uint32_t ch);


#endif
