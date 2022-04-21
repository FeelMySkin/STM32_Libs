#include "ws2812_controller.h"

WS2812Controller::WS2812Controller()
{

}
//

WS2812Controller::~WS2812Controller()
{

}
//

void WS2812Controller::Init(WS_TypeDef ws)
{
	this->ws = ws;
	bittime_stream = new uint16_t[ws.pixels*24+2];
	bittime_stream[0] = 0x00;
	bittime_stream[24*ws.pixels+1] = 0x00;
	colors_old = new Color[ws.pixels];
	colors_new = new Color[ws.pixels];
	colors_old[0] = Color(1,0,0);
	InitGPIO();
	InitTIM();
	InitDMA();
	for(int i = 0;i<1000000;++i) asm("NOP");
	Colorize();
}
//

void WS2812Controller::InitDMA()
{
	LL_DMA_InitTypeDef dma;
	dma.Channel = ws.ws_dma_channel;
	dma.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	dma.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
	dma.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
	dma.MemBurst = LL_DMA_MBURST_SINGLE;
	dma.MemoryOrM2MDstAddress = (uint32_t)bittime_stream;
	dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
	dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	dma.Mode = LL_DMA_MODE_NORMAL;
	dma.NbData = 0;
	dma.PeriphBurst = LL_DMA_PBURST_SINGLE;
	if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH1) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR1;
	else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH2) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR2;
	else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH3) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR3;
	else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH4) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR4;
	dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
	dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	dma.Priority = LL_DMA_PRIORITY_HIGH;
	LL_DMA_Init(ws.ws_dma,ws.ws_dma_stream,&dma);
	
	
}
//

void WS2812Controller::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = ws.ws_af;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = ws.ws_pin;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(ws.ws_gpio,&gpio);
}
//

void WS2812Controller::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = FULL_CYCLE/(1000000000/SystemCoreClock);
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERDIRECTION_UP;
	tim.Prescaler = 0;
	tim.RepetitionCounter = 1;
	LL_TIM_Init(ws.ws_tim,&tim);
	
	LL_TIM_OC_InitTypeDef oc;
	oc.CompareValue = 0;
	oc.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	oc.OCMode = LL_TIM_OCMODE_PWM1;
	oc.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	oc.OCState = LL_TIM_OCSTATE_DISABLE;
	LL_TIM_OC_Init(ws.ws_tim,ws.ws_tim_ch,&oc);
	
	LL_TIM_DisableAllOutputs(ws.ws_tim);
	LL_TIM_OC_EnablePreload(ws.ws_tim,ws.ws_tim_ch);
	LL_TIM_CC_EnableChannel(ws.ws_tim,ws.ws_tim_ch);
	LL_TIM_EnableCounter(ws.ws_tim);
	
	//if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH1) LL_TIM_EnableDMAReq_CC1(ws.ws_tim);
	//else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH2) LL_TIM_EnableDMAReq_CC2(ws.ws_tim);
	//else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH3) LL_TIM_EnableDMAReq_CC3(ws.ws_tim);
	//else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH4) LL_TIM_EnableDMAReq_CC4(ws.ws_tim);
	
	LL_TIM_EnableDMAReq_UPDATE(ws.ws_tim);
}
//

void WS2812Controller::SetPixelColor(uint8_t number,uint8_t red, uint8_t green, uint8_t blue)
{
	SetPixelColor(number,Color(red,green,blue));
}
//

void WS2812Controller::SetPixelColor(uint8_t number, Color color)
{
	colors_new[number]=color;
}
//

void WS2812Controller::Colorize()
{
	bool finalize = false;
	for(int i = 0;i<ws.pixels;++i)
	{
		if(colors_old[i] != colors_new[i]) finalize = true;
	}
	
	if(finalize)
	{
		for(int i = 0;i<ws.pixels;++i)
		{
			for(int j = 0;j<8;++j)
			{
				bittime_stream[24*i+j+1] = (colors_new[i].red>>(7-j))&1?ws.ws_tim->ARR*4/5:ws.ws_tim->ARR*1/5; //*4/5 or /5
				bittime_stream[24*i+8+j+1] = (colors_new[i].green>>(7-j))&1?ws.ws_tim->ARR*4/5:ws.ws_tim->ARR*1/5;
				bittime_stream[24*i+16+j+1] = (colors_new[i].blue>>(7-j))&1?ws.ws_tim->ARR*4/5:ws.ws_tim->ARR*1/5;
			}
			colors_old[i]=colors_new[i];
		}
		ClearDmaTCFlag(ws.ws_dma,ws.ws_dma_stream);
		LL_DMA_SetDataLength(ws.ws_dma,ws.ws_dma_stream,24*ws.pixels+2);
		LL_DMA_EnableStream(ws.ws_dma,ws.ws_dma_stream);
		
	}
	
}
//

bool WS2812Controller::IsWriting()
{
	return LL_DMA_GetDataLength(ws.ws_dma,ws.ws_dma_stream)==0?false:true;
}
//
