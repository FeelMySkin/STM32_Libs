/**
 * @file dali_controller.h
 * @author Phil (zetsuboulevel@gmail.com)
 * @brief DALI (Digital Addressable Lighting Interface) Hardware Controller.
 * Only can send and receive data. 
 * Manchester encoding/decoding.
 * @version 0.1
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef DALI_CONTROLLER_H
#define DALI_CONTROLLER_H

#include "periph_functions.h"

#define DALI_HYST 			200 /** HYSTERESIS in timings */
#define DEBUG_RECEIVER /**Enables DEBUG buffer */
#define DALI_BAUDS_COUNT	1 /** Number of BAUDS */

const uint16_t DALI_BAUDS[DALI_BAUDS_COUNT] = {1200}; /** Some baud rates that can be used (not all of them can work) */

/**
 * @brief TX pin logic
 * 
 */
enum DALI_LOGIC
{
	DALI_LOGIC_POSITIVE, /** GND on pin reset */
	DALI_LOGIC_NEGATIVE  /** GND on pin set */
};

/**
 * @brief DALI RX Working type
 * 
 */
enum DALI_WORK_TYPE
{
	DALI_WORK_IC, /** TIM and IC based work */
	DALI_WORK_EXTI, /** External Interrupt based work */
	DALI_WORK_SAMPLING_IC, /** TIM and IC based work for sending and sampling for receive */
	DALI_WORK_SAMPLING_EXTI, /** External Interrupt based work for sending and sampling for receive */
};

/**
 * @brief Type of received pulses from RX 
 * 
 */
enum DALI_RECEIVED_PULSE
{
	DALI_PULSE_HALFBIT,
	DALI_PULSE_FULLBIT,
	DALI_PULSE_STOP,
	DALI_PULSE_ERROR
};

/**
 * @brief DALI Initializer
 * 
 */
struct DALI_InitTypeDef
{
	GPIO_TypeDef* 		dali_tx_gpio; /** set GPIO* for TX pin */
	GPIO_TypeDef* 		dali_rx_gpio; /** set GPIO* for RX pin */
	uint16_t 			dali_tx_pin; /** set LL_GPIO_PIN_* for TX pin */
	uint16_t 			dali_rx_pin; /** set LL_GPIO_PIN_* for RX pin */
	TIM_TypeDef*		dali_tim; /** Set TIM* for work */
	uint32_t			dali_rx_ch; /** Set corresponding TIM_CH (If DALI_IC Type) */
	uint32_t			dali_af; /** Set RX Pin LL_GPIO_AF_* for TIM* Channel (If DALI_IC Type) */
	TIM_TypeDef*		kz_tim; /** Set another TIM* to check if long zero */
	uint32_t 			callback_line; /** Set LL_EXTI_LINE_* for receive callback (if needed) */
	DALI_LOGIC			logic:2; /** Set DALI_LOGIC */
	DALI_WORK_TYPE		work_type:2; /** Set DALI Type */
};

/**
 * @brief DALI Hardware Controller class. 
 * Only can send and receive data.
 * 
 */
class DaliController
{
	public:
		DaliController();
		~DaliController();
		/**
		 * @brief Initialize DALI Class
		 * @param str Initializer Struct
		 */
		void Init(DALI_InitTypeDef str);

		/**
		 * @brief DALI data sender method.
		 * 
		 * @param mess message to send
		 * @param n_bits Number of bits (by default 8, 16, 24)
		 * @param baud custom baudrate. Take it from DALI_BAUDS array (default 1200).
		 */
		void Send(uint32_t mess,uint8_t n_bits, uint16_t baud = DALI_BAUDS[0]);

		/**
		 * @brief DALI data sender method with delay.
		 * Actually, not used
		 * 
		 * @param mess message to send
		 * @param n_bits Number of bits (by default 8, 16, 24)
		 * @param delay Send delay (in us, default 5000)
		 * @param baud custom baudrate. Take it from DALI_BAUDS array (default 1200).
		 */
		void SendDelayed(uint32_t mess, uint8_t n_bits, uint32_t delay = 5000, uint16_t baud = DALI_BAUDS[0]);

		/**
		 * @brief Process incoming data
		 * 
		 * @param tim_flag true - Process from TIM IRQ, false = Process from EXTI IRQ. 
		 * Call it from TIM and (DALI_EXTI type) EXTI IRQHandler.
		 */
		void Process(bool tim_flag);

		/**
		 * @brief Enables/Disables Receive callback
		 * 
		 * @param state (TRUE/FALSE)
		 */
		void EnableRecvInterrupt(bool state);

		/**
		 * @brief Enables/Disables Shirt Circuit (No DALI Line Voltage) checking.
		 * 
		 * @param state (TRUE/FALSE)
		 */
		void EnableKZCheck(bool state);
		/**
		 * @brief Start listetning on RX pin
		 * 
		 */
		void StartReceiving();
		/**
		 * @brief Stop listenings on RX pin
		 * 
		 */
		void StopReceiving();

		/**
		 * @brief Check if Shirt circuit  (No DALI Line Voltage).
		 * 
		 */
		void CheckKZ();

		/**
		 * @brief Process IC (If DALI_IC Type). 
		 * Call it from TIM_IRQHandler.
		 * 
		 */
		void ProcessIC();
	
		bool sending,receiving,delayed; /** is sending, is receiving, is delayed sending flags */
		bool send_completed,receive_completed; /** send or receive completed flags */
		bool kz_state; /** is short circuit flag */
		uint32_t received; /** received message */
		uint8_t recv_bytes; /** received message length */
		//uint16_t last_baud;
	
	private:
		/**
		 * @brief Initialize GPIO pins for RX and TX (and EXTI if DALI_EXTI type)
		 * 
		 */
		void InitGPIO();

		/**
		 * @brief Initialize TIM* for DALI (and TIM_IC if DALI_IC type)
		 * 
		 */
		void InitTIM();
		/**
		 * @brief Start Sending message on TX pin
		 * 
		 */
		void StartSending();

		/**
		 * @brief Decode received data (from manchester to byte)
		 * 
		 */
		void ReadData();

		/**
		 * @brief Set LOW on TX pin
		 * 
		 */
		void SetLow();

		/**
		 * @brief Set HIGH on TX pin
		 * 
		 */
		void SetHigh();
	
		DALI_InitTypeDef dali; /** DALI initializer object */
	
		uint8_t recv_cnt; /** number of received timings on RX pin */
		uint8_t delay_cnt; /** delay on transmit */
		uint16_t recv_buf[80]; /** received timings on RX pin */
	
		uint16_t send_buf[80]; /** Timings for to-send data */
		uint8_t send_cnt; /** currently sent data */
		uint8_t send_len; /** length of the to-send data */
	
		uint8_t kz_counter; /** shirt circuit counts */
		bool kz_check; /** check short circuit flag */
	
		#ifdef DEBUG_RECEIVER
			uint32_t recv_list[256]; /** populate received array */
			uint8_t r_cnt; /** cyclic counter for recv_list */
		#endif
		bool irq_en; /** Callback enabled/disabled flag */
	
};

#endif
