#include "dali_controller.h"

DaliController::DaliController()
{
	irq_en = false;
	led_en = false;
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
	dali = get;
	InitGPIO();
	InitTIM();
	receiving = false;
	sending = false;
	StartReceiving();
	//StopReceiving();
}
//

void DaliController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.Pin = dali.dali_tx_pin;
	LL_GPIO_Init(dali.dali_tx_gpio,&gpio);
	
	gpio.Mode = LL_GPIO_MODE_INPUT;
	gpio.Pin = dali.dali_rx_pin;
	LL_GPIO_Init(dali.dali_rx_gpio,&gpio);
	SetHigh();
	
	
	LL_EXTI_InitTypeDef exti;
	exti.LineCommand = ENABLE;
	exti.Line_0_31 = GetExtiLine(dali.dali_rx_pin);
	exti.Mode = LL_EXTI_MODE_IT;
	exti.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
	SetExtiSource(dali.dali_tx_gpio,GetExtiLine(dali.dali_rx_pin));
	LL_EXTI_Init(&exti);
	
	EnableExtiIRQn(GetExtiLine(dali.dali_rx_pin),0);
}
//


void DaliController::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	
	tim.Autoreload = 1000;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/1000000-1;
	tim.RepetitionCounter = 0;
	LL_TIM_Init(dali.dali_tim,&tim);
	
	LL_TIM_ClearFlag_UPDATE(dali.dali_tim);
	LL_TIM_EnableIT_UPDATE(dali.dali_tim);
	
	EnableTimIRQn(dali.dali_tim,0);

}
//

void DaliController::StartReceiving()
{
	
	LL_EXTI_ClearFlag_0_31(GetExtiLine(dali.dali_rx_pin));
	LL_EXTI_EnableIT_0_31(GetExtiLine(dali.dali_rx_pin));
}
//

void DaliController::StopReceiving()
{
	LL_EXTI_DisableIT_0_31(GetExtiLine(dali.dali_rx_pin));
}
//

void DaliController::SendDelayed(uint32_t mess,uint8_t n_bits, uint32_t delay, uint16_t baud)
{
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
	
	delayed = true;
	
	StopReceiving();
	if(receiving)
	{
		StartReceiving();
		return;
	}
	
	LL_TIM_DisableCounter(dali.dali_tim);
	LL_TIM_SetCounter(dali.dali_tim,0);
	LL_TIM_SetAutoReload(dali.dali_tim,delay);
	LL_TIM_EnableCounter(dali.dali_tim);
}
//

void DaliController::Send(uint32_t mess,uint8_t n_bits, uint16_t baud)
{
	send_completed = false;
	uint32_t to_send =(1<<(n_bits)) | mess;
	uint8_t bits[32];
	for(int i = 0;i<n_bits+1;++i) bits[i] = (to_send>>(n_bits-i))&1;
	send_len = 0;
	
	uint16_t halfbit = (1000000/baud)/2;
	
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
	
	StartSending();
}
//

void DaliController::ReadData()
{
	uint16_t baud = 1000000/(recv_buf[0]*2);
	for(uint8_t i = 0;i<DALI_BAUDS_COUNT;++i)
	{
		if(baud>=DALI_BAUDS[i]-150 && baud<=DALI_BAUDS[i]+150)
		{			
			baud = (1000000/DALI_BAUDS[i])/2;
		}
	}
	
	for(int i = 0;i<recv_cnt;++i)
	{
		if(recv_buf[i]>=baud-DALI_HYST && recv_buf[i]<=baud+DALI_HYST) recv_buf[i] = 1;
		else if(recv_buf[i]>=2*baud-DALI_HYST && recv_buf[i]<=2*baud+DALI_HYST) recv_buf[i] = 2;
		else if(recv_buf[i]>=4*baud-DALI_HYST || recv_buf[i]<=30) recv_buf[i] = 4;
		else recv_buf[i] = 3;
	}
	if(recv_cnt<=5) return;
	bool zero = false;
	
	received = 0;
	recv_bytes = 0;
	for(int i = 0;i<recv_cnt;++i)
	{
		if(recv_buf[i] == 4) break;
		for(int j = i+1;j<recv_cnt;++j)
		{
			received<<=1;
			received|=zero?0:1;
			recv_bytes++;
			if(recv_buf[j] == 2) zero = !zero;
			else
			{
				i=j;
				break;
			}
		}
	}
	received ^= (1<<(recv_bytes-1));
	recv_bytes-=1;
	
	#ifdef DEBUG_RECEIVER
		recv_list[r_cnt] = received;
		r_cnt++;
	#endif
	
	receive_completed = true;
	if(irq_en) LL_EXTI_GenerateSWI_0_31(dali.callback_line);
}
//

void DaliController::Process(bool tim_flag)
{
	if(sending)
	{
		if(send_cnt == send_len)
		{
			LL_TIM_DisableCounter(dali.dali_tim);
			SetHigh();
			sending = false;
			send_completed = true;
			StartReceiving();
			return;
		}
		LL_GPIO_TogglePin(dali.dali_tx_gpio,dali.dali_tx_pin);
		LL_TIM_SetAutoReload(dali.dali_tim,send_buf[send_cnt]);
		send_cnt++;
	}
	
	if(receiving)
	{
		if(!tim_flag)
		{
			recv_buf[recv_cnt] = LL_TIM_GetCounter(dali.dali_tim);
			LL_TIM_DisableCounter(dali.dali_tim);
			LL_TIM_SetCounter(dali.dali_tim,0);
			LL_TIM_EnableCounter(dali.dali_tim);
			recv_cnt++;
		}
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
	
	if(delayed)
	{
		delayed = false;
		LL_TIM_DisableCounter(dali.dali_tim);
		StartSending();
		return;
	}
	
	if(!receiving && !sending)
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
	StopReceiving();
	if(receiving)
	{
		StartReceiving();
		return;
	}
	send_cnt = 1;
	sending = true;
	LL_TIM_SetCounter(dali.dali_tim,0);
	LL_TIM_SetAutoReload(dali.dali_tim,send_buf[0]);
	LL_TIM_EnableCounter(dali.dali_tim);
	LL_GPIO_TogglePin(dali.dali_tx_gpio,dali.dali_tx_pin);
}
//

void DaliController::EnableRecvInterrupt(bool state)
{
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
