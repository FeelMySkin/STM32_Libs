#include "dali_controller.h"

DaliController::DaliController()
{
	/** default initializers */
	irq_en = false;
	delayed = false;
	kz_check = false;
	kz_state = false;
	kz_counter = 0;
	
	#ifdef DEBUG_RECEIVER
		r_cnt = 0;
	#endif
}
//

DaliController::~DaliController()
{
	
}
//

void DaliController::Init(DALI_InitTypeDef get)
{
	/** Initialize Peripherals */
	dali = get;
	InitGPIO();
	InitTIM();
	receiving = false;
	sending = false;

	/**Start Listen */
	StartReceiving();
}
//

void DaliController::InitGPIO()
{
	/** Initializer GPIO peripherals */
	/** TX as OUTPUT PUSHPULL */
	LL_GPIO_InitTypeDef gpio;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.Pin = dali.dali_tx_pin;
	LL_GPIO_Init(dali.dali_tx_gpio,&gpio);
	
	/** RX as ALTERNATE (DALI_IC) or INPUT PULL_UP (DALI_EXTI) */
	if(dali.type == DALI_IC)
	{
		gpio.Alternate = dali.dali_af;
		gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	}
	else gpio.Mode = LL_GPIO_MODE_INPUT;
	gpio.Pin = dali.dali_rx_pin;
	LL_GPIO_Init(dali.dali_rx_gpio,&gpio);
	SetHigh();
	
	/**If DALI_EXTI Enable EXTI IRQHandler on RX pins */
	if(dali.type == DALI_EXTI)
	{
		LL_EXTI_InitTypeDef exti;
		exti.LineCommand = ENABLE;
		exti.Line_0_31 = GetExtiLine(dali.dali_rx_pin);
		exti.Mode = LL_EXTI_MODE_IT;
		exti.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
		SetExtiSource(dali.dali_tx_gpio,GetExtiLine(dali.dali_rx_pin));
		LL_EXTI_Init(&exti);
		
		EnableExtiIRQn(GetExtiLine(dali.dali_rx_pin),0);
	}
}
//


void DaliController::InitTIM()
{
	/**Initialize main timer with 1MHz (1us) frequency */ 
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = 3*(1000000/DALI_BAUDS[0])/2;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/1000000-1;
	tim.RepetitionCounter = 0;
	LL_TIM_Init(dali.dali_tim,&tim);
	
	LL_TIM_ClearFlag_UPDATE(dali.dali_tim);
	 if(dali.type == DALI_EXTI) LL_TIM_EnableIT_UPDATE(dali.dali_tim); /**If DALI_EXTI Type enable UPDATE IRQ on TIM */
	
	/**Enable IRQHAndler on TIM* */
	EnableTimIRQn(dali.dali_tim,0);
	
	/**If DALI_IC enable InputCapture on both Edges and Enable IC */
	if(dali.type == DALI_IC)
	{
		LL_TIM_IC_InitTypeDef ic;
		ic.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
		ic.ICFilter = LL_TIM_IC_FILTER_FDIV1;
		ic.ICPolarity = LL_TIM_IC_POLARITY_BOTHEDGE;
		ic.ICPrescaler = LL_TIM_ICPSC_DIV1;
		LL_TIM_IC_Init(dali.dali_tim,dali.dali_rx_ch,&ic);
		LL_TIM_EnableCounter(dali.dali_tim);
	}
	
	

}
//

void DaliController::StartReceiving()
{	
	/**If DALI_IC Disables Update IRQ, restarts counter, sets maximum autoreload and enables CC  IRQ */
	if(dali.type == DALI_IC)
	{
		ClearIcFlag(dali.dali_tim,dali.dali_rx_ch);
		LL_TIM_DisableIT_UPDATE(dali.dali_tim);
		LL_TIM_SetCounter(dali.dali_tim,0);
		LL_TIM_SetAutoReload(dali.dali_tim,3*417);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH1) LL_TIM_EnableIT_CC1(dali.dali_tim);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH2) LL_TIM_EnableIT_CC2(dali.dali_tim);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH3) LL_TIM_EnableIT_CC3(dali.dali_tim);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH4) LL_TIM_EnableIT_CC4(dali.dali_tim);
	}
	/** Else Enables RX EXTI IRQ */
	else
	{
		LL_EXTI_ClearFlag_0_31(GetExtiLine(dali.dali_rx_pin));
		LL_EXTI_EnableIT_0_31(GetExtiLine(dali.dali_rx_pin));
	}
}
//

