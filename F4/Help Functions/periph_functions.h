#ifndef PERIPH_FUNCTIONS_H
#define PERIPH_FUNCTIONS_H

#include "stm32f4xx.h"

void EnableExtiIRQn(uint32_t exti_line,uint8_t priority);
void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority);
void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t stream,uint8_t priority);
void EnableUartIRQn(USART_TypeDef* uart, uint8_t priority);

void DisableExtiIRQn(uint32_t exti_line);
void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t stream);

void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line);
uint32_t GetExtiLine(uint32_t pin);
uint8_t CheckDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);
void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);

#endif
