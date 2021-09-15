#ifndef PERIPH_FUNCTIONS_H
#define PERIPH_FUNCTIONS_H


#include "stdint.h"
#include "stdlib.h"
#include "stm32f0xx.h"
	
void EnableExtiIRQn(uint32_t exti_line,uint8_t priority);
void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority);
void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t channel,uint8_t priority);
void EnableUsartIrqn(USART_TypeDef* usart,uint8_t priority);
void DisableExtiIRQn(uint32_t exti_line);
void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t channel);

void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line);
void RemapDmaUsart(USART_TypeDef*,uint32_t channel);
uint32_t GetExtiLine(uint32_t pin);
uint32_t GetTimOC(TIM_TypeDef* tim,uint32_t channel);
//void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);


#endif
