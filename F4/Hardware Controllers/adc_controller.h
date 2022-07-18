#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "defines.h"

#define ADCs_quant 19
#define TEMP_CH		16
#define V_REF_CH	17
#define V_BAT_CH	18

const uint32_t channel_mapping[ADCs_quant] = 	{LL_ADC_CHANNEL_0,LL_ADC_CHANNEL_1,LL_ADC_CHANNEL_2,LL_ADC_CHANNEL_3,LL_ADC_CHANNEL_4,LL_ADC_CHANNEL_5,LL_ADC_CHANNEL_6,
										LL_ADC_CHANNEL_7,LL_ADC_CHANNEL_8,LL_ADC_CHANNEL_9,LL_ADC_CHANNEL_10,LL_ADC_CHANNEL_11,LL_ADC_CHANNEL_12,LL_ADC_CHANNEL_13,
										LL_ADC_CHANNEL_14,LL_ADC_CHANNEL_15,LL_ADC_CHANNEL_TEMPSENSOR,LL_ADC_CHANNEL_VREFINT,LL_ADC_CHANNEL_VBAT};

										
										
enum ADC_TYPE
{
	ADC_TYPE_VOLTAGE,
	ADC_TYPE_CURRENT,
	ADC_TYPE_POS_RESISTANCE,
	ADC_TYPE_NEG_RESISTANCE,
	ADC_TYPE_INNER_VOLTAGE,
	ADC_TYPE_INNER_TEMP,
	ADC_TYPE_RAW
};

struct ADC_Struct
{
	ADC_TYPE 		type;
	GPIO_TypeDef* 	gpio;
	uint32_t		pin;
	float			coeff;
	uint32_t		adc_ch;
	uint8_t 		ch_num;
	float			offset;
};

struct ADC_InitStruct
{
	ADC_TypeDef*	adc;
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
		void AddTempSensor();
		void EnableDmaInterrupt(bool stat);
		float getMeasure(uint8_t adc_channel);
		float *GetPointerToChannel(uint8_t ch_num);
		uint32_t *GetPointerToMeas(uint8_t ch_num);
		void ProcessAll();
		void Process(uint32_t ch);
		void ProcessInner();
		void ProcessTemp();
		float GetTemp();
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
		uint32_t *meas_getter;
		float *results;
		uint8_t buf_cnt;
		float inner_voltage,inner_voltage_coeff;
	
		volatile uint32_t temp_sens1, temp_sens2;
	
		ADC_Flags flags;
		ADC_Struct	*adc;
		ADC_InitStruct init;
};


#endif
