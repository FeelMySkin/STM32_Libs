#include "gsm_controller.h"

GSM_Controller::GSM_Controller()
{
	gsm_type = GSM_TYPE_NO_TYPE;
}
//

void GSM_Controller::Init(UsartController*	usart)
{
	this->usart = usart;
	//gsm_buf.SetOverwrite(false);
}
//

void GSM_Controller::Send()
{
	//LL_DMA_SetDataLength(gsm.tx_dma,gsm.tx_dma_stream,send_len);
	//LL_DMA_EnableStream(gsm.tx_dma,gsm.tx_dma_stream);
	
	uint16_t timeout = 0;
	while(!usart->IsSent() && timeout<=1500)
	{		
		vTaskDelay(1);
		timeout++;
	}
	
	//LL_DMA_DisableStream(gsm.tx_dma,gsm.tx_dma_stream);
	//ClearDmaTCFlag(gsm.tx_dma,gsm.tx_dma_stream);
	
	vTaskDelay(5);
	
}
//

/*Send One char*/
void GSM_Controller::Send(char ch)
{
	usart->Send(ch);
	Send();
}
//

/*Send String*/
void GSM_Controller::Send(const char *send, uint32_t len = 0)
{
	if(len == 0) len = strlen(send);
	usart->Send((uint8_t*)send, len);
	/*for(int i = 0;i<len;++i) to_send[i] = send[i];
	send_len = len;*/
	Send();
}
//

/*Send Data to Server*/
void GSM_Controller::SendToServer(uint8_t* send, uint32_t len)
{
	if(gsm_type == GSM_TYPE_LYNQ_L307E)
	{
		char snd[3];
		sprintf(snd,"%u",len);
		Send("AT+CIPSEND=");
		Send(snd);
		Send("\r");
		vTaskDelay(200);
	}
	
	usart->Send(send, len);
	/*for(int i = 0;i<len;++i) to_send[i] = send[i];
	send_len = len;*/
	Send();
	if(gsm_type == GSM_TYPE_LYNQ_L307E) Send(0x1A);
}

/*All Modules OK.Send SMS message. */
void GSM_Controller::SendSMS(SMS send_sms)
{
	GSM_MODE ret = mode;
	SetMode(GSM_AT_MODE);
	char real_num[20];
	
	uint8_t pdu[256];
	pdu[0] = 0x00; //SCA, 0 - take from SIM
	pdu[1] = 0x01; // PDU Type; VP = 10; MTI = 01
	pdu[2] = 0x00; // Message Reference
	
	/*Message Address (number)*/
	if(send_sms.number[0] == '+') pdu[4] = 0x91;
	else pdu[4] = 0x81;
	
	uint8_t counter = 0;
	for(int i = 0;i<send_sms.number_length;++i)
	{
		if(send_sms.number[i] >= '0' && send_sms.number[i]<='9')
		{		
			real_num[counter] = send_sms.number[i];
			counter++;
		}
	}
	pdu[3] = counter;
	
	for(int i = 0;i<counter;++i)
	{
		if(i%2 == 0) pdu[5+(i>>1)] = (real_num[i] - '0') | 0xF0;
		else pdu[5+(i>>1)] &= ((real_num[i] - '0')<<4) | 0x0F;
	}
	
	pdu[6+(counter>>1)] = 0x00; //PID
	pdu[7+(counter>>1)] = 0x00; //DCS
	
	if(send_sms.message_length>160) send_sms.message_length = 160;
	pdu[8+(counter>>1)] = send_sms.message_length;
	uint16_t curr_position = 9+(counter>>1);
	uint8_t c = 0;
	uint8_t shift_cntr = 0;
	for(int i = 0;i<send_sms.message_length;++i)
	{
		c |= (send_sms.message[i]&0x7F)<<shift_cntr;
		shift_cntr+=7;
		if(shift_cntr>=8)
		{
			pdu[curr_position] = c;
			curr_position++;
			shift_cntr-=8;
			c = (send_sms.message[i]&0x7F)>>(7-shift_cntr);
		}
	}
	if(shift_cntr != 0) pdu[curr_position++] = c;
	//send_sms.message = (char*)head_ptr;
	
	char gsm_to_send[512];
	char *ptr_gsm = gsm_to_send;
	for(int i = 0;i<(curr_position);++i)
	{
	
		sprintf(ptr_gsm,"%02X",pdu[i]);
		ptr_gsm+=2;
	}
	
	Send("AT+CMGS=",8);
	char addr[10];
	sprintf(addr,"%d",(curr_position-1));
	Send(addr,sprintf(addr,"%d",(curr_position-1)));
	Send('\r');
	vTaskDelay(500);
	Send(gsm_to_send,(curr_position*2));
	Send(0x1A);
	vTaskDelay(2000);
	SetMode(ret);
	//free(real_num);
}
//

