#include "adc_controller.h"

ADCController::ADCController()
{
	
	size = 0;
	samples = 5;
	buf_cnt = 0;
	inner_voltage = 3.3;
	inner_voltage_coeff = inner_voltage/4095.0;
	first_data_gained = false;
	sampling = LL_ADC_SAMPLINGTIME_79CYCLES_5;
}
//

ADCController::~ADCController()
{
	
}
//

void ADCController::AddLine(ADC_Struct str)
{
	
	ADC_Struct *last_buf = adc;
	adc = new ADC_Struct[size+1];
	for(int i = 0;i<size;++i)
	{
		adc[i] = last_buf[i];
	}
	delete [] last_buf;
	
	adc[size] = str;
	adc[size].adc_ch = channel_mapping[adc[size].ch_num];
	size++;
	
}
//


void ADCController::AddInnerVoltageLine()
{
	ADC_Struct new_adc;
	new_adc.adc_ch = LL_ADC_CHANNEL_VREFINT;
	new_adc.ch_num = V_REF_CH;
	new_adc.coeff = 1;
	new_adc.gpio = 0;
	new_adc.pin = 0;
	new_adc.type = ADC_TYPE_INNER_VOLTAGE;
	new_adc.offset = 0;
	
	LL_ADC_SetCommonPathInternalCh(ADC1_COMMON,LL_ADC_PATH_INTERNAL_VREFINT);
	AddLine(new_adc);
	flags.voltage = 1;
}
//

void ADCController::Init(ADC_InitStruct in_str, uint8_t samples)
{
	init = in_str;
	this->samples = samples;
	SortLines();
	InitMemory();
	InitGPIO();
	InitDMA();
	InitLines();
	
	LL_ADC_REG_StartConversion(ADC1);
}
//

void ADCController::InitGPIO()
{	
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_ANALOG;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	for(int i = 0;i<size;++i)
	{
		gpio.Pin = adc[i].pin;
		if(adc[i].type != ADC_TYPE_INNER_VOLTAGE) LL_GPIO_Init(adc[i].gpio,&gpio);
	}
}
//

