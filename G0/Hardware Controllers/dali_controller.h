#ifndef DALI_CONTROLLER_H
#define DALI_CONTROLLER_H

#include "defines.h"

#define DALI_HYST 	180
#define DEBUG_RECEIVER
#define DALI_BAUDS_COUNT	4
//#define DALI_HALFBIT		417 //in us

const uint16_t DALI_BAUDS[DALI_BAUDS_COUNT] = {1200,2400,4800,9600};

enum DALI_LOGIC
{
	DALI_LOGIC_POSITIVE,
	DALI_LOGIC_NEGATIVE
};

enum DALI_TYPE
{
	DALI_IC,
	DALI_EXTI
};

struct DALI_InitTypeDef
{
	GPIO_TypeDef* 		dali_tx_gpio;
	GPIO_TypeDef* 		dali_rx_gpio;
	uint16_t 			dali_tx_pin;
	uint16_t 			dali_rx_pin;
	TIM_TypeDef*		dali_tim;
	uint32_t			dali_rx_ch;
	TIM_TypeDef*		kz_tim;
	uint32_t 			callback_line;
	DALI_LOGIC			logic:2;
	DALI_TYPE			type:2;
};



class DaliController
{
	public:
		DaliController();
		~DaliController();
		void Init(DALI_InitTypeDef);
		void Send(uint32_t,uint8_t, uint16_t baud = DALI_BAUDS[0]);
		void SendDelayed(uint32_t, uint8_t, uint32_t delay = 5000, uint16_t baud = DALI_BAUDS[0]);
		void Process(bool); //Use it in DALI_TIM Update IRQHandler with "true" and in EXTI(if type DALI_EXTI) with "false"
		void EnableRecvInterrupt(bool state);
		void EnableKZCheck(bool state);
		void StartReceiving();
		void StopReceiving();
		void CheckKZ();
		void ProcessIC(); //Use it in DALI_TIM IC IRQHandler (if type DALI_IC)
	
		bool sending,receiving,delayed;
		bool send_completed,receive_completed;
		bool kz_state;
		uint32_t received;
		uint8_t recv_bytes;
	
	private:
		void InitGPIO();
		void InitTIM();
		void StartSending();
		void ReadData();
		void SetLow();
		void SetHigh();
	
		DALI_InitTypeDef dali;
	
		uint8_t recv_cnt;
		uint8_t delay_cnt;
		uint16_t recv_buf[80];
	
		uint16_t send_buf[80];
		uint8_t send_cnt;
		uint8_t send_len;
		uint16_t active_pin;
	
		uint8_t kz_counter;
		bool kz_check;
	
		#ifdef DEBUG_RECEIVER
			uint32_t recv_list[256];
			uint8_t r_cnt;
		#endif
		bool irq_en,led_en;
	
};

#endif
