#include "adc_controller.h"
#include "stdlib.h"

ADCController::ADCController()
{
	/** set defaults */
	samples = 5; 
	inner_voltage = 3.3f;
	inner_voltage_coeff = inner_voltage/4095.0f;
	first_data_gained = false;
	sampling = LL_ADC_SAMPLINGTIME_56CYCLES;
}
//

ADCController::~ADCController()
{
	
}
//

void ADCController::AddLine(ADC_Struct str)
{
	/** adds new channel structure to the list */
	/*ADC_Struct *last_buf = adc; //gets pointer to the last buffer
	adc = new ADC_Struct[size+1]; // expands the old buffer
	for(int i = 0;i<size;++i)
	{
		adc[i] = last_buf[i]; //copy to new
	}
	if(size != 0) delete [] last_buf; //delete old*/

	/**If ADC is not initialized, init it*/
	if(size == 0) adc = new ADC_Struct[1];
	/**Else reallocate memory for new Initializer */
	else adc = reinterpret_cast<ADC_Struct*>(realloc(adc,(size+1)*sizeof(ADC_Struct)));

	adc[size] = str;
	adc[size].adc_ch = channel_mapping[adc[size].ch_num]; //add channel to the list
	size++;
}
//

void ADCController::AddInnerVoltageLine()
{
	/** Initializr struct and call AddLine */
	ADC_Struct new_adc;
	new_adc.adc_ch = LL_ADC_CHANNEL_VREFINT;
	new_adc.ch_num = V_REF_CH;
	new_adc.coeff = 1;
	new_adc.gpio = 0;
	new_adc.pin = 0;
	new_adc.type = ADC_TYPE_INNER_VOLTAGE;
	new_adc.offset = 0;
	AddLine(new_adc);
	flags.voltage = 1;
}
//

void ADCController::AddTempSensor()
{
	/** Initializr struct and call AddLine */
	ADC_Struct new_adc;
	new_adc.adc_ch = LL_ADC_CHANNEL_TEMPSENSOR;
	new_adc.ch_num = TEMP_CH;
	new_adc.coeff = 1;
	new_adc.gpio = 0;
	new_adc.pin = 0;
	new_adc.type = ADC_TYPE_INNER_TEMP;
	
	new_adc.offset = 0;
	AddLine(new_adc);
	flags.temp = 1;
}
//

void ADCController::Init(ADC_InitStruct in_str, uint8_t samples)
{
	/** Initialize peripheral */
	init = in_str;
	this->samples = samples;
	SortLines();
	InitMemory();
	InitGPIO();
	InitDMA();
	InitLines();
	
	/** Start endless readings */
	LL_ADC_REG_StartConversionSWStart(init.adc);
}
//

void ADCController::InitGPIO()
{	
	/** Initialize pins and set them to Analog */
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_ANALOG;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	for(int i = 0;i<size;++i)
	{
		gpio.Pin = adc[i].pin;
		if(adc[i].type != ADC_TYPE_INNER_VOLTAGE && adc[i].type != ADC_TYPE_INNER_TEMP) LL_GPIO_Init(adc[i].gpio,&gpio);
	}
}
//

void ADCController::InitLines()
{
	/** Enables inner channels (if enabled) */
	LL_ADC_SetCommonPathInternalCh(
	#if  defined(ADC1_COMMON)
		ADC1_COMMON
	#elif defined(ADC123_COMMON)
		ADC123_COMMON
	#endif
	,
	flags.voltage?LL_ADC_PATH_INTERNAL_VREFINT:0 | 
	flags.temp?LL_ADC_PATH_INTERNAL_TEMPSENSOR:0 | 
	flags.bat?LL_ADC_PATH_INTERNAL_VBAT:0);
	
	/** Sets ADCs clock */
	LL_ADC_SetCommonClock(
	#if  defined(ADC1_COMMON)
		ADC1_COMMON
	#elif defined(ADC123_COMMON)
		ADC123_COMMON
	#endif
	,
	LL_ADC_CLOCK_SYNC_PCLK_DIV2);
	
	/* Initialize ADC base functions */
	LL_ADC_InitTypeDef adc_ini;
	adc_ini.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	adc_ini.Resolution = LL_ADC_RESOLUTION_12B;
	adc_ini.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
	LL_ADC_Init(init.adc,&adc_ini);
	
	/** Initialize ADCs Regular channels */
	LL_ADC_REG_InitTypeDef reg;
	reg.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	reg.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	reg.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	reg.SequencerLength = RanksCounter();
	reg.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	LL_ADC_REG_Init(init.adc,&reg);
	
	/** Write channels to sequencer */
	for(int i = 0;i<size;++i)
	{
		LL_ADC_REG_SetSequencerRanks(init.adc,GetRank(i),adc[i].adc_ch);
		if(adc[i].type != ADC_TYPE_INNER_TEMP) LL_ADC_SetChannelSamplingTime(init.adc,adc[i].adc_ch,sampling);
		else if(sampling != LL_ADC_SAMPLINGTIME_3CYCLES) LL_ADC_SetChannelSamplingTime(init.adc,adc[i].adc_ch,sampling);
		else LL_ADC_SetChannelSamplingTime(init.adc,adc[i].adc_ch,LL_ADC_SAMPLINGTIME_28CYCLES);
		
	}
	
	/** Enables peripheral */
	LL_ADC_Enable(init.adc);
}
//

