#ifndef GSM_CONTROLLER_H
#define GSM_CONTROLLER_H

#include "defines.h"
#include "cyclic_buffer.h"
#include "help_functions.h"
#include "usart_controller.h"


#define GSM_BUFFER_SIZE 4096

enum GSM_STATUS
{
	GSM_NO_INFO,
	GSM_NO_SIM,
	GSM_NO_MODEM,
	GSM_NO_NETWORK,
	GSM_NETWORK_OK,
	GSM_CONNECTED
};
//

enum GSM_MODE
{
	GSM_AT_MODE,
	GSM_ONLINE_MODE
};
//

struct SMS
{
	char message[200];
	uint16_t message_length;
	char number[16];
	uint8_t number_length;
	uint8_t DSC;
};
//

enum GSM_MODULE_TYPE
{
	GSM_TYPE_SIMCOM_SIM800,
	GSM_TYPE_SIMCOM_A7670,
	GSM_TYPE_LYNQ_L307E, //Progress PN6280
	GSM_TYPE_NO_TYPE
};
//


class GSM_Controller
{
	public:
		GSM_Controller();
		void Init(UsartController*	usart);
		void Send(const char *send,uint32_t len);
		void SendToServer(uint8_t*, uint32_t len);
		void Send(char ch);
		void PushBuffer();
		void PowerDown();
		void Reset();
		bool CheckModem();
		bool CheckSim();
		bool CheckNetwork();
		void SetupModem();
		bool WaitMessage(bool clear,uint32_t timeout=0);
		void SendSMS(SMS send_sms);
		bool CheckSMS();
		void ConnectToServer(Connection conn);
		void Disconnect();
		void CheckSignal();
		void Ping();
		void SetBaud(uint32_t baud);
		bool CheckModemState();
		TIME_DATE GetSyncTime();
		TIME_DATE GetTime();
		void SaveTime(TIME_DATE time);
		uint32_t GetMessageLength();
	
		SMS sms;
		GSM_STATUS state;
		GSM_MODE mode;
		GSM_MODULE_TYPE		gsm_type;
		//Cyclic_Buffer<uint8_t,GSM_BUFFER_SIZE> gsm_buf;
		uint8_t gsm_buf[GSM_BUFFER_SIZE];
		uint8_t signal_level;
		bool line_clear;
		uint8_t mess_buffer[2048];
		
	private:
		void Send();
		bool SetMode(GSM_MODE);
		bool ReadAnswer(bool no_cr_lf=false);
		uint8_t GetPduSymbol(const char* str);
	
		uint8_t 			to_send[500];
		uint8_t 			send_buf[400];
		char 				answer[512];
		uint32_t 			send_len;
		UsartController*	usart;
};

#endif