void DaliController::StopReceiving()
{
	/**if DALI_IC Disables CC IRQ */
	if(dali.type == DALI_IC)
	{
		LL_TIM_SetCounter(dali.dali_tim,0);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH1) LL_TIM_DisableIT_CC1(dali.dali_tim);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH2) LL_TIM_DisableIT_CC2(dali.dali_tim);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH3) LL_TIM_DisableIT_CC3(dali.dali_tim);
		if(dali.dali_rx_ch == LL_TIM_CHANNEL_CH4) LL_TIM_DisableIT_CC4(dali.dali_tim);
	}
	/** else Disables RX EXTI IRQ */
	else LL_EXTI_DisableIT_0_31(GetExtiLine(dali.dali_rx_pin));
	
}
//

void DaliController::SendDelayed(uint32_t mess,uint8_t n_bits, uint32_t delay, uint16_t baud)
{
	/** See Send() method */
	send_completed = false;
	uint32_t to_send =(1<<(n_bits)) | mess;
	uint8_t bits[32];
	for(int i = 0;i<n_bits+1;++i) bits[i] = (to_send>>(n_bits-i))&1;
	send_len = 0;
	
	uint8_t halfbit = (1000000/baud)/2;
	
	for(int i = 0;i<n_bits+1;++i)
	{
		if(i == 0 || bits[i] == bits[i-1]) 
		{
			send_buf[send_len] = halfbit;
			send_len++;
		}
		
		if(i == n_bits || bits[i] == bits[i+1])
		{
			send_buf[send_len] = halfbit;
			send_len++;
		}
		else
		{
			send_buf[send_len] = 2*halfbit;
			send_len++;
		}
	}
	
	/** set delayed flag */
	delayed = true;
	
	StopReceiving();
	if(receiving)
	{
		StartReceiving();
		return;
	}
	
	/** Set delay to TIM */
	LL_TIM_DisableCounter(dali.dali_tim);
	LL_TIM_SetCounter(dali.dali_tim,0);
	LL_TIM_SetAutoReload(dali.dali_tim,delay);
	LL_TIM_EnableCounter(dali.dali_tim);
}
//

void DaliController::Send(uint32_t mess,uint8_t n_bits, uint16_t baud)
{
	/**Adds start bit and increase n_bits by 1 */
	send_completed = false;
	uint32_t to_send =(1<<(n_bits)) | mess;
	++n_bits;

	/** get bits from message */
	uint8_t bits[32];
	for(int i = 0;i<n_bits;++i) bits[i] = (to_send>>((n_bits-1)-i))&1;
	send_len = 0;
	
	/** Get halfbit timing (Manchester encoding) */
	uint16_t halfbit = (1000000/baud)/2;
	
	/** For every bit */
	for(int i = 0;i<n_bits;++i)
	{
		 /**If first bit or current bit equal to previous bit - send halfbit time Pulse */
		if(i == 0 || bits[i] == bits[i-1]) 
		{
			send_buf[send_len] = halfbit;
			send_len++;
		}
		
		/** if last halfbit or current bit equal to next bit - send halfbit time Pulse */
		if(i == n_bits-1 || bits[i] == bits[i+1])
		{
			send_buf[send_len] = halfbit;
			send_len++;
		}
		/** else if current bit bot equal to next bit - send full bit time Pulse */
		else
		{
			send_buf[send_len] = 2*halfbit;
			send_len++;
		}
	}
	
	/**Send */
	StartSending();
}
//

///TODO: recheck logic 
void DaliController::ReadData()
{
	
	/** Decode received data */
	/*uint16_t baud = 1000000/(recv_buf[0]*2);
	for(uint8_t i = 0;i<DALI_BAUDS_COUNT;++i)
	{
		if(baud>=DALI_BAUDS[i]-150 && baud<=DALI_BAUDS[i]+150)
		{			
			baud = (1000000/DALI_BAUDS[i])/2;
			last_baud = DALI_BAUDS[i];
		}
	}*/
	
	/**Set halfbit time */
	uint16_t baud = 417;
	
	/**For every received pulse */
	for(int i = 0;i<recv_cnt;++i)
	{
		/**if time of pulse around halfbit time - set received buffer to PULSE_HALFBIT */
		if(recv_buf[i]>=baud-DALI_HYST && recv_buf[i]<=baud+DALI_HYST) recv_buf[i] = DALI_PULSE_HALFBIT;
		/**if time of pulse around full bit - set received buffer to 2 */
		else if(recv_buf[i]>=2*baud-DALI_HYST && recv_buf[i]<=2*baud+DALI_HYST) recv_buf[i] = DALI_PULSE_FULLBIT;
		/**if received stop signal (long HIGH level) - set recv_buf to 4 */
		else if(recv_buf[i]>=4*baud-DALI_HYST || recv_buf[i]<=30) recv_buf[i] = DALI_PULSE_STOP;
		/**Else set recv_buf to 3 */
		else recv_buf[i] = DALI_PULSE_ERROR;
	}
	/**If there is less than 5 pulses - error (return) */
	if(recv_cnt<=5) return;
	
	bool zero = false; /** current bit zero or not zero flag */
	received = 0;
	recv_bytes = 0;
	/**For every received pulse */
	for(int i = 0;i<recv_cnt;++i)
	{
		if(recv_buf[i] == DALI_PULSE_STOP) break;
		for(int j = i+1;j<recv_cnt;++j)
		{
			/** slide bits by 1 */
			received<<=1;
			/** set received to 0 if zero flag else set to 1 */
			received|=zero?0:1;
			/** received bits++ */
			recv_bytes++;
			/**If received fullbit - switch 1 to 0 or 0 to 1 */
			if(recv_buf[j] == DALI_PULSE_FULLBIT) zero = !zero;
			/** else step further */
			else
			{
				i=j;
				break;
			}
		}
	}
	/** Remove start bitand decrease received bits */
	received ^= (1<<(recv_bytes-1));
	recv_bytes-=1;
	
	/**Add received to debug */
	#ifdef DEBUG_RECEIVER
		recv_list[r_cnt] = received;
		r_cnt++;
	#endif
	
	/** set flag and (if needed) generate EXTI event */
	receive_completed = true;
	if(irq_en) LL_EXTI_GenerateSWI_0_31(dali.callback_line);
}
//

