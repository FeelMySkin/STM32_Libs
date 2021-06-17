#ifndef GPS_CONTROLLER_H
#define GPS_CONTROLLER_H

#include "defines.h"
#include "cyclic_buffer.h"


struct GPS_InitTypeDef
{
	GPIO_TypeDef*	tx_gpio;
	GPIO_TypeDef*	rx_gpio;
	uint32_t		tx_pin;
	uint32_t		rx_pin;
	USART_TypeDef*	uart;
	uint32_t		uart_af;
};
//


struct RMC_Packet
{
	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t hours;
	uint16_t mins;
	uint16_t sec;
	uint16_t msec;
	double lattitude;
	double longitude;
	uint8_t signal_level;
};
//
		
		
		
class GPS_Controller
{
	public:
		GPS_Controller();
		void Init(GPS_InitTypeDef);
		void Process();
		void PushBuffer();
		void ProcessSecond();
	
		RMC_Packet rmc;
		Cyclic_Buffer<char,512> full_buffer;
	
	private:
		void InitGPIO();
		void InitUART();
		void ProcessRMC();
		void ProcessTime();
	
		char buffer;
		char rmc_buffer[100];
		volatile bool completed;
		volatile uint8_t pointer;
		bool test_bool;
		uint32_t timeout;
		GPS_InitTypeDef gps;
};

#endif