/*Get PDU Symbol From SMS*/
uint8_t GSM_Controller::GetPduSymbol(const char* str)
{
	uint8_t ret = 0;
	
	if(*str >= '0' && *str<='9') ret = (*str - '0')<<4;
	else ret = ((*str - 'A') + 0x0A)<<4;
	
	str++;
	
	if(*str >= '0' && *str<='9') ret |= (*str - '0');
	else ret |= ((*str - 'A') + 0x0A);
	
	return ret;
}
//

/*Test for A7670. Check Reseived SMS (if any)*/
bool GSM_Controller::CheckSMS()
{
	GSM_MODE ret = mode;
	SetMode(GSM_AT_MODE);
	if(gsm_type != GSM_TYPE_LYNQ_L307E)
	{
		WaitMessage(true,1);
		Send("AT+CMGL=4\r",10);
		vTaskDelay(100);
	}
	else
	{
		WaitMessage(true,1);
		Send("AT+EMGL=4\r");
		vTaskDelay(500);
	}
	
	if(!ReadAnswer()) return false;
	
	char index[3];
	uint8_t index_len;
	
	if(ComparePartStrings(answer,"+EMGL:"))
	{
		index_len = GetLengthToSymbol(answer+7,',');
		for(int i = 0;i<index_len;++i)
		{
			index[i] = answer[7+i];
		}
		WaitMessage(true,1);
		Send("AT+CMGR=");
		Send(answer[7]);
		Send('\r');
		vTaskDelay(200);
		ReadAnswer();
	}
	
	
	uint8_t message_length = 0;
	
	uint16_t p_ptr = 0;
	
	bool sms_status = false;
	//WaitMessage(true,1);
	
	
	if(ComparePartStrings(answer,"+CMGL: ") || ComparePartStrings(answer,"+CMGR:"))
	{
		p_ptr+=7;
		
		/*Get SMS index*/
		if(gsm_type != GSM_TYPE_LYNQ_L307E)
		{
			index_len = GetLengthToSymbol(answer+p_ptr,',');
			for(int i = 0;i<index_len;++i)
			{
				index[i] = answer[p_ptr];
				p_ptr++;
			}
			p_ptr++;
			
			/*skip SMS stat and alpha*/
			for(int i = 0;i<2;++i)
			{
				while(answer[p_ptr] != ',')
				{
					p_ptr++;
				}
				p_ptr++;
			}
		}
		else
		{
			p_ptr+=GetLengthToSymbol(answer+p_ptr,',')+1;
			p_ptr+=GetLengthToSymbol(answer+p_ptr,',')+1;
			p_ptr++;
		}
		
		

		
		/*get Message Length*/
		while(answer[p_ptr] != 0)
		{
			message_length*=10;
			message_length += answer[p_ptr] - '0';
			p_ptr++;
		}
		if(!ReadAnswer(true)) return false;
		p_ptr = 0;
		//p_ptr+=2;
				
		/*Message Centter Address (skipped)*/
		p_ptr+=2+GetPduSymbol(answer+p_ptr)*2;
		
		/*Get PDU type*/
		//uint8_t pdu_type;// = pdu[0];
		//pdu_type = getBufSymb(recv, tail_ptr);
		p_ptr+=2;
		
		/*Get sender number*/
		sms.number_length = GetPduSymbol(answer+p_ptr);
		p_ptr+=2;
		if(GetPduSymbol(answer+p_ptr) == 0x91) sms.number[0] = '+';
		p_ptr+=2;
		for(int i = 0;i<sms.number_length;++i)
		{
			sms.number[i+1] = ((i%2?GetPduSymbol(answer+p_ptr)>>4:GetPduSymbol(answer+p_ptr)) &0x0F) + '0';
			
			if(i%2)
			{
				p_ptr+=2;
			}
		}
		p_ptr+=2;
		
		/*Get PID, DCS and (skipped)time sent*/
		uint8_t PID = GetPduSymbol(answer+p_ptr);
		p_ptr+=2;
		sms.DSC = GetPduSymbol(answer+p_ptr);
		p_ptr+=16;
		
		/*Get message length*/
		sms.message_length = GetPduSymbol(answer+p_ptr); //UDL
		p_ptr+=2;
		
		/*Get Text*/
		//sms_length = UDL;
		uint8_t Cntr = 0;
		uint8_t HiLoHex = 1;
		uint8_t mess_buf[140];
		
		for(int i = 0;;++i)
		{
			if(answer[p_ptr] == 0) break;
			mess_buf[i] = GetPduSymbol(answer+p_ptr);
			p_ptr+=2;
		}
		
		char c;
		uint8_t CRChi= 0xFF, CRClo= 0xFF;
		switch(sms.DSC)
		{
			case 0x00:
				for(int i = 0;i<sms.message_length;++i)
				{
					c= (mess_buf[i-(i>>3)-1]>>(8-(i&0x07))) | (mess_buf[i-(i>>3)]<<(i&0x07));
					c&= 0x7F;
					if (c>='0' && c<='9') c-= '0';
					else if (c>='A' && c<='F') c= c-'A'+0x0A;
					else if (c>='a' && c<='f') c= c-'a'+0x0A;
					else if ((c==' ' || c=='.' || c==',' || c==';' || c==':') && HiLoHex==1)
						continue;
					else
					{		
						//goto _Verbal_7bit;
						for (i=0;i!=sms.message_length;i++) 
						{
							c= (mess_buf[i-(i>>3)-1]>>(8-(i&0x07))) | (mess_buf[i-(i>>3)]<<(i&0x07));
							c&= 0x7F;
							if (c>='a' && c<='z') c&= 0xDF;
							sms.message[i]= c;
						}	
						break;
					}
					if (HiLoHex==1) {
						sms.message[Cntr]= c<<4;
						HiLoHex=0;
					} else {
						sms.message[Cntr]|= c;
						Cntr++;
						HiLoHex=1;
					}		
				}
				
				sms_status = true; //if(ProcessSMS(message,UDL)) SendSMS(number,message,UDL);
			break;
				
			case 0x04:
				// 8-bit data
				for (int i=0;i!=sms.message_length;i++) {
					c= mess_buf[i];
					sms.message[i] = c;
					c^= CRClo;
					CRClo= CRChi^CRC_Table_Hi[c];
					CRChi= CRC_Table_Lo[c];
				}
				if (CRClo!=0 || CRChi!=0) break;
				sms_status = true;//if(ProcessSMS(message,UDL-2)) SendSMS(number,message,UDL);
			break;
					
			case 0x08:
				// 16-bit verbal text
				if (sms.message_length<4) break;
				sms.message_length= sms.message_length>>1;
				for (int i=0;i!=sms.message_length;i++) {
					c= mess_buf[2*i + 1];
					if ((mess_buf[2*i])!=0x00) {
						// russian upcase
						if (c>=0x10 && c<=0x4F) c= (c+0xB0)&0xDF;
					} else {
						// english upcase
						if (c>='a' && c<='z') c&= 0xDF;
					}
					sms.message[i]= c;
				}
				
				sms_status = true;//if(ProcessSMS(message,UDL)) SendSMS(number,message,UDL);
			break;
				
			default:
				break;
			
			
		}
		
		if(sms.number[0] == '+') sms.number_length+=1;
		vTaskDelay(100);
		Send("AT+CMGD=",8);
		Send(index,index_len);
		Send('\r');
		vTaskDelay(100);
	}
	WaitMessage(true,1);
	SetMode(ret);
	return sms_status;
}
//