void ADCController::InitLines()
{
	LL_ADC_EnableInternalRegulator(ADC1);
	for(int i = 0;i<10000;++i ) asm("NOP");
	
	LL_ADC_StartCalibration(ADC1);
	while(LL_ADC_IsCalibrationOnGoing(ADC1)) asm("NOP");

	
	LL_ADC_InitTypeDef adc_ini;
	adc_ini.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	adc_ini.Resolution = LL_ADC_RESOLUTION_12B;
	adc_ini.LowPowerMode = LL_ADC_LP_MODE_NONE;
	adc_ini.Clock = LL_ADC_CLOCK_ASYNC_DIV1;
	LL_ADC_Init(ADC1,&adc_ini);
	
	LL_ADC_REG_InitTypeDef reg;
	reg.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	reg.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	reg.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	reg.SequencerLength = RanksCounter();
	reg.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	reg.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
	LL_ADC_REG_Init(ADC1,&reg);
	
	LL_ADC_ClearFlag_ADRDY(ADC1);
	LL_ADC_Enable(ADC1);
	while(!LL_ADC_IsActiveFlag_ADRDY(ADC1)) asm("NOP");
	
	LL_ADC_REG_SetSequencerScanDirection(ADC1,LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
	LL_ADC_REG_SetSequencerConfigurable(ADC1,LL_ADC_REG_SEQ_CONFIGURABLE);
	LL_ADC_SetSamplingTimeCommonChannels(ADC1,LL_ADC_SAMPLINGTIME_COMMON_1,sampling);
	LL_ADC_SetSamplingTimeCommonChannels(ADC1,LL_ADC_SAMPLINGTIME_COMMON_2,LL_ADC_SAMPLINGTIME_3CYCLES_5);
	
	for(int i = 0;i<size;++i)
	{
		LL_ADC_REG_SetSequencerRanks(ADC1,GetRank(i),adc[i].adc_ch);
		//if(adc[i].adc_ch == LL_ADC_CHANNEL_VREFINT) LL_ADC_SetChannelSamplingTime(ADC1,adc[i].adc_ch,LL_ADC_SAMPLINGTIME_COMMON_2);
		/*else */LL_ADC_SetChannelSamplingTime(ADC1,adc[i].adc_ch,LL_ADC_SAMPLINGTIME_COMMON_1);
		while(!LL_ADC_IsActiveFlag_CCRDY(ADC1)) asm("NOP");
	}
	if(size!=8) LL_ADC_REG_SetSequencerRanks(ADC1,GetRank(size),LL_ADC_CHANNEL_15);
	
	
	//LL_ADC_ClearFlag_ADRDY(ADC1);
	//LL_ADC_Enable(ADC1);
	//while(!LL_ADC_IsActiveFlag_ADRDY(ADC1)) asm("NOP");
}
//


void ADCController::SetSamplingTime(uint32_t sampling)
{
	this->sampling = sampling;
}
//

void ADCController::InitDMA()
{	
	LL_DMA_InitTypeDef dma;
	dma.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
	dma.MemoryOrM2MDstAddress = (uint32_t)meas;
	dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
	dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	dma.Mode = LL_DMA_MODE_CIRCULAR;
	dma.NbData = size*samples; //ADC_CHANNELS;
	dma.PeriphOrM2MSrcAddress = (uint32_t)&ADC1->DR;
	dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
	dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	dma.Priority = LL_DMA_PRIORITY_HIGH;
	dma.PeriphRequest = LL_DMAMUX_REQ_ADC1;
	LL_DMA_Init(init.dma,init.dma_channel,&dma);
	
	LL_DMA_EnableIT_TC(ADC_DMA,ADC_DMA_CH);
	LL_DMA_EnableChannel(init.dma,init.dma_channel);
	
}
//

void ADCController::InitMemory()
{
	meas = new uint16_t[samples*size];
	for(int i = 0;i<samples*size;++i) meas[i] = 0;
	
	results = new double[size];
	for(int i =0;i<size;++i) results[i] = 0;
}
//

void ADCController::EnableDmaInterrupt(bool state)
{
	if(state) EnableDmaIRQn(ADC_DMA,ADC_DMA_CH,1);
	else DisableDmaIRQn(ADC_DMA,ADC_DMA_CH);
}
//

uint32_t ADCController::RanksCounter()
{
	if(size == 1) return LL_ADC_REG_SEQ_SCAN_DISABLE;
	if(size == 2) return LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
	if(size == 3) return LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
	if(size == 4) return LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
	if(size == 5) return LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;
	if(size == 6) return LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
	if(size == 7) return LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS;
	if(size == 8) return LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
	
	return 0;
}
//

uint32_t ADCController::GetRank(uint8_t c)
{
	c=c+1;
	if(c == 1) return LL_ADC_REG_RANK_1;
	if(c == 2) return LL_ADC_REG_RANK_2;
	if(c == 3) return LL_ADC_REG_RANK_3;
	if(c == 4) return LL_ADC_REG_RANK_4;
	if(c == 5) return LL_ADC_REG_RANK_5;
	if(c == 6) return LL_ADC_REG_RANK_6;
	if(c == 7) return LL_ADC_REG_RANK_7;
	if(c == 8) return LL_ADC_REG_RANK_8;
	
	return 0;
}
//

void ADCController::Process(uint32_t ch)
{
	uint32_t meas_getter = 0;
	uint8_t ptr;
		
	for(ptr = 0;ptr<size;++ptr)
	{
		if(adc[ptr].ch_num == ch)break;
	}
	
	for(int j = 0;j < samples;++j)
	{
		meas_getter += meas[j*size+ptr];
	}
	meas_getter/=samples;

	switch(adc[ptr].type)
	{
		case ADC_TYPE_CURRENT:
			results[ptr] = ((meas_getter*inner_voltage_coeff + adc[ptr].offset)*adc[ptr].coeff)*1000.0;
			break;
		
		case ADC_TYPE_POS_RESISTANCE:
			results[ptr] = (adc[ptr].coeff*(4095.0/meas_getter - 1)) + adc[ptr].offset;
			break;
		
		case ADC_TYPE_NEG_RESISTANCE:
			results[ptr] = (adc[ptr].coeff/(4095.0/meas_getter - 1)) + adc[ptr].offset;
			break;
		
		case ADC_TYPE_VOLTAGE:
			results[ptr] = (meas_getter*inner_voltage_coeff  + adc[ptr].offset)*adc[ptr].coeff;
			break;
		
		case ADC_TYPE_INNER_VOLTAGE:
			results[ptr] = ((VREFINT_CAL_VREF*(*VREFINT_CAL_ADDR))/(meas_getter))/1000.0;
			inner_voltage = results[ptr];
			inner_voltage_coeff = inner_voltage/4095.0;
		break;
		
		case ADC_TYPE_RAW:
			results[ptr] = (meas_getter);
		break;
	}
}
//

void ADCController::ProcessAll()
{
	uint32_t *meas_getter = new uint32_t[size];
	for(int i = 0;i<size;++i)
	{
		meas_getter[i]  =0;
		for(int j = 0;j < samples;++j)
		{
			meas_getter[i] += meas[j*size+i];
		}
		meas_getter[i]/=samples;
	}
	
	for(int i = 0;i<size;++i)
	{
		switch(adc[i].type)
		{
			case ADC_TYPE_CURRENT:
				results[i] = ((meas_getter[i]*inner_voltage_coeff + adc[i].offset)*adc[i].coeff)*1000.0;
				break;
			
			case ADC_TYPE_POS_RESISTANCE:
				results[i] = (adc[i].coeff*(4095.0/meas_getter[i] - 1)) + adc[i].offset;
				break;
			
			case ADC_TYPE_NEG_RESISTANCE:
				results[i] = (adc[i].coeff/(4095.0/meas_getter[i] - 1)) + adc[i].offset;
				break;
			
			case ADC_TYPE_VOLTAGE:
				results[i] = (meas_getter[i]*inner_voltage_coeff + adc[i].offset)*adc[i].coeff;
				break;
			
			case ADC_TYPE_INNER_VOLTAGE:
				results[i] = ((VREFINT_CAL_VREF* (*VREFINT_CAL_ADDR))/(meas_getter[i]))/1000.0;
				inner_voltage = results[i];
				inner_voltage_coeff = inner_voltage/4095.0;
			break;
			
			case ADC_TYPE_RAW:
				results[i] = (meas_getter[i]);
			break;
		}
	}
	delete[] meas_getter;
}
//

void ADCController::ProcessInner()
{
	Process(flags.v_ptr);
}
//

double ADCController::getMeasure(uint32_t ch_num)
{
	for(int i = 0;i<size;++i)
	{
		if(adc[i].ch_num == ch_num) return results[i];
	}
	
	return 0xFF;
}
//

void ADCController::SwapChannels(uint8_t ch1, uint8_t ch2)
{
	ADC_Struct buf = adc[ch1];
	adc[ch1] = adc[ch2];
	adc[ch2] = buf;
}
//

void ADCController::SortLines()
{
	uint8_t minimal = 0;
	
	for(int i = 0;i<size;++i)
	{
		minimal = i;
		for(int j = i+1;j<size;++j)
		{
			if(adc[minimal].ch_num > adc[j].ch_num) minimal = j;
		}
		if(minimal != i) SwapChannels(i,minimal);
	}
	
	for(int i = 0;i<size;++i)
	{
		if(adc[i].adc_ch == LL_ADC_CHANNEL_TEMPSENSOR) flags.t_ptr = i;
		else if(adc[i].adc_ch == LL_ADC_CHANNEL_VBAT) flags.b_ptr = i;
		else if(adc[i].adc_ch == LL_ADC_CHANNEL_VREFINT) flags.v_ptr = i;
	}
}
//
