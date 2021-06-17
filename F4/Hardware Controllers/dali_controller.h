#ifndef DALI_CONTROLLER_H
#define DALI_CONTROLLER_H

#include "defines.h"

#define DALI_HYST 	180
#define DEBUG_RECEIVER
#define DALI_HALFBIT		417 //in us

enum DALI_LOGIC
{
	DALI_LOGIC_POSITIVE,
	DALI_LOGIC_NEGATIVE
};

struct DALI_InitTypeDef
{
	GPIO_TypeDef* 		dali_tx_gpio;
	GPIO_TypeDef* 		dali_rx_gpio;
	uint16_t 			dali_tx_pin;
	uint16_t 			dali_rx_pin;
	TIM_TypeDef*		dali_tim;
	TIM_TypeDef*		kz_tim;
	uint32_t 			callback_line;
	DALI_LOGIC			logic;
};

class DaliController
{
	public:
		DaliController();
		~DaliController();
		void Init(DALI_InitTypeDef);
		void Send(uint32_t,uint8_t);
		void SendDelayed(uint32_t, uint8_t, uint32_t delay = 5000);
		void Process(bool);
		void EnableRecvInterrupt(bool state);
		void EnableKZCheck(bool state);
		void StartReceiving();
		void StopReceiving();
		void CheckKZ();
	
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
