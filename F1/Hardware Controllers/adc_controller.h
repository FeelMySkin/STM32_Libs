#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "defines.h"

#define TEMP_CH		10
#define V_REF_CH	11
#define V_BAT_CH	12
//#define ADCs_quant	13

const uint32_t channel_mapping[] = 	{LL_ADC_CHANNEL_0,LL_ADC_CHANNEL_1,LL_ADC_CHANNEL_2,LL_ADC_CHANNEL_3,LL_ADC_CHANNEL_4,LL_ADC_CHANNEL_5,LL_ADC_CHANNEL_6,
										LL_ADC_CHANNEL_7,LL_ADC_CHANNEL_8,LL_ADC_CHANNEL_9,LL_ADC_CHANNEL_TEMPSENSOR,LL_ADC_CHANNEL_VREFINT
										#ifdef LL_ADC_CHANNEL_BAT
										,LL_ADC_CHANNEL_VBAT
										#endif
										};

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

struct ADC_Struct
{
	ADC_TYPE 		type;
	GPIO_TypeDef* 	gpio;
	uint32_t		pin;
	double			coeff;
	uint32_t		adc_ch;
	uint8_t 		ch_num;
	double			offset;
};

struct ADC_InitStruct
{
	ADC_TypeDef*	adc; /** set ADC* */
	DMA_TypeDef* 	dma; /** set DMA* */
	uint32_t		dma_channel; /** set LL_DMA_CHANNEL_CH* */
};

struct ADC_Flags
{
	bool temp:1;
	bool voltage:1;
	bool bat:1;
};

class ADCController
{
	public:
		ADCController();
		~ADCController();
		void Init(ADC_InitStruct str, uint8_t samples=5);
		void SetSamplingTime(uint32_t sampling);
		void AddLine(ADC_Struct str);
		void AddInnerVoltageLine();
		void EnableDmaInterrupt(bool stat);
		double getMeasure(uint8_t adc_channel);
		void ProcessAll();
		void Process(uint8_t ch);
		void ProcessInner();
		bool first_data_gained;
	
	
	private:
		void InitGPIO();
		void InitLines();
		void InitADC();
		uint32_t GetRank(uint8_t c);
		void InitDMA();
		void InitMemory();
		void SortLines();
		void SwapChannels(uint8_t ch1, uint8_t ch2);
		uint32_t GetRanksCount(uint8_t chans);
	
		ADC_Struct *adc;
		uint8_t size,samples;
		uint32_t *meas;
		double *results;
		uint8_t buf_cnt;
		double inner_voltage,inner_voltage_coeff;
		uint32_t sampling;
		ADC_Flags flags;
		
		ADC_InitStruct init; /** ADC init struct */
};

#endif