void ADCController::SetSamplingTime(uint32_t sampling)
{
	this->sampling = sampling;
}
//

void ADCController::InitDMA()
{	
	/** Enables DMA. From ADC to RAW array (meas) Alligned by WORD */
	LL_DMA_InitTypeDef dma;
	dma.Channel = init.dma_channel;
	dma.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
	dma.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
	dma.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
	dma.MemBurst = LL_DMA_MBURST_SINGLE;
	dma.MemoryOrM2MDstAddress = (uint32_t)meas;
	dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
	dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	dma.Mode = LL_DMA_MODE_CIRCULAR;
	dma.NbData = size*samples; //ADC_CHANNELS;
	dma.PeriphBurst = LL_DMA_PBURST_SINGLE;
	dma.PeriphOrM2MSrcAddress = (uint32_t)&init.adc->DR;
	dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
	dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	dma.Priority = LL_DMA_PRIORITY_MEDIUM;
	LL_DMA_Init(init.dma,init.dma_stream,&dma);
	
	/** Enables Interrupt and Stream */
	LL_DMA_EnableIT_TC(init.dma,init.dma_stream);
	LL_DMA_EnableStream(init.dma,init.dma_stream);
	
}
//

void ADCController::InitMemory()
{
	/** Allocate meas (RAW) array */
	meas = new uint32_t[samples*size];
	for(int i = 0;i<samples*size;++i) meas[i] = 0;
	
	/** Allocate result (Counted) array */
	results = new float[size];
	for(int i =0;i<size;++i) results[i] = 0;
	
	/** Allocate averaging array (For ProcessAll) */
	meas_getter = new uint32_t[size];
}
//

void ADCController::EnableDmaInterrupt(bool stat)
{
	/** Enables or Disables IRQ_Handler */
	if(stat) EnableDmaIRQn(init.dma,init.dma_stream,1);
	else DisableDmaIRQn(init.dma,init.dma_stream);
}
//

uint32_t ADCController::RanksCounter()
{
	/** Get Sequencer Ranks based on Channel List size */
	if(size == 1) return LL_ADC_REG_SEQ_SCAN_DISABLE;
	if(size == 2) return LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
	if(size == 3) return LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
	if(size == 4) return LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
	if(size == 5) return LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;
	if(size == 6) return LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
	if(size == 7) return LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS;
	if(size == 8) return LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
	if(size == 9) return LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS;
	if(size == 10) return LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS;
	if(size == 11) return LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS;
	if(size == 12) return LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS;
	if(size == 13) return LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS;
	if(size == 14) return LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS;
	
	return 0;
}
//

uint32_t ADCController::GetRank(uint8_t c)
{
	/** Sets current channel rank */
	c=c+1;
	if(c == 1) return LL_ADC_REG_RANK_1;
	if(c == 2) return LL_ADC_REG_RANK_2;
	if(c == 3) return LL_ADC_REG_RANK_3;
	if(c == 4) return LL_ADC_REG_RANK_4;
	if(c == 5) return LL_ADC_REG_RANK_5;
	if(c == 6) return LL_ADC_REG_RANK_6;
	if(c == 7) return LL_ADC_REG_RANK_7;
	if(c == 8) return LL_ADC_REG_RANK_8;
	if(c == 9) return LL_ADC_REG_RANK_9;
	if(c == 10) return LL_ADC_REG_RANK_10;
	if(c == 11) return LL_ADC_REG_RANK_11;
	if(c == 12) return LL_ADC_REG_RANK_12;
	if(c == 13) return LL_ADC_REG_RANK_13;
	if(c == 14) return LL_ADC_REG_RANK_14;
	
	return 0;
}
//