void GSM_Controller::PushBuffer()
{
	//gsm_buf.push(LL_USART_ReceiveData8(usart));
}
//

bool GSM_Controller::SetMode(GSM_MODE new_mode)
{
	if(mode == new_mode || gsm_type == GSM_TYPE_LYNQ_L307E) return true;
	else if(new_mode == GSM_AT_MODE)
	{
		vTaskDelay(1100);
		WaitMessage(true,1);
		Send("+++",3);
		WaitMessage(true,1);
		vTaskDelay(1100);
		ReadAnswer();
		Send('\r');
		vTaskDelay(100);
	}
	else if(new_mode == GSM_ONLINE_MODE)
	{
		WaitMessage(true,10);
		Send("ATO\r",5);
		vTaskDelay(100);
		ReadAnswer();
		if(ComparePartStrings(answer,"ERROR") || ComparePartStrings(answer,"NO CARRIER"))
		{
			if(state == GSM_CONNECTED) state = GSM_NETWORK_OK;
			return false;
		}
	}
	//
	
	mode = new_mode;
	return true;
}
//

void GSM_Controller::PowerDown()
{
	SetMode(GSM_AT_MODE);
	WaitMessage(true,1);
	Send("AT+CPOWD=1\r",11);
	vTaskDelay(1000);
	vTaskDelay(500);
}
//

