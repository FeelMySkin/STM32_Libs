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

#include "periph_functions.h"

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
	ADC_TYPE_INNER_TEMP, /** Count inner(core) temperature */
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
	float			offset; /** offset from base voltage (-3.3~3.3) */
};

/**
 * @brief ADC and DMA Initializer Struct.
 * 
 */
struct ADC_InitStruct
{
	ADC_TypeDef*	adc; /** set ADC* */
	DMA_TypeDef* 	dma; /** set DMA* */
	uint32_t		dma_stream; /** set LL_DMA_STREAM_* */
	uint32_t		dma_channel; /** set LL_DMA_CHANNEL_CH* */
};
	
/**
 * @brief Checks if inner ADC enabled flags
 * 
 */
struct ADC_Flags
{
	bool temp:1;
	bool voltage:1;
	bool bat:1;
};

/**
 * @brief ADC Controller class. Takes ADC data and populates array through DMA.
 * First set all lines (AddLine) (includes Temp and Inner V), then set sampling time (SetSmaplingTime), then Init
 */
class ADCController
{
	public:
		ADCController();
		~ADCController();
		/**
		 * @brief Initialize the ADC
		 * 
		 * @param str initializer struct
		 * @param samples set number of samples min = 1
		 */
		void Init(ADC_InitStruct str, uint8_t samples=5);
		/**
		 * @brief Set the Sampling Time
		 * 
		 * @param sampling set LL_ADC_SAMPLINGTIME_*
		 */
		void SetSamplingTime(uint32_t sampling);
		/**
		 * @brief Dynamicaly dds new ADC Line with it's channel
		 * 
		 * @param str ADC channel initializer (see ADC_Struct)
		 */
		void AddLine(ADC_Struct str);
		/** 
		 * @brief Enables Inner Voltage Channel
		 * 
		*/
		void AddInnerVoltageLine(); 
		/** 
		 * @brief Enables Core Temperature Channel 
		 * 
		*/
		void AddTempSensor();
		/**
		 * @brief Enables or Disables DMA Interrupt
		 * 
		 * @param stat (TRUE/FALSE)
		 */
		void EnableDmaInterrupt(bool stat);
		/**
		 * @brief Get the Measure by channel number
		 * 
		 * @param adc_channel channel number (0~15)
		 * @return (float) measured and counted adc channel 
		 */
		float getMeasure(uint8_t adc_channel);
		/**
		 * @brief Get the Pointer To Counted Measure 
		 * 
		 * @param ch_num channel number (0~15)
		 * @return (float*) pointer to *results
		 */
		float *GetPointerToChannel(uint8_t ch_num);
		/**
		 * @brief Get the Pointer To RAW measure
		 * (perfectly works with 1 sample)
		 * @param ch_num channel number (0~15)
		 * @return (uint32_t*) pointer to *meas
		 */
		uint32_t *GetPointerToMeas(uint8_t ch_num);
		/**
		 * @brief Count All channels from RAW measurements
		 * 
		 */
		void ProcessAll();
		/**
		 * @brief Process selected channel only
		 * 
		 * @param ch channel number (0~15)
		 */
		void Process(uint32_t ch);
		/**
		 * @brief Count Inner voltage only
		 * 
		 */
		void ProcessInner();
		/**
		 * @brief Count core temperature only
		 * 
		 */
		void ProcessTemp();
		/**
		 * @brief Get the Core Temperature
		 * 
		 * @return (float) core temp in degC
		 */
		float GetTemp();
		bool first_data_gained; /** check if first DMA cycle is completed */
	
	
	private:
		/**
		 * @brief Initialize ALl channels GPIO
		 * 
		 */
		void InitGPIO();
		/**
		 * @brief Initialize ADC and every ADC line 
		 * 
		 */
		void InitLines();
		/**
		 * @brief Initialize DMA for ADC
		 * 
		 */
		void InitDMA();
		/**
		 * @brief Initialize arrays for RAW samples (channels * samples)
		 * 
		 */
		void InitMemory();
		/**
		 * @brief Sort ADC lines by number (low to high). In some MCUs it is needed that 
		 * ADC lines are sorted out.
		 * 
		 */
		void SortLines();
		/**
		 * @brief For SortLines method. Swaps to channels in array.
		 * 
		 * @param ch1 First channel number (0~15)
		 * @param ch2 Second channel number (0~15)
		 */
		void SwapChannels(uint8_t ch1, uint8_t ch2);
		/**
		 * @brief Counts and returns RANKS SCANNER quantity for ADC (For consequence readings).
		 * 
		 * @return (uint32_t) LL_ADC_REG_SEQ_SCAN_*
		 */
		uint32_t RanksCounter();
		/**
		 * @brief Get the Rank for initializing sequence readings. Calls in InitLines.
		 * 
		 * @param ch number of initializing channel (0~19). 
		 * @return (uint32_t) LL_ADC_REG_RANK_*
		 */
		uint32_t GetRank(uint8_t ch);
	
		uint32_t sampling; /**LL_ADC_SAMPLINGTIME_* */
		uint8_t size,samples; /** size of array and samples */
		uint32_t *meas; /** RAW measured data from ADC */
		uint32_t *meas_getter; /** Array for averaging raw datas */
		float *results; /** counted measures */
		float inner_voltage,inner_voltage_coeff; /** inner voltage and inner voltage correcting coefficient (for fast math) */
	
		ADC_Flags flags; /** flags for inner measures */
		ADC_Struct	*adc; /** channels dynamic list */
		ADC_InitStruct init; /** ADC init struct */
};


#endif
