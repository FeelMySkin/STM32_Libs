/**
 * @file gps_controller.h
 * @author Phil (zetsuboulevel@gmail.com)
 * @brief GPS Controller C++ Class. Unparses RMC structure only.
 * Uses UsartController Class.
 * @version 0.1
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef GPS_CONTROLLER_H
#define GPS_CONTROLLER_H

#include "periph_functions.h"
#include "help_funcitons.h"
#include "usart_controller.h"


/**
 * @brief GPS RMC (Recommended Minimum sentence C) Packet structure
 * 
 */
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
		/**
		 * @brief Intialize class. It uses external USART.
		 * 
		 * @param usart set pointer to UsartController class
		 */
		void Init(UsartController*	usart);

		/**
		 * @brief Process received data.
		 * 
		 */
		void Process();

		/**
		 * @brief Not used for now.
		 * 
		 */
		void PushBuffer();

		/**
		 * @brief Cal this function every ms to count desync timer.
		 * After desync time should be taken from another source.
		 * 
		 */
		void ProcessSecond();
	
		RMC_Packet rmc; /** rmc structure with data */
	
	private:
		/**
		 * @brief Parse RMC data from usart buffer
		 * 
		 */
		void ProcessRMC();

		/**
		 * @brief Count time to new GMT.
		 * 
		 */
		void ProcessTime();
	
		char rmc_buffer[100]; /** write here rmc data to parse */
		volatile bool completed; /** flag that data reception is completed */
		volatile uint8_t pointer; /**If not all data loaded from USART - set current pointer to rmc_buffer */
		uint8_t bad_baud_tim; /** counter to change baudrate if data not receiving properly */
		uint32_t timeout; /**desync timerout */
		UsartController*	usart; /** POinter to UsartController class */
};

#endif
