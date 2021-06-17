#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "defines.h"

#define TEMP_CH		10
#define V_REF_CH	11
#define V_BAT_CH	12
#define ADCs_quant	13

const uint32_t channel_mapping[ADCs_quant] = 	{LL_ADC_CHANNEL_0,LL_ADC_CHANNEL_1,LL_ADC_CHANNEL_2,LL_ADC_CHANNEL_3,LL_ADC_CHANNEL_4,LL_ADC_CHANNEL_5,LL_ADC_CHANNEL_6,
										LL_ADC_CHANNEL_7,LL_ADC_CHANNEL_8,LL_ADC_CHANNEL_9,LL_ADC_CHANNEL_TEMPSENSOR,LL_ADC_CHANNEL_VREFINT,LL_ADC_CHANNEL_VBAT};

enum ADC_TYPE
{
	ADC_TYPE_VOLTAGE,
	ADC_TYPE_CURRENT,
	ADC_TYPE_POS_RESISTANCE,
	ADC_TYPE_NEG_RESISTANCE,
	ADC_TYPE_INNER_VOLTAGE,
	ADC_TYPE_RAW
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
		void Init(uint8_t samples=5);
		void SetSamplingTime(uint32_t sampling);
		void AddLine(ADC_Struct);
		void AddInnerVoltageLine();
		void EnableDmaInterrupt(bool stat);
		double getMeasure(uint32_t adc_channel);
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
	
		ADC_Struct *adc;
		uint8_t size,samples;
		uint32_t *meas;
		double *results;
		uint8_t buf_cnt;
		double inner_voltage,inner_voltage_coeff;
		uint32_t sampling;
		ADC_Flags flags;
};

#endif
