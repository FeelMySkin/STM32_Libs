#ifndef PERIPH_FUNCTIONS_H
#define PERIPH_FUNCTIONS_H

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_iwdg.h"

void EnableExtiIRQn(uint32_t exti_line,uint8_t priority);
void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority);
void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t stream,uint8_t priority);
void EnableUartIRQn(USART_TypeDef* uart, uint8_t priority);

void DisableExtiIRQn(uint32_t exti_line);
void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t stream);

void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line);
uint32_t GetExtiLine(uint32_t pin);
uint8_t CheckDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);
uint8_t GetTimIcFlag(TIM_TypeDef* tim,uint32_t ch);
void ClearIcFlag(TIM_TypeDef* tim,uint32_t ch);

void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);

#endif