void ADCController::Process(uint32_t ch)
{
	/** Process Single channel */
	uint8_t ptr;
	/** Gets channel position in list */
	for(ptr = 0;ptr<size;++ptr)
	{
		if(adc[ptr].ch_num == ch)break;
	}
	
	/** Average samples from channel */
	meas_getter[0] = 0;
	for(int j = 0;j < samples;++j)
	{
		meas_getter[0] += meas[j*size+ptr];
	}
	meas_getter[0]/=samples;

	/** Count channel by type */
	switch(adc[ptr].type)
	{
		case ADC_TYPE_CURRENT:
			/**((raw*3.3/4095)+offset)*coeff */
			results[ptr] = ((meas_getter[0]*inner_voltage_coeff + adc[ptr].offset)*adc[ptr].coeff); //mA
			break;
		
		case ADC_TYPE_POS_RESISTANCE:
			/** (coeff*(4095/raw - 1) + offset) */
			results[ptr] = (adc[ptr].coeff*(4095.0f/meas_getter[0] - 1)) + adc[ptr].offset; //Ohms
			break;
		
		case ADC_TYPE_NEG_RESISTANCE:
			/** (coeff/(4095/raw - 1) + offset) */
			results[ptr] = (adc[ptr].coeff/(4095.0f/meas_getter[0] - 1)) + adc[ptr].offset; //Ohms
			break;
		
		case ADC_TYPE_VOLTAGE:
			/** (raw*3.3/4095 + offset)*coeff */
			results[ptr] = (meas_getter[0]*inner_voltage_coeff  + adc[ptr].offset)*adc[ptr].coeff; //V
			break;
		
		case ADC_TYPE_INNER_VOLTAGE:
			/** See datasheet */
			results[ptr] = ((VREFINT_CAL_VREF* (*VREFINT_CAL_ADDR))/(meas_getter[0]))/1000.0f; //V
			inner_voltage = results[ptr]; //V
			inner_voltage_coeff = inner_voltage/4095.0f;
		break;
		
		case ADC_TYPE_INNER_TEMP:		
			/**See Datasheet */
			results[ptr] = ((int32_t)(meas_getter)-(*TEMPSENSOR_CAL1_ADDR))*(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)/(float)((*TEMPSENSOR_CAL2_ADDR) - (*TEMPSENSOR_CAL1_ADDR)) + TEMPSENSOR_CAL1_TEMP;//degC
		break;
		
		case ADC_TYPE_RAW:
		/** Nothing to do */
			results[ptr] = (meas_getter[0]);
		break;
	}
}
//

void ADCController::ProcessAll()
{
	/** See Process(), but averaging and counting every channel */
	if(samples != 1)
	{
		for(int i = 0;i<size;++i)
		{
			meas_getter[i]  =0;
			for(int j = 0;j < samples;++j)
			{
				meas_getter[i] += meas[j*size+i];
			}
			meas_getter[i]/=samples;
		}
	}
	else
	{
		for(int i = 0;i<size;++i) meas_getter[i] = meas[i];
	}
	
	for(int i = 0;i<size;++i)
	{
		switch(adc[i].type)
		{
			case ADC_TYPE_CURRENT:
				results[i] = ((meas_getter[i]*inner_voltage_coeff + adc[i].offset)*adc[i].coeff);
				break;
			
			case ADC_TYPE_POS_RESISTANCE:
				results[i] = (adc[i].coeff*(4095.0f/meas_getter[i] - 1)) + adc[i].offset;
				break;
			
			case ADC_TYPE_NEG_RESISTANCE:
				results[i] = (adc[i].coeff/(4095.0f/meas_getter[i] - 1)) + adc[i].offset;
				break;
			
			case ADC_TYPE_VOLTAGE:
				results[i] = (meas_getter[i]*inner_voltage_coeff + adc[i].offset)*adc[i].coeff;
				break;
			
			case ADC_TYPE_INNER_VOLTAGE:
				results[i] = ((VREFINT_CAL_VREF* (*VREFINT_CAL_ADDR))/(meas_getter[i]))/1000.0f;
				inner_voltage = results[i];
				inner_voltage_coeff = inner_voltage/4095.0f;
			break;
			
			case ADC_TYPE_INNER_TEMP:
			results[i] = ((meas_getter[i])-(int32_t)*TEMPSENSOR_CAL1_ADDR)*(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)/((int32_t)*TEMPSENSOR_CAL2_ADDR - (int32_t)*TEMPSENSOR_CAL1_ADDR)+ TEMPSENSOR_CAL1_TEMP;
		break;
			
			case ADC_TYPE_RAW:
				results[i] = (meas_getter[i]);
			break;
		}
	}
}
//

void ADCController::ProcessInner()
{
	Process(V_REF_CH);
}
//

void ADCController::ProcessTemp()
{
	Process(TEMP_CH);
}
//

float ADCController::getMeasure(uint8_t adc_num)
{
	/** find corresponding channel number and return */
	for(int i = 0;i<size;++i)
	{
		if(adc[i].ch_num == adc_num) return results[i];
	}
	
	/** if nothing - return 0 */
	return 0;
}
//

float ADCController::GetTemp()
{
	return getMeasure(TEMP_CH);
}
//

float *ADCController::GetPointerToChannel(uint8_t ch)
{
	/** find corresponding channel number and return pointer to result */
	for(int i = 0;i<size;++i)
	{
		if(adc[i].ch_num == ch) return &results[i];
	}
	
	/**or return 0 */
	return 0x00;
}
//


uint32_t *ADCController::GetPointerToMeas(uint8_t ch_num)
{
	/** find corresponding channel number and return pointer to meas */
	for(int i = 0;i<size;++i)
	{
		if(adc[i].ch_num == ch_num) return &meas[i];
	}
	
	/**or return 0 */
	return 0x00;
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
	
	/**UNEFFECTIVELY Sort by Channel number */

	for(int i = 0;i<size;++i)
	{
		minimal = i;
		for(int j = i+1;j<size;++j)
		{
			if(adc[minimal].ch_num > adc[j].ch_num) minimal = j;
		}
		if(minimal != i) SwapChannels(i,minimal);
	}
}
//