void DaliController::ProcessIC()
{
	/**If receive in process get current time passed and restart counter */
	if(receiving)
	{
		if(LL_TIM_GetCounter(dali.dali_tim) <=30) return; /** For noise only */
		recv_buf[recv_cnt] = LL_TIM_GetCounter(dali.dali_tim);
		LL_TIM_DisableCounter(dali.dali_tim);
		LL_TIM_SetCounter(dali.dali_tim,0);
		LL_TIM_EnableCounter(dali.dali_tim);
		recv_cnt++;

	}
	/**If not receiving and not sending - start reciving (received first bit) */
	if(!receiving && !sending)
	{
		receive_completed = false;
		receiving = true;
		LL_TIM_SetAutoReload(dali.dali_tim,4*DALI_BAUDS[0]); /** 2 stop bits */
		LL_TIM_SetCounter(dali.dali_tim,0);
		LL_TIM_ClearFlag_UPDATE(dali.dali_tim);
		LL_TIM_EnableIT_UPDATE(dali.dali_tim);
		recv_cnt = 0;
	}
}
//

void DaliController::Process(bool tim_flag)
{
	if(sending)
	{
		/**If all bits send (all pulses generated) */
		if(send_cnt == send_len)
		{ 
			/**Disable counter for DALI_EXTI */
			if(dali.type == DALI_EXTI) LL_TIM_DisableCounter(dali.dali_tim);
			/**Set TX to HIGH, set all flags and start listening */
			SetHigh();
			sending = false;
			send_completed = true;
			StartReceiving();
			return;
		}
		/**If not all pulses sent - togle TX pin and set next pulse time */
		LL_GPIO_TogglePin(dali.dali_tx_gpio,dali.dali_tx_pin);
		LL_TIM_SetAutoReload(dali.dali_tim,send_buf[send_cnt]);
		send_cnt++;
	}
	
	if(receiving)
	{
		/**For DALI_IC if UPDATE IRQ happened - stop receiving and decode (as 2 stop bits counted) */
		if(dali.type == DALI_IC)
		{
			recv_buf[recv_cnt] = LL_TIM_GetCounter(dali.dali_tim);
			LL_TIM_DisableIT_UPDATE(dali.dali_tim);
			recv_cnt++;
			receiving = false;
			ReadData();
			return;
		}
		else
		{
			/** If TIM UPDATE IRQ not happened - get current TIM counter and restart it */
			if(!tim_flag)
			{
				if(LL_TIM_GetCounter(dali.dali_tim) <=30) return;
				recv_buf[recv_cnt] = LL_TIM_GetCounter(dali.dali_tim);
				LL_TIM_DisableCounter(dali.dali_tim);
				LL_TIM_SetCounter(dali.dali_tim,0);
				LL_TIM_EnableCounter(dali.dali_tim);
				recv_cnt++;
			}
			/** Else decode */
			else
			{
				recv_buf[recv_cnt] = LL_TIM_GetCounter(dali.dali_tim);
				LL_TIM_DisableCounter(dali.dali_tim);
				recv_cnt++;
				receiving = false;
				ReadData();
				return;
			}
		}
	}
	
	
	/**Start sending if delayed ticked */
	if(delayed)
	{
		delayed = false;
		LL_TIM_DisableCounter(dali.dali_tim);
		StartSending();
		return;
	}
	
	/**If DALI_EXTI and EXTI IRQ happened (with no receiving and sending) - start receive */
	if(!receiving && !sending && dali.type == DALI_EXTI)
    {
        receive_completed = false;
        receiving = true;
        LL_TIM_SetAutoReload(dali.dali_tim,4*417);
        LL_TIM_SetCounter(dali.dali_tim,0);
        LL_TIM_EnableCounter(dali.dali_tim);
        recv_cnt = 0;
    }
	
}
//