void GSM_Controller::Reset()
{
	WaitMessage(true,1);
	usart->Reset();
	state = GSM_NO_INFO;
}
//

/*Just get the answer from AT command (to know module is onLine)*/
bool GSM_Controller::CheckModem()
{
	Send("AT\r\n",4);
	vTaskDelay(100);
	while(ReadAnswer(false))
	{
		if(ComparePartStrings(answer,"OK") || ComparePartStrings(answer,"AT"))
		{	
			
			state = GSM_NO_SIM;
			return  true;
		}
	}
	return false;
}
//

/*Read Answer from USART receive. no_cr_lf - if not need another CR LF before read. Read to last CR LF*/
bool GSM_Controller::ReadAnswer(bool no_cr_lf)
{
	if(!WaitMessage(false,100)) return false;
	if(usart->Length() <= 2) return false;
	
	if(!no_cr_lf)
	{
		while(usart->GetChar() !='\r' && usart->GetChar(1) != '\n')
		{	
			usart->ShiftTail(1);
			if(usart->Length()<=2) return false;
		}
		while(usart->GetChar() == '\r' || usart->GetChar() == '\n') usart->ShiftTail(1);
	}
	else if(usart->GetChar() =='\r' && usart->GetChar(1) == '\n') usart->ShiftTail(2);
	
	for(int i = 0;usart->GetChar() != '\r';++i)
	{	
		if(usart->Length() == 0) return false;
		answer[i] = usart->Pull();
		answer[i+1] = 0;
	}
	usart->ShiftTail(2);
	return true;
	
}
//

/*All Modules OK. Get Network Registration*/
bool GSM_Controller::CheckNetwork()
{
	WaitMessage(true,1);
	Send("AT+CREG?\r",9); 
	vTaskDelay(100);
	if(!ReadAnswer())
	{	
		WaitMessage(true,1);
		return false;
	}
	
	if(ComparePartStrings(answer,"+CREG: "))
	{
		if((answer[9] == '1' || answer[9] == '5') && answer[10] != '0')
		{	
			if(state == GSM_NO_NETWORK) state = GSM_NETWORK_OK;
			return true;
		}
	}

	/*if(stringcomp((char*)recv,"\r\n+CREG: ",tail_ptr))
	{
		IncrementTail(11);
		if(recv[tail_ptr] == '1' || recv[tail_ptr] == '5') return true;
	}*/
	
	return false;
}
//

