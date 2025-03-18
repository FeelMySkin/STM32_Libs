/**
 * @file periph_functions.h
 * @author Phil (zetsuboulevel@gmail.com)
 * @brief LL peripheral includes and some basic functions.
 * @version 1.0
 * @date 2022-08-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
#include "stm32f4xx_ll_crc.h"


/** IRQ Enablers Zone (NVIC_EnableIRQ) */

/**
 * @brief Enables EXTI IRQ_Handler on exti_line with priority
 * 
 * @param exti_line set LL_EXTI_LINE_*
 * @param priority  set priority (0~15)
 */
void EnableExtiIRQn(uint32_t exti_line,uint8_t priority); 


/**
 * @brief Enables TIM* IRQHandler
 * 
 * @param tim set TIM*
 * @param priority set priority (0~15)
 */
void EnableTimIRQn(TIM_TypeDef* tim,uint8_t priority);


/**
 * @brief Enable DMA* IRQ_Handler
 * 
 * @param dma set DMA*
 * @param stream set LL_DMA_STREAM_*
 * @param priority set priority (0~15)
 */
void EnableDmaIRQn(DMA_TypeDef* dma, uint32_t stream,uint8_t priority);


/**
 * @brief Enables USART* IRQ_Handler
 * 
 * @param uart set USART*
 * @param priority set priority (0~15)
 */
void EnableUartIRQn(USART_TypeDef* uart, uint8_t priority);





/**IRQ Disablers Zone(NVIC_DisableIRQ)*/

/**
 * @brief Disables IRQ_Hanler on exti_line
 * 
 * @param exti_line set LL_EXTI_LINE_*
 */
void DisableExtiIRQn(uint32_t exti_line);

/**
 * @brief Disables IRQ_Handler for DMA*
 * 
 * @param dma set DMA*
 * @param stream set LL_DMA_STREAM*
 */
void DisableDmaIRQn(DMA_TypeDef* dma, uint32_t stream);


/** EXTI Zone*/

/**
 * @brief Set the EXTI Source to SysConfig register
 * 
 * @param gpio set the GPIO* (exmpl. GPIOB)
 * @param exti_line set the LL_EXTI_LINE_*
 */
void SetExtiSource(GPIO_TypeDef* gpio,uint32_t exti_line);

/**
 * @brief Returns LL_EXTI_LINE_* based on pin
 * 
 * @param pin set LL_GPIO_PIN_*
 * @return LL_EXTI_LINE_* 
 */
uint32_t GetExtiLine(uint32_t pin);


/**IRQ Flags Zone */

/**
 * @brief Check if DMA* Trasnfer Complete (TC) Flag is set
 * 
 * @param dma set DMA*
 * @param dma_stream set LL_DMA_STREAM_*
 * @return (TRUE,FALSE) 
 */
uint8_t CheckDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);

/**
 * @brief Check if TIM* Capture Compare(CC) on channel Flag is set
 * 
 * @param tim set TIM*
 * @param ch set LL_TIM_CHANNEL_CH*
 * @return (TRUE,FALSE) 
 */
uint8_t GetTimIcFlag(TIM_TypeDef* tim,uint32_t ch);

/**
 * @brief Clears TIM* Capture Compare(CC) Flag on channel
 * 
 * @param tim set TIM*
 * @param ch set LL_TIM_CHANNEL_CH*
 */
void ClearIcFlag(TIM_TypeDef* tim,uint32_t ch);
void EnableTimCCIT(TIM_TypeDef* tim,uint32_t channel);
void DisableTimCCIT(TIM_TypeDef* tim,uint32_t channel);

void SetTimCC(TIM_TypeDef* tim, uint32_t ch, uint32_t duty);
uint32_t GetTimCC(TIM_TypeDef* tim, uint32_t ch);

/**
 * @brief Clears DMA* Transfer COmplete(TC) Flag on stream
 * 
 * @param dma set DMA*
 * @param dma_stream set LL_DMA_STREAM_*
 */
void ClearDmaTCFlag(DMA_TypeDef* dma, uint32_t dma_stream);




#endif
