#ifndef DALI_HW_H
#define DALI_HW_H

#include "defines.h"

#define DALI_HYST 			3 /*Hysteresis in samples*/
#define DALI_SAMPLINGS		8 /*Number of samples*/
#define DEBUG_RECEIVER		/*Enables DEBUG Buffer (what received)*/
#define DALI_BAUD			1200 /*DALI baudrate*/


enum DALI_LOGIC
{
	DALI_LOGIC_POSITIVE,
	DALI_LOGIC_NEGATIVE
};

/**
 * @brief DALI State machine
 * 
 */
enum DALI_STATE_MACHINE
{
	DALI_STATE_IDLE,	/*Idle state. Listening...*/
	DALI_STATE_SENDING, /*Data Sending state.*/
	DALI_STATE_WAIT_TO_SEND, /*Delayed Sending*/
	DALI_STATE_RECEIVING,	/*Receiving state.*/
};

/**
 * @brief Type of received pulses from RX
 * 
 */
enum DALI_RECEIVED_PULSE
{
	DALI_PULSE_HALFBIT, /*Manchester half-bit*/
	DALI_PULSE_FULLBIT, /*Manchester full-length bit*/
	DALI_PULSE_STOP, /*STOP bit (>baud)*/
	DALI_PULSE_ERROR, /*Unknown BIT length*/
};

/**
 * @brief DALI Initializer
 * 
 */
struct DALI_InitTypeDef
{
	GPIO_TypeDef* 		dali_tx_gpio; /* Set GPIO* for TX pin*/
	GPIO_TypeDef* 		dali_rx_gpio; /*Set GPIO* for RX pin*/
	uint16_t 			dali_tx_pin; /*Set LL_GPIO_PIN_* for TX pin*/
	uint16_t 			dali_rx_pin; /*Set LL_GPIO_PIN_* for RX pin*/
	TIM_TypeDef*		dali_tim; /*Set TIM* timer*/
	TIM_TypeDef*		kz_tim; /*Set another TIM* timer (if needed) for long zero (DALI line not powered error)*/
	uint32_t 			callback_line; /*SET LL_EXTI_LINE_* for callback (if needed)*/
	DALI_LOGIC			rx_logic:2; /*Set DALI_LOGIC for TX pin*/
	DALI_LOGIC			tx_logic:2; /*Set DALI_LOGIC for RX pin*/
};

/**
 * @brief DALI Hardware Controller Class.
 * Only Transport level (receive and transmit)
 * 
 */
class DaliController
{
	public:
		DaliController();
		~DaliController();

		/**
		 * @brief DALI Class Initializer
		 * @param str Initializer struct
		 * 
		 */
		void Init(DALI_InitTypeDef str);

		/**
		 * @brief DALI data sender method.
		 * 
		 * @param mess message to send.
		 * @param n_bits Number of bits (8, 16, 24)
		 */
		bool Send(uint32_t mess, uint8_t n_bits);

		/**
		 * @brief DALI Data sender method woth delay.
		 * 
		 * @param mess message to send
		 * @param n_bits Number of bits to send (8, 16, 24)
		 * @param delay Send delay in us (default 5000)
		 */
		bool SendDelayed(uint32_t mess, uint8_t n_bits, uint32_t delay = 5000);

		/**
		 * @brief Process Timer. Call it from dali_tim.
		 * 
		 */
		void Process();

		/**
		 * @brief Enable/Disable Received data callback to EXTI.
		 * 
		 * @param state (TRUE/FALSE)
		 */
		void EnableRecvInterrupt(bool state);

		/**
		 * @brief Enables/Disables long-zero check (NOD DALI Line Voltage).
		 * 
		 * @param state (TRUE/FALSE)
		 */
		void EnableKZCheck(bool state);

		/**
		 * @brief Start Receiving.
		 * 
		 */
		void StartReceiving();

		/**
		 * @brief Check if no DALI Line Voltage. Call it from KZ_TIM
		 * 
		 */
		void CheckKZ();

		DALI_STATE_MACHINE state; /*Current DALI Line state*/
		bool send_completed,receive_completed; /*send/receive flags*/
		bool kz_state; /* Is no DALI Voltage flag*/
		uint32_t received; /*received message*/
		uint8_t recv_bytes; /*received message length*/
	
	private:
		/**
		 * @brief Initialize DALI GPIO
		 * 
		 */
		void InitGPIO();

		/**
		 * @brief Initialize all TIMs.
		 * 
		 */
		void InitTIM();

		/**
		 * @brief Start sending message on TX pin
		 * 
		 */
		void StartSending();

		/**
		 * @brief Decode received data.
		 * 
		 */
		void ReadData();

		/**
		 * @brief Set logic LOW on TX pin.
		 * 
		 */
		void SetLow();

		/**
		 * @brief Set logic HIGH on TX pin.
		 * 
		 */
		void SetHigh();

		
		/**
		 * @brief Processes Samplings of TIM URQ if current state is IDLE or RECEIVING.
		 * 
		 */
		void ProcessCounter();
	
		DALI_InitTypeDef dali; /* DALI initializer object*/
	
		uint8_t recv_cnt; /* number of received samples on RX pin*/
		uint8_t curr_bit; /*Current sampling bit*/
		uint8_t delay_cnt; /* Delay on transmit*/
		uint16_t recv_buf[80]; /*Received samples on RX pin*/
	
		uint16_t send_buf[80]; /*Timings for to-send data*/
		uint8_t send_cnt; /*Currently sent data*/
		uint8_t send_len; /*Length of the current to-send data.*/
		uint16_t sampling_arr; /*Autoreload Register for dali_tim by sampling rate*/
		//uint16_t active_pin;
	
		uint8_t kz_counter; /**Low Voltage counter*/
		bool kz_check; /*Flag for Low Voltage TIM*/
	
		#ifdef DEBUG_RECEIVER
			uint32_t recv_list[256]; /*Populates this array of received data*/
			uint8_t r_cnt; /*Number of received data*/
		#endif
		bool irq_en; /*Flag for callback*/
	
};

#endif