/*All Modules OK. Configure GSM Module*/
void GSM_Controller::SetupModem()
{	
	
	//vTaskDelay(1200);
	//Config GSM Module
	Send("ATE0\r"); //Disable ECHO, all modules understand
	vTaskDelay(800);
	uint8_t ptr = 0;
	
	//Understand GSM module type
	WaitMessage(true,1);
	while(gsm_type == GSM_TYPE_NO_TYPE)
	{
		vTaskDelay(1000);
		Send("ATI\r");
		while(ReadAnswer())
		{	
			if(ComparePartStrings(answer,"SIM800"))
			{
				gsm_type = GSM_TYPE_SIMCOM_SIM800;
			}
			else
			{
				while(!ComparePartStrings(answer,"Model:"))
				{
					if(!ReadAnswer(true)) break;
				}
				if(ComparePartStrings(answer,"Model: A76")) gsm_type = GSM_TYPE_SIMCOM_A7670;
				if(ComparePartStrings(answer,"Model:LYNQ_L30")) gsm_type = GSM_TYPE_LYNQ_L307E;
			}
		}
		ptr++;
		if(ptr>=50) NVIC_SystemReset();
	}
	
	WaitMessage(true,1);
	vTaskDelay(200);
	Send("AT+CNMI=2,0,0,0,0\r"); //New SMS Indication, disable event. All Modules
	vTaskDelay(200);
	Send("AT+CSCLK=0\r",11); //Disable Slow Clock (No Sleep Mode). All Modules
	vTaskDelay(200);
	
	if(gsm_type == GSM_TYPE_LYNQ_L307E)
	{
		
		WaitMessage(true,1);
		for(int i = 0;i<50;++i)
		{
			
			vTaskDelay(1000);
			ReadAnswer();
			if(ComparePartStrings(answer,"TCPIP") && strlen(answer) != 0)
			{
				break;
			}
			if(i >=49) NVIC_SystemReset();
		}
		vTaskDelay(2000);
		Send("AT+ESIMS=0\r");
		vTaskDelay(800);
		Send("AT+EIND=0\r");
		vTaskDelay(800);
		Send("AT+CIPRXGET?\r");
		vTaskDelay(800);
	}
	
	if(gsm_type == GSM_TYPE_SIMCOM_A7670 || gsm_type == GSM_TYPE_SIMCOM_SIM800) Send("AT+IPR=0\r",9); //Set baud rate, SIMCom only, 0 - autobaud
	vTaskDelay(200);
	if(gsm_type == GSM_TYPE_SIMCOM_SIM800) Send("AT+GSMBUSY=1\r",13); // Disable all in-calls. ONLY Sim800
	vTaskDelay(200);
	if(gsm_type == GSM_TYPE_SIMCOM_SIM800) Send("AT+CBAND=ALL_BAND\r",18); //Set Operator Band. ONLY Sim800
	vTaskDelay(200);
	Send("AT&W\r",5); //Save
	vTaskDelay(200);
}
//

/*Check SimCard Inserted. All modules OK*/
bool GSM_Controller::CheckSim()
{
	WaitMessage(true,1);
	if(gsm_type == GSM_TYPE_SIMCOM_SIM800) Send("AT+CSMINS?\r",11);
	else if(gsm_type == GSM_TYPE_SIMCOM_A7670) Send("AT+CICCID\r",11);
	else if(gsm_type == GSM_TYPE_LYNQ_L307E) Send("AT+ESIMS?\r",11);
	vTaskDelay(200);
	if(!ReadAnswer())
	{	
		WaitMessage(true,1);
		return false;
	}
	
	if(gsm_type == GSM_TYPE_SIMCOM_SIM800 && ComparePartStrings(answer,"+CSMINS:"))
	{
		if(answer[11] == '1' || answer[11] == '5')
		{	
			if(state == GSM_NO_SIM) state = GSM_NO_NETWORK;
			return true;
		}
	}
	else if(gsm_type == GSM_TYPE_LYNQ_L307E && ComparePartStrings(answer,"+ESIMS:"))
	{
		if(answer[8] == '1')
		{	
			if(state == GSM_NO_SIM) state = GSM_NO_NETWORK;
			return true;
		}
	}
	else if (gsm_type == GSM_TYPE_SIMCOM_A7670 && ComparePartStrings(answer,"+ICCID:"))
	{
		if(state == GSM_NO_SIM) state = GSM_NO_NETWORK;
		return true;
		
	}
	
	return false;
	
	/*if(stringcomp((char*)recv,"\r\n+CSMINS:",tail_ptr))
	{
		IncrementTail(13);
		if(recv[tail_ptr] == '1' || recv[tail_ptr] == '5') return true;
	}*/
	//return true;
}
//

/*Just wait to return message. clear - reset tail. timeout - wait time in 100ms*/
bool GSM_Controller::WaitMessage(bool clear,uint32_t timeout)
{
	uint16_t buf_len = 0;
	if(timeout == 0) timeout = 0xFFFFFF;
	else timeout+=1;
	while(buf_len != usart->Length() && usart->Length() != 0 && timeout--)
	{
		buf_len = usart->Length();
		vTaskDelay(100);
	}
	if(clear)usart->ClearBuffer();
	if(timeout == 0) return false;
	else return true;
}
//

