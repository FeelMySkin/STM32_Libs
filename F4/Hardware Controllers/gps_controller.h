#ifndef GPS_CONTROLLER_H
#define GPS_CONTROLLER_H

#include "defines.h"
#include "usart_controller.h"


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
		void Init(UsartController*	usart);
		void Process();
		void PushBuffer();
		void ProcessSecond();
	
		RMC_Packet rmc;
	
	private:
		void ProcessRMC();
		void ProcessTime();
	
		char buffer;
		char rmc_buffer[100];
		volatile bool completed;
		volatile uint8_t pointer;
		bool test_bool;
		uint32_t timeout;
		UsartController*	usart;
};

#endif
