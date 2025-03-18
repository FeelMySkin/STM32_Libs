/**
 * @file ws2812_controller.h
 * @author Phil (zetsuboulevel@gmail.com)
 * @brief Hardware Controller for WS2812-based smart LEDs.
 * Based on PWM and DMA* (Sending CPU-independent)
 * @version 0.1
 * @date 2022-09-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef WS2812_CONTROLLER_H
#define WS2812_CONTROLLER_H

#include "periph_functions.h"

/**Hard defined PWM Frequency in nsec. Need to change */
#define FULL_CYCLE	1250 //in nsec (WS2812 1250, UCS - 2500)

/**
 * @brief bit length of the smart LED.
 *
 */
enum WS_BitWidth
{
	WS_4_BIT = 0x01,
	WS_8_BIT = 0x02
};

/**
 * @brief enum for colors positions (RGB, RBG...)
 *
 */
enum WS_ColorPosition
{
	WS_RGB,
	WS_RBG,
	WS_GRB,
	WS_GBR,
	WS_BRG,
	WS_BGR
};

/**
 * @brief Initializing struct.
 * 
 */
struct WS_TypeDef
{
	GPIO_TypeDef*		ws_gpio; /** Set GPIO* */
	uint32_t 			ws_pin; /** set LL_GPIO_PIN_* */
	uint32_t			ws_af; /** set LL_GPIO_AF_* for TIM* PWM */
	TIM_TypeDef*		ws_tim; /** set TIM* */
	uint32_t			ws_tim_ch; /** set LL_TIM_CHANNEL_CH* */
	DMA_TypeDef*		ws_dma; /** set DMA* */
	uint32_t 			ws_dma_channel; /** set LL_DMA_CHANNEL_CH* */
	uint16_t			pixels; /** set number of pixels */
	WS_BitWidth			bit_width; /** bit width */
	WS_ColorPosition	col_pos; /** colors shift */
};

/**
 * @brief Color struct. (or class). Can set RGB and compare 2 objects 
 * 
 */
struct Color
{
	Color(uint8_t r,uint8_t g, uint8_t b) {red=r;green=g,blue=b;}; /** constructor with initializers */
	Color(){red=0;green=0;blue=0;}; /** Default constructor */
	bool operator!=(/*const Color& left, */const Color &right) /** operator != override */
	{
		if(this->red != right.red || this->green != right.green || this->blue != right.blue) return true; /** compare 2 colors */
		else return false;
	}
	bool operator==(/*const Color& left, */const Color &right) /** operator == override */
	{
		if(this->red == right.red || this->green == right.green || this->blue == right.blue) return true; /** compare 2 colors */
		else return false;
	}
	uint8_t red; /** red channel 0-256 */
	uint8_t green; /** green channel 0-256 */
	uint8_t blue; /** blue channel 0-256 */
};

/**
 * @brief WS2812 Controller class
 * 
 */
class WS2812Controller
{
    public:
		WS2812Controller();
		~WS2812Controller();
		/**
		 * @brief Initialize Class.
		 * 
		 * @param ws WS_TypeDef structure.
		 */
		void Init(WS_TypeDef ws);

		/**
		 * @brief Set the Pixel Color by RGB
		 * 
		 * @param number pixel position
		 * @param red red channel
		 * @param green green channel
		 * @param blue blue channel
		 */
		void SetPixelColor(uint8_t number,uint8_t red, uint8_t green, uint8_t blue);

		/**
		 * @brief Set the Pixel Color by Color structure
		 * 
		 * @param number pixel position
		 * @param color Color object
		 */
		void SetPixelColor(uint8_t number,Color color);

		/**
		 * @brief Redraw pixels. (If needed)
		 * 
		 */
		void Colorize();

		/**
		 * @brief Check if colorizing operation is ongoing.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool IsWriting();

    private:
		/**
		 * @brief Initialize GPIO
		 * 
		 */
		void InitGPIO();

		/**
		 * @brief Initialize Timer
		 * 
		 */
		void InitTIM();

		/**
		 * @brief Initialize DMA
		 * 
		 */
		void InitDMA();
	
		/**
		 * @brief Set pixel with changed pixel rightly position
		 * @param num pixel position
		 * @param color Color object
		 *
		 */
		void Pixelize(uint8_t num, Color color);

		WS_TypeDef ws; /** WS_TypeDef object */
		uint16_t* bittime_stream; /** stream to DMA. COntains timings for every bit */
		Color* colors_new; /** New colors data */
		Color* colors_old; /** Old colors data */
		uint16_t zero_bittime, one_bittime; /** for fast-counting */
};


#endif