/*Only foe LYNQ (Progress) Module. Get message length to parse from network.*/
uint32_t GSM_Controller::GetMessageLength()
{
	uint32_t r_len = 0;
	if(gsm_type == GSM_TYPE_LYNQ_L307E)
	{
		WaitMessage(true,1);
		Send("AT+CIPRXGET=2,512\r");
		vTaskDelay(200);
		if(ReadAnswer(true))
		{
			if(!ComparePartStrings(answer,"+CIPRXGET")) r_len = 0;
			else
			{
				r_len = atoi(answer+13);
			}
		}
		
	}
	else r_len = usart->Length();
	
	return r_len;
	
	/*for(int i = r_len-1;i>=0;--i)
	{
		mess_buffer[i] = usart->Pull();
	}
	
	if(ReadAnswer())
	{
		if(ComparePartStrings(answer,"+RECEIVE,"))
		{
			r_len = atoi(answer+9);
			for(int i = r_len-1;i>=0;--i)
			{
				mess_buffer[i] = usart->Pull();
			}
			usart->ShiftTail(2);
			return r_len;
		}
	}
	return 0;*/
}
//

//Connect to Server. All OK
void GSM_Controller::ConnectToServer(Connection conn_setup)
{	
	WaitMessage(true,1);
	/*Configure Access point*/
	if(gsm_type == GSM_TYPE_SIMCOM_SIM800)
	{
		Send("AT+CIPCSGP=1,\"",14);
		if(GetLengthToSymbol(conn_setup.access_point,0) > 0) Send(conn_setup.access_point,GetLengthToSymbol(conn_setup.access_point,0));
		Send("\",\"",3);
		if(GetLengthToSymbol(conn_setup.login,0) > 0) Send(conn_setup.login,GetLengthToSymbol(conn_setup.login,0));
		Send("\",\"",3);
		if(GetLengthToSymbol(conn_setup.pass,0) > 0) Send(conn_setup.pass,GetLengthToSymbol(conn_setup.pass,0));
		Send("\"\r",2);
	}
	else if(gsm_type == GSM_TYPE_LYNQ_L307E)
	{
		Send("AT+CSTT=\"");
		if(GetLengthToSymbol(conn_setup.access_point,0) > 0) Send(conn_setup.access_point,GetLengthToSymbol(conn_setup.access_point,0));
		Send("\",\"IP\",\"");
		if(GetLengthToSymbol(conn_setup.login,0) > 0) Send(conn_setup.login,GetLengthToSymbol(conn_setup.login,0));
		Send("\",\"",3);
		if(GetLengthToSymbol(conn_setup.pass,0) > 0) Send(conn_setup.pass,GetLengthToSymbol(conn_setup.pass,0));
		Send("\"\r",2);
		
		Send("AT+CGDCONT=1,\"IP\",\"");
		if(GetLengthToSymbol(conn_setup.access_point,0) > 0) Send(conn_setup.access_point,GetLengthToSymbol(conn_setup.access_point,0));
		Send("\"\r",2);
		
		vTaskDelay(200);
		Send("AT+CIPMUX=0\r");
		vTaskDelay(200);
		Send("AT+CIICR\r");
		vTaskDelay(200);
		Send("AT+CGDATA=\"M-MBIM\",1,1\r");
		
	}
	else if(gsm_type == GSM_TYPE_SIMCOM_A7670)
	{
		Send("AT+CGDCONT=1,\"IP\",\"");
		if(GetLengthToSymbol(conn_setup.access_point,0) > 0) Send(conn_setup.access_point,GetLengthToSymbol(conn_setup.access_point,0));
		Send("\"\r",2);
		Send("AT+CGAUTH=1,");
		if(strlen(conn_setup.login) != 0 && strlen(conn_setup.pass) != 0)
		{
			Send("1,\"");
			Send(conn_setup.pass,GetLengthToSymbol(conn_setup.pass,0));
			Send("\",\"",3);
			Send(conn_setup.login,GetLengthToSymbol(conn_setup.login,0));
			Send("\"\r",2);
		}
		else if( strlen(conn_setup.pass) != 0)
		{
			Send("2,");Send(conn_setup.pass,GetLengthToSymbol(conn_setup.pass,0));
			Send("\",\r");
		}
		else Send("0\r");
	}
	vTaskDelay(200);
	if(!ReadAnswer()) return;
	
	vTaskDelay(100);
	/*Enable transparent mode.*/
	if(gsm_type == GSM_TYPE_SIMCOM_A7670 || gsm_type == GSM_TYPE_SIMCOM_SIM800) Send("AT+CIPMODE=1\r",13);
	
	vTaskDelay(100);
	/*Configure transparent mode.*/
	if(/*gsm_type == GSM_TYPE_SIMCOM_A7670 || */gsm_type == GSM_TYPE_SIMCOM_SIM800) Send("AT+CIPCCFG=3,2,100,1,0,50,20\r",29);
	vTaskDelay(100);
	Send("AT+CGATT=1\r",11);
	vTaskDelay(100);
	while(ReadAnswer())
	{
		vTaskDelay(100);
	}
	
	for(int i = 0;i<30;++i)
	{
		Send("AT+CGREG=1\r");
		vTaskDelay(500);
		Send("AT+CEREG=1\r");
		vTaskDelay(500);
		Send("AT+CGREG?\r",10);
		vTaskDelay(500);
		Send("AT+CEREG?\r",10);
		vTaskDelay(500);
		WaitMessage(false,10);
		while(ReadAnswer())
		{
			if(ComparePartStrings(answer,"+CGREG: "))
			{
				if(answer[10] == '1' || answer[11] == '1')
				{	
					i = 50;
					break;
				}
			}
		}			
		if(i == 29)
		{
			Disconnect();
			return;
		}
	}
	//uint16_t curr_head = head_ptr;
	
	/*Start session*/
	WaitMessage(true,1);
	if(gsm_type == GSM_TYPE_LYNQ_L307E || gsm_type == GSM_TYPE_SIMCOM_SIM800)
	{
		Send("AT+CIPSTART=\"TCP\",\"",19);
		if(GetLengthToSymbol(conn_setup.server1,0) > 0) Send(conn_setup.server1,GetLengthToSymbol(conn_setup.server1,0));
		Send("\",\"",3);
		if(GetLengthToSymbol(conn_setup.port1,0) > 0) Send(conn_setup.port1,GetLengthToSymbol(conn_setup.port1,0));
		Send("\"\r",2);
		vTaskDelay(200);
	}
	else if (gsm_type == GSM_TYPE_SIMCOM_A7670)
	{
		Send("AT+NETOPEN\r");
		Send("AT+CIPOPEN=0,\"TCP\",\"");
		if(GetLengthToSymbol(conn_setup.server1,0) > 0) Send(conn_setup.server1,GetLengthToSymbol(conn_setup.server1,0));
		Send("\",",2);
		if(GetLengthToSymbol(conn_setup.port1,0) > 0) Send(conn_setup.port1,GetLengthToSymbol(conn_setup.port1,0));
		Send("\r",1);
		vTaskDelay(200);
	}
	ReadAnswer();
	
	if(ComparePartStrings(answer,"OK"))
	{
		
		for(uint32_t i = 0;i<100*conn_setup.timeout;++i)
		{
			vTaskDelay(500);
			if(ReadAnswer())
			{
				if(ComparePartStrings(answer,"CONNECT") || ComparePartStrings(answer,"+RECEIVE"))
				{
					state = GSM_CONNECTED;
					mode = GSM_ONLINE_MODE;
					
					if(gsm_type == GSM_TYPE_LYNQ_L307E)
					{
						Send("AT+CIPRXGET=1\r");
						vTaskDelay(100);
						WaitMessage(true,1);
					}
					
					return;
				}
				else if(ComparePartStrings(answer,"ERROR") || ComparePartStrings(answer,"PDP")) break;
			}
		}	
	}
	
	#ifdef TEST
	if(conn_setup.server2[0] != 0)
	{
		Send("AT+CIPSTART=\"TCP\",\"",19);
		if(strlen(conn_setup.server2) > 0) Send(conn_setup.server1,strlen(conn_setup.server1));
		Send("\",\"",3);
		if(strlen(conn_setup.port2) > 0) Send(conn_setup.port1,strlen(conn_setup.port1));
		Send("\"\r",2);
		
		//Send("AT+CIPSTART=\"TCP\",\"92.53.108.35\",\"20405\"\r",41);
		
		for(int i = 0;i<10000*conn_setup.timeout;++i)
		{
			if(stringcomp((char*)recv,"\r\nOK\r\n\r\nCONNECT\r\n", curr_head))
			{		
				tail_ptr = head_ptr;
				stream_mode = true;
				state = GSM_CONNECTED;
				return true;
			}
			/*if(i == 1000) last_ptr = head_ptr;
			else if (i>1000 && last_ptr != head_ptr)
			{
				tail_ptr = head_ptr;
				stream_mode = true;
				return true;
			}*/
			vTaskDelay(tick_1ms);
		}
	}
	
	#endif
	Disconnect();
}
//

