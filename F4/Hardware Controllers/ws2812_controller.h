#ifndef WS2812_CONTROLLER_H
#define WS2812_CONTROLLER_H

#include "defines.h"

#define FULL_CYCLE	1250 //in nsec (WS2812 1250, UCS - 2500)

struct WS_TypeDef
{
	GPIO_TypeDef* ws_gpio;
	uint32_t 			ws_pin;
	uint32_t			ws_af;
	TIM_TypeDef*	ws_tim;
	uint32_t			ws_tim_ch;
	DMA_TypeDef*	ws_dma;
	uint32_t			ws_dma_stream;
	uint32_t 			ws_dma_channel;
	uint16_t			pixels;
};

struct Color
{
	Color(uint8_t r,uint8_t g, uint8_t b) {red=r;green=g,blue=b;};
	Color(){red=0;green=0;blue=0;};
	bool operator!=(/*const Color& left, */const Color &right)
	{
		if(this->red != right.red || this->green != right.green || this->blue != right.blue) return true;
		else return false;
	}
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

class WS2812Controller
{
    public:
			WS2812Controller();
			~WS2812Controller();
			void Init(WS_TypeDef);
			void SetPixelColor(uint8_t number,uint8_t red, uint8_t green, uint8_t blue);
			void SetPixelColor(uint8_t number,Color color);
			void Colorize();
			bool IsWriting();

    private:
			void InitGPIO();
			void InitTIM();
			void InitDMA();
			WS_TypeDef ws;
			uint16_t* bittime_stream;
			Color* colors_new;
			Color* colors_old;
};


#endif