void DaliController::StartSending()
{
	/**If not receiving - proceed */
	if(receiving) return;
	StopReceiving();
	
	/**Set first pulse time for UPDATE IRQ, Enable UPDATE IRQ (for DALI_IC) and Enable Counter (for DALI_EXTI) */
	send_cnt = 1;
	sending = true;
	LL_TIM_SetCounter(dali.dali_tim,0);
	LL_TIM_SetAutoReload(dali.dali_tim,send_buf[0]);
	if(dali.type == DALI_IC) 
	{
		LL_TIM_ClearFlag_UPDATE(dali.dali_tim);
		LL_TIM_EnableIT_UPDATE(dali.dali_tim);
	}
	LL_TIM_EnableCounter(dali.dali_tim);
	/**Set TX to LOW */
	LL_GPIO_TogglePin(dali.dali_tx_gpio,dali.dali_tx_pin);
}
//

void DaliController::EnableRecvInterrupt(bool state)
{
	/**Activate or Deactivate Software EXTI line IRQ */
	if(state)
	{
		LL_EXTI_InitTypeDef exti;
		exti.LineCommand = ENABLE;
		exti.Line_0_31 = dali.callback_line;
		exti.Mode = LL_EXTI_MODE_IT;
		exti.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
		LL_EXTI_Init(&exti);
		irq_en = true;
		
		EnableExtiIRQn(dali.callback_line,0);
	}
	else
	{
		LL_EXTI_InitTypeDef exti;
		exti.LineCommand = DISABLE;
		exti.Line_0_31 = dali.callback_line;
		exti.Mode = LL_EXTI_MODE_IT;
		exti.Trigger = LL_EXTI_TRIGGER_NONE;
		LL_EXTI_Init(&exti);
		irq_en = false;
		DisableExtiIRQn(dali.callback_line);
	}
}
//

void DaliController::SetHigh()
{
	switch(dali.logic)
	{
		case DALI_LOGIC_NEGATIVE:
			LL_GPIO_ResetOutputPin(dali.dali_tx_gpio,dali.dali_tx_pin);
		break;
		
		case DALI_LOGIC_POSITIVE:
			LL_GPIO_SetOutputPin(dali.dali_tx_gpio,dali.dali_tx_pin);
		break;
	}
}
//

void DaliController::SetLow()
{
	switch(dali.logic)
	{
		case DALI_LOGIC_NEGATIVE:
			LL_GPIO_SetOutputPin(dali.dali_tx_gpio,dali.dali_tx_pin);
		break;
		
		case DALI_LOGIC_POSITIVE:
			LL_GPIO_ResetOutputPin(dali.dali_tx_gpio,dali.dali_tx_pin);
		break;
	}
}
//

void DaliController::CheckKZ()
{
	/**If RX in LOW state - count, else reset counter */
	switch(dali.logic)
	{
		case DALI_LOGIC_NEGATIVE:
			if(LL_GPIO_ReadInputPort(dali.dali_rx_gpio)&dali.dali_rx_pin)
			{
				kz_counter++;
			}
			else kz_counter = 0;
		break;
		
		case DALI_LOGIC_POSITIVE:
			if(LL_GPIO_ReadInputPort(dali.dali_rx_gpio)&dali.dali_rx_pin)
			{
				kz_counter=0;
			}
			else kz_counter++;
		break;
	}
	
	/**If ~15ms RX on LOW - Short Circuit error */
	if(kz_counter == 150)
	{
		kz_state = true;
		kz_counter = 149;
	}
	else kz_state = false;
}
//

void DaliController::EnableKZCheck(bool state)
{
	/**ENABLE TIM to trigger UPDATE IRQC every 100 us */
	if(state)
	{
		LL_TIM_InitTypeDef tim;
		tim.Autoreload = 100;
		tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
		tim.CounterMode = LL_TIM_COUNTERMODE_UP;
		tim.Prescaler = SystemCoreClock/1000000-1;
		tim.RepetitionCounter = 0;
		LL_TIM_Init(dali.kz_tim,&tim);
		
		LL_TIM_ClearFlag_UPDATE(dali.kz_tim);
		LL_TIM_EnableIT_UPDATE(dali.kz_tim);
		EnableTimIRQn(dali.kz_tim,4);
		LL_TIM_EnableCounter(dali.kz_tim);
		kz_check = true;
	}
	else
	{
		if(kz_check) LL_TIM_DisableCounter(dali.kz_tim);
		kz_check = false;
	}
}
//
