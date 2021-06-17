#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#include "defines.h"

#define TEMP_CH		11
#define V_REF_CH	12
#define V_BAT_CH	13
#define ADCs_quant	14

const uint32_t channel_mapping[ADCs_quant] = 	{LL_ADC_CHANNEL_0,LL_ADC_CHANNEL_1,LL_ADC_CHANNEL_2,LL_ADC_CHANNEL_3,LL_ADC_CHANNEL_4,LL_ADC_CHANNEL_5,LL_ADC_CHANNEL_6,
										LL_ADC_CHANNEL_7,LL_ADC_CHANNEL_8,LL_ADC_CHANNEL_9,LL_ADC_CHANNEL_10,LL_ADC_CHANNEL_TEMPSENSOR,LL_ADC_CHANNEL_VREFINT,LL_ADC_CHANNEL_VBAT};

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

//#define size 3
//#define samples 10
//template <uint8_t size,uint8_t samples>
class ADCController
{
	public:
		ADCController();
		~ADCController();
		void Init(uint8_t samples=5);
		void SetSamplingTime(uint32_t sampling);
		void AddLine(ADC_Struct);
		void AddInnerVoltageLine();
		double getMeasure(uint32_t adc_channel);
		void Process(uint32_t adc_channel);
		void ProcessAll();
		void ProcessInner();
		void EnableDmaInterrupt(bool state);
		bool first_data_gained;
	
	
	private:
		void InitGPIO();
		void InitLines();
		void InitADC();
		void InitDMA();
		void SortLines();
		void InitMemory();
		void SwapChannels(uint8_t ch1, uint8_t ch2);
		uint32_t RanksCounter();
		uint32_t GetRank(uint8_t);
	
		ADC_Struct	*adc;
		uint8_t		size,samples;
		uint32_t *meas;
		double *results;
		uint8_t buf_cnt;
		uint32_t sampling;
		double inner_voltage,inner_voltage_coeff;
};

//#include "adc_controller.cpp"


#endif
