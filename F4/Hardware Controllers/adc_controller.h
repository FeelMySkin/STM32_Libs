/**
 * @file adc_controller.h
 * @author Phil (zetsuboulevel@gmail.com)
 * @brief ADC C++ class based on STM32 LL Library.
 * @version 0.1
 * @date 2022-08-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "defines.h"

/**
 * @brief Hard-defines channel numbers
 * 
 */
#define ADCs_quant 19
#define TEMP_CH		16
#define V_REF_CH	17
#define V_BAT_CH	18

/**
 * @brief Channel mapping for initializing
 * 
 */
const uint32_t channel_mapping[ADCs_quant] = 	{LL_ADC_CHANNEL_0,LL_ADC_CHANNEL_1,LL_ADC_CHANNEL_2,LL_ADC_CHANNEL_3,LL_ADC_CHANNEL_4,LL_ADC_CHANNEL_5,LL_ADC_CHANNEL_6,
										LL_ADC_CHANNEL_7,LL_ADC_CHANNEL_8,LL_ADC_CHANNEL_9,LL_ADC_CHANNEL_10,LL_ADC_CHANNEL_11,LL_ADC_CHANNEL_12,LL_ADC_CHANNEL_13,
										LL_ADC_CHANNEL_14,LL_ADC_CHANNEL_15,LL_ADC_CHANNEL_TEMPSENSOR,LL_ADC_CHANNEL_VREFINT,LL_ADC_CHANNEL_VBAT};

/**
 * @brief Type of ADC measure.
 * 
 */
enum ADC_TYPE
{
	ADC_TYPE_VOLTAGE, /** Count RAW data to Voltage */
	ADC_TYPE_CURRENT, /** Count RAW data to Current */
	ADC_TYPE_POS_RESISTANCE, /** Count RAW data to Resistance */
	ADC_TYPE_NEG_RESISTANCE, /** Count RAW data to Resistance */
	ADC_TYPE_INNER_VOLTAGE, /** Count inner(supply) voltage */
	ADC_TYPE_RAW /** RAW data from ADC */
};

/**
 * @brief ADC Channel Initializer
 * 
 */
struct ADC_Struct
{
	ADC_TYPE 		type; /** ADC_TYPE*/
	GPIO_TypeDef* 	gpio; /** set GPIO* (exmpl GPIOA) */
	uint32_t		pin; /** set LL_GPIO_PIN_* */
	float			coeff; /** set correcting coefficient (for voltage dividers, current shunt etc) */
	uint32_t		adc_ch; /** not need to set this (it is LL_ADC_CHANNEL_CH*) */
	uint8_t 		ch_num; /** set ch number (not LL_ADC_CHANNEL_CH*). (0~15) */
	float			offset; /** offset from base voltage (0-3.3) */
};

/**
 * @brief ADC DMA Initializer Struct.
 * 
 */
struct ADC_InitStruct
{
	DMA_TypeDef* 	dma;
	uint32_t		dma_stream;
	uint32_t		dma_channel;
};
	

struct ADC_Flags
{
	bool temp:1;
	bool voltage:1;
	bool bat:1;
	uint8_t v_ptr:4;
	uint8_t t_ptr:4;
	uint8_t b_ptr:4;
};

class ADCController
{
	public:
		ADCController();
		~ADCController();
		void Init(ADC_InitStruct, uint8_t samples=5);
		void SetSamplingTime(uint32_t sampling);
		void AddLine(ADC_Struct);
		void AddInnerVoltageLine();
		void EnableDmaInterrupt(bool stat);
		double getMeasure(uint8_t adc_channel);
		double *GetPointerToChannel(uint8_t ch_num);
		void ProcessAll();
		void Process(uint32_t ch);
		void ProcessInner();
		bool first_data_gained;
	
	
	private:
		void InitGPIO();
		void InitLines();
		void InitADC();
		void InitDMA();
		void InitMemory();
		void SortLines();
		void SwapChannels(uint8_t ch1, uint8_t ch2);
		uint32_t RanksCounter();
		uint32_t GetRank(uint8_t);
	
		uint32_t sampling;
		uint8_t size,samples;
		uint32_t *meas;
		double *results;
		uint8_t buf_cnt;
		double inner_voltage,inner_voltage_coeff;
	
		ADC_Flags flags;
		ADC_Struct	*adc;
		ADC_InitStruct init;
};


#endif
