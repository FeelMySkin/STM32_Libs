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
	/** Set WS_TypeDef to class object. */
	this->ws = ws;

	/** Initialize memory with 24bit*pixels + start and stop bit (for TIM sync).
	 * And set first and last bits to 0
	 */
	bittime_stream = new uint16_t[ws.pixels*3*4*ws.bit_width+2];
	bittime_stream[0] = 0x00;
	bittime_stream[3*4*ws.bit_width*ws.pixels+1] = 0x00;
	/** Set old color to 0 (to draw first 0-row) */
	colors_old = new Color[ws.pixels];
	colors_new = new Color[ws.pixels];
	colors_old[0] = Color(1,0,0);

	/**Initialize peripherals */
	InitGPIO();
	InitTIM();
	InitDMA();
	
	/**Count timings and Colorize 
	 * *4/5 (for 1 bit) or *1/5 (for 0 bit)
	*/
	one_bittime =  ws.ws_tim->ARR*4/5;
	zero_bittime = ws.ws_tim->ARR*1/5;
	//for(int i = 0;i<1000000;++i) asm("NOP");
	Colorize();
}
//

void WS2812Controller::InitDMA()
{

	/** Initialize DMA as TIM_CC selected channel.
	 * MEMORY to PERIPH, Halfword 
	*/
	LL_DMA_InitTypeDef dma;
	dma.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	dma.MemoryOrM2MDstAddress = (uint32_t)bittime_stream;
	dma.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
	dma.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	dma.Mode = LL_DMA_MODE_NORMAL;
	dma.NbData = 0;
	if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH1) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR1;
	else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH2) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR2;
	else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH3) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR3;
	else if(ws.ws_tim_ch == LL_TIM_CHANNEL_CH4) dma.PeriphOrM2MSrcAddress = (uint32_t)&ws.ws_tim->CCR4;
	dma.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
	dma.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	dma.Priority = LL_DMA_PRIORITY_HIGH;

	LL_DMA_Init(ws.ws_dma,ws.ws_dma_channel,&dma);
	
	
}
//

void WS2812Controller::InitGPIO()
{
	/** Initialize GPIO as ALTERNATE PUSHPULL on TIM* */
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
	/**Set TIM* Prescaler to 0 with Cycle Time */
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = FULL_CYCLE/(1000000000/SystemCoreClock);
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERDIRECTION_UP;
	tim.Prescaler = 0;
	tim.RepetitionCounter = 0;
	LL_TIM_Init(ws.ws_tim,&tim);
	
	/** Initialize PWM as PWM1 */
	LL_TIM_OC_InitTypeDef oc;
	oc.CompareValue = 0;
	oc.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	oc.OCMode = LL_TIM_OCMODE_PWM1;
	oc.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	oc.OCState = LL_TIM_OCSTATE_DISABLE;
	LL_TIM_OC_Init(ws.ws_tim,ws.ws_tim_ch,&oc);
	
	/** Enable Preload on channel and Enable Channel */
	LL_TIM_DisableAllOutputs(ws.ws_tim);
	LL_TIM_OC_EnablePreload(ws.ws_tim,ws.ws_tim_ch);
	LL_TIM_CC_EnableChannel(ws.ws_tim,ws.ws_tim_ch);
	LL_TIM_EnableCounter(ws.ws_tim);
	LL_TIM_EnableAllOutputs(ws.ws_tim);
	
	/** Enable DMA by UPDATE event */
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
	Color col;
	switch (ws.col_pos)
	{
		case WS_RGB:
			col = Color(color.red, color.green, color.blue);
		break;
		
		case WS_RBG:
			col = Color(color.red, color.blue, color.green);
		break;
		
		case WS_GRB:
			col = Color(color.green, color.red, color.blue);
		break;
		
		case WS_GBR:
			col = Color(color.green, color.blue, color.red);
		break;
		
		case WS_BGR:
			col = Color(color.blue, color.green, color.red);
		break;
		
		case WS_BRG:
			col = Color(color.blue, color.red, color.green);
		break;
			
	}
	Pixelize(number, col);
}
//

void WS2812Controller::Pixelize(uint8_t num, Color color)
{
	for(int j = 0;j<4*ws.bit_width;++j)
	{
		/** count for evey channel */
		bittime_stream[3*4*ws.bit_width*num+j+1] = (color.red>>(4*ws.bit_width-1-j))&1?one_bittime:zero_bittime;
		bittime_stream[3*4*ws.bit_width*num+4*ws.bit_width+j+1] = (color.green>>(4*ws.bit_width-1-j))&1?one_bittime:zero_bittime;
		bittime_stream[3*4*ws.bit_width*num+2*4*ws.bit_width+j+1] = (color.blue>>(4*ws.bit_width-1-j))&1?one_bittime:zero_bittime;
	}
}

void WS2812Controller::Colorize()
{
	/** Check if pixels REALLY needs a redraw */
	bool finalize = false;
	for(int i = 0;i<ws.pixels;++i)
	{
		if(colors_old[i] != colors_new[i]) finalize = true;
	}
	
	/** If needs - count every bit's timing */
	if(finalize)
	{
		for(int i = 0;i<ws.pixels;++i)
		{
			/*for(int j = 0;j<8;++j)
			{
				bittime_stream[24*i+j+1] = (colors_new[i].red>>(7-j))&1?one_bittime:zero_bittime;
				bittime_stream[24*i+8+j+1] = (colors_new[i].green>>(7-j))&1?one_bittime:zero_bittime;
				bittime_stream[24*i+16+j+1] = (colors_new[i].blue>>(7-j))&1?one_bittime:zero_bittime;
			}*/
			colors_old[i]=colors_new[i];
		}

		/** Set length and enable DMA */
		LL_DMA_DisableChannel(ws.ws_dma, ws.ws_dma_channel);
		ClearDmaTCFlag(ws.ws_dma,ws.ws_dma_channel);
		LL_DMA_SetDataLength(ws.ws_dma,ws.ws_dma_channel,3*4*ws.bit_width*ws.pixels+2);
		LL_DMA_EnableChannel(ws.ws_dma,ws.ws_dma_channel);
		
	}
	
}
//

bool WS2812Controller::IsWriting()
{
	return LL_DMA_GetDataLength(ws.ws_dma,ws.ws_dma_channel)==0?false:true;
}
//