void GSM_Controller::Disconnect()
{
	SetMode(GSM_AT_MODE);
	Send('\r');
	vTaskDelay(500);
	WaitMessage(true,1);
	
	Send("AT+CIPSHUT\r",11);
	vTaskDelay(100);
	Send("AT+CGATT=0\r",11);
	vTaskDelay(100);
	WaitMessage(false,10);
	WaitMessage(true,1);
	
	state = GSM_NETWORK_OK;
}
//

void GSM_Controller::CheckSignal()
{
	GSM_MODE ret = mode;
	SetMode(GSM_AT_MODE);
	WaitMessage(true,1);
	Send("AT+CSQ\r",7);
	vTaskDelay(500);
	ReadAnswer();
	
	uint8_t ptr = 0;
	if(ComparePartStrings(answer,"+CSQ: "))
	{
		ptr+=6;
		char sig[3] = {0};
		CopyString(sig,answer+ptr,GetLengthToSymbol(answer+ptr,','));
		signal_level = atoi(sig);
	}
	SetMode(ret);
}
//

void GSM_Controller::Ping()
{
	SetMode(GSM_AT_MODE);
	CheckSignal();
	SetMode(GSM_ONLINE_MODE);
}
//

bool GSM_Controller::CheckModemState()
{
	bool state;
	GSM_MODE ret = mode;
	SetMode(GSM_AT_MODE);
	state = CheckSim() || CheckNetwork();
	if(state) CheckSignal();
	SetMode(ret);
	return state;
	
}
//

TIME_DATE GSM_Controller::GetTime()
{
	GSM_MODE ret = mode;
	SetMode(GSM_AT_MODE);
	WaitMessage(true,1);
	for(int i = 0;i<4;++i)
	{
		Send("AT+CCLK?\r",9);
		vTaskDelay(200);
		if(ReadAnswer() && ComparePartStrings(answer,"+CCLK:")) break;
		WaitMessage(true,1);
	}
	//ReadAnswer();
	TIME_DATE tm = {0};
	uint8_t date[6];
	
	if(ComparePartStrings(answer,"+CCLK:"))
	{
		uint8_t ptr = 8;
		char num[2];
		for(int i = 0;i<6;++i)
		{
			num[0] = answer[ptr];
			num[1] = answer[ptr+1];
			date[i] = atoi(num);
			ptr+=3;
		}
		tm.year = date[0];
		tm.month = date[1];
		tm.day = date[2];
		tm.hours = date[3];
		tm.mins = date[4];
		tm.secs = date[5];
	}
	SetMode(ret);
	return tm;
}
//

void GSM_Controller::SaveTime(TIME_DATE time)
{
	GSM_MODE ret = mode;
	SetMode(GSM_AT_MODE);
	WaitMessage(true,1);
	
	
	char send_string[31];
	sprintf(send_string,"AT+CCLK=\"%02i/%02i/%02i,%02i:%02i:%02i+00\"\r",	time.year,
																																				time.month,
																																				time.day,
																																				time.hours,
																																				time.mins,
																																				time.secs);
	Send(send_string,31);
	vTaskDelay(200);
	SetMode(ret);
}
//

void GSM_Controller::SetBaud(uint32_t baud)
{
	usart->SetBaud(baud);
	//gsm.baud_rate = baud;
	//InitUSART();
}
//

void GSM_Controller::Call(const char *num)
{
	Send(num);
}
