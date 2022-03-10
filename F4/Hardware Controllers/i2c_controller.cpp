#include "i2c_controller.h"

I2CController::I2CController()
{
	
}
//

I2CController::~I2CController()
{

}
//

void I2CController::Init(I2CController_TypeDef contrl)
{
	i2c = contrl;
	InitGPIO();
	InitI2C();
	InitDMA();
	InitTIM();
}
//

void I2CController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	
	gpio.Pin = i2c.scl_pin;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.Alternate = i2c.i2c_scl_af;
	LL_GPIO_Init(i2c.scl_gpio,&gpio);
	
	gpio.Pin = i2c.sda_pin;
	gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.Alternate = i2c.i2c_sda_af;
	LL_GPIO_Init(i2c.sda_gpio,&gpio);
}
//

void I2CController::InitI2C()
{
	LL_I2C_DisableGeneralCall(i2c.i2c);
	//LL_I2C_DisableClockStretching(i2c.i2c);
	
	LL_I2C_InitTypeDef i2c_ini;
	#ifdef LL_I2C_ANALOGFILTER_ENABLE
		i2c_ini.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
		i2c_ini.DigitalFilter = 15;
	#endif
	i2c_ini.OwnAddress1 = 0;
	i2c_ini.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	i2c_ini.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
	i2c_ini.PeripheralMode = LL_I2C_MODE_I2C;
	i2c_ini.ClockSpeed = i2c.speed;
	i2c_ini.TypeAcknowledge = LL_I2C_ACK;
	LL_I2C_Init(i2c.i2c,&i2c_ini);
	
	LL_I2C_EnableClockStretching(i2c.i2c);
	LL_I2C_Enable(i2c.i2c);
}
//

void I2CController::InitDMA()
{
	
}
//

void I2CController::InitTIM()
{
	if(i2c.help_tim == NULL) return;
	
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = I2C_DEFAULT_AWAITER*10;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/10000;
	tim.RepetitionCounter = 0;
	
	LL_TIM_Init(i2c.help_tim,&tim);
	LL_TIM_SetOnePulseMode(i2c.help_tim,LL_TIM_ONEPULSEMODE_SINGLE);
	
	LL_TIM_EnableIT_UPDATE(i2c.help_tim);
	LL_TIM_ClearFlag_UPDATE(i2c.help_tim);
	EnableTimIRQn(i2c.help_tim,0);
	
}
//

I2C_RESULT I2CController::WriteBytes(uint8_t addr,uint8_t* bytes,uint8_t len)
{
	/*ADDR*/
	addr = addr<<1;
	if(LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_ClearFlag_STOP(i2c.i2c);
	if(LL_I2C_IsActiveFlag_ADDR(i2c.i2c)) LL_I2C_ClearFlag_ADDR(i2c.i2c);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c) || LL_I2C_IsActiveFlag_BERR(i2c.i2c) || LL_I2C_IsActiveFlag_AF(i2c.i2c)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.i2c);
	for(int i = 0;i<5;++i)
	{	
		if(LL_I2C_IsActiveFlag_SB(i2c.i2c)) break;
		Await(I2C_DEFAULT_PAUSE);
	}
	if(!LL_I2C_IsActiveFlag_SB(i2c.i2c))
	{
		HardReset();
		return I2C_TIMEOUT;
	}
	
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	Await(I2C_DEFAULT_PAUSE);
	
	/*TRANSMIT*/
	LL_I2C_TransmitData8(i2c.i2c,bytes[0]);
	for(int i = 1;i<len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return I2C_TIMEOUT;
			}
		}
		LL_I2C_TransmitData8(i2c.i2c,bytes[i]);
		Await(I2C_DEFAULT_PAUSE);
		
	}
	
	while(!LL_I2C_IsActiveFlag_BTF(i2c.i2c)) asm("NOP");
	LL_I2C_GenerateStopCondition(i2c.i2c);
	return I2C_WRITE_OK;
}
//

I2C_RESULT I2CController::ReadBytes(uint8_t addr,uint8_t len)
{
	addr = 0x01 | (addr<<1);
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c) || LL_I2C_IsActiveFlag_BERR(i2c.i2c) || LL_I2C_IsActiveFlag_AF(i2c.i2c)) HardReset();
	if(len == 1) LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_NACK);
	else LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_SB(i2c.i2c)) asm("NOP");
	
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	SetTimeout();
	
	for(int i = 0;i<len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.i2c))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return I2C_TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.i2c);
		if(i==len-2) LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_NACK);
	}
	
	LL_I2C_GenerateStopCondition(i2c.i2c);
	//while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	return I2C_RECV_OK;
}
//

I2C_RESULT I2CController::TransceiveBytes(uint8_t addr,uint8_t* s_bytes, uint8_t s_len, uint8_t r_len)
{
	/*ADDR*/
	addr = addr<<1;
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c) || LL_I2C_IsActiveFlag_BERR(i2c.i2c) || LL_I2C_IsActiveFlag_AF(i2c.i2c)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.i2c);
	
	for(int i = 0;i<5;++i)
	{	
		if(LL_I2C_IsActiveFlag_SB(i2c.i2c)) break;
		Await(I2C_DEFAULT_PAUSE);
	}
	if(!LL_I2C_IsActiveFlag_SB(i2c.i2c))
	{
		HardReset();
		return I2C_TIMEOUT;
	}
	
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			if(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	
	
	/*WRITE BYTES*/
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	Await(I2C_DEFAULT_PAUSE);
	LL_I2C_TransmitData8(i2c.i2c,s_bytes[0]);
	
	for(int i = 1;i<s_len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
		{
			if(timeout)
			{
				if(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_GenerateStopCondition(i2c.i2c);
				return I2C_TIMEOUT;
			}
		}
		Await(I2C_DEFAULT_PAUSE);
		LL_I2C_TransmitData8(i2c.i2c,s_bytes[i]);
		
	}
	while(!LL_I2C_IsActiveFlag_BTF(i2c.i2c)) asm("NOP");
	
	
	
	/*RESTART*/
	addr |= 0x01;
	if(r_len == 1) LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_NACK);
	else LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_SB(i2c.i2c)) asm("NOP");
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			if(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	
	/*READ BYTES*/
	for(int i = 0;i<r_len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.i2c))
		{
			if(timeout)
			{
				if(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_GenerateStopCondition(i2c.i2c);
				return I2C_TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.i2c);
		Await(I2C_DEFAULT_PAUSE);
		if(i==r_len-2) LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_NACK);
	}
	
	if(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_GenerateStopCondition(i2c.i2c);
	//while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	return I2C_RECV_OK;
}
//

I2C_RESULT I2CController::ReadRegister(uint8_t addr, uint8_t reg, uint8_t len)
{
	
	/*ADDR*/
	addr = addr<<1;
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c) || LL_I2C_IsActiveFlag_BERR(i2c.i2c) || LL_I2C_IsActiveFlag_AF(i2c.i2c)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_SB(i2c.i2c)) asm("NOP");
	
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	
	/*SET REGISTER*/
	LL_I2C_TransmitData8(i2c.i2c,reg);
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
	{
		if(timeout)
		{
			LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	
	
	/*RESTART*/
	addr |= 0x01;
	if(len == 1) LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_NACK);
	else LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_SB(i2c.i2c)) asm("NOP");
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.i2c);

	/*READ_REGS*/
	for(int i = 0;i<len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.i2c))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return I2C_TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.i2c);
		if(i==len-2) LL_I2C_AcknowledgeNextData(i2c.i2c,LL_I2C_NACK);
	}
	
	LL_I2C_GenerateStopCondition(i2c.i2c);
	//while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	return I2C_RECV_OK;
	
}
//

I2C_RESULT I2CController::WriteRegister(uint8_t addr, uint8_t reg, uint8_t* bytes, uint8_t len)
{
	
	/*ADDR*/
	addr = addr<<1;
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c) || LL_I2C_IsActiveFlag_BERR(i2c.i2c) || LL_I2C_IsActiveFlag_AF(i2c.i2c)) HardReset();

	LL_I2C_GenerateStartCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_SB(i2c.i2c)) asm("NOP");
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			LL_I2C_GenerateStopCondition(i2c.i2c);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.i2c);
	
	
	LL_I2C_TransmitData8(i2c.i2c,reg);
	for(int i = 0;i<len+1;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return I2C_TIMEOUT;
			}
		}
		if(i == len) break;;
		LL_I2C_TransmitData8(i2c.i2c,bytes[i]);
		
	}
		
	
	LL_TIM_DisableCounter(i2c.help_tim);
	while(!LL_I2C_IsActiveFlag_BTF(i2c.i2c)) asm("NOP");
	LL_I2C_GenerateStopCondition(i2c.i2c);
	return I2C_WRITE_OK;
}
//

void I2CController::SetTimeout()
{
	if(i2c.help_tim == NULL) return;
	LL_TIM_DisableCounter(i2c.help_tim);
	LL_TIM_ClearFlag_UPDATE(i2c.help_tim);
	timeout = false;
	LL_TIM_SetAutoReload(i2c.help_tim,I2C_DEFAULT_AWAITER*10);
	LL_TIM_SetCounter(i2c.help_tim,0);
	LL_TIM_ClearFlag_UPDATE(i2c.help_tim);
	LL_TIM_EnableCounter(i2c.help_tim);
}
//

void I2CController::TimeoutHandler()
{
	timeout = true;
}
//

void I2CController::SoftReset()
{
	LL_I2C_Disable(i2c.i2c);
	while(LL_I2C_IsEnabled(i2c.i2c)) ;
	LL_I2C_Enable(i2c.i2c);
}
//

void I2CController::HardReset()
{
	LL_I2C_ClearFlag_AF(i2c.i2c);
	LL_I2C_ClearFlag_ARLO(i2c.i2c);
	LL_I2C_ClearFlag_BERR(i2c.i2c);
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_Disable(i2c.i2c);
	while(LL_I2C_IsEnabled(i2c.i2c)) ;
	
	LL_GPIO_SetPinMode(i2c.scl_gpio,i2c.scl_pin,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(i2c.sda_gpio,i2c.sda_pin,LL_GPIO_MODE_OUTPUT);
	
	for(int i = 0;i<50;++i)
	{
		LL_GPIO_TogglePin(i2c.scl_gpio,i2c.scl_pin);
		Await(20);
		LL_GPIO_TogglePin(i2c.sda_gpio,i2c.sda_pin);
		Await(20);
	}
	
	LL_GPIO_SetPinMode(i2c.scl_gpio,i2c.scl_pin,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(i2c.sda_gpio,i2c.sda_pin,LL_GPIO_MODE_ALTERNATE);
	
	LL_I2C_EnableReset(i2c.i2c);
	LL_I2C_DisableReset(i2c.i2c);
	InitI2C();
	
	//LL_I2C_Enable(i2c.i2c);
}
//

void I2CController::GetAddressList()
{
	
	for(int i = 0;i<16;++i) addr_list[i] = 0;
	
	for(int i = 0;i<16;++i)
	{
		for(int j = 0;j<8;++j)
		{
			if(IsOnline(j + 8*i))
			{
				addr_list[i]|=1<<j;
			}
		}
	}
}
//

bool I2CController::IsOnline(uint8_t addr)
{
	bool ret = false;
	addr = addr<<1;
	if(LL_I2C_IsActiveFlag_STOP(i2c.i2c)) LL_I2C_ClearFlag_STOP(i2c.i2c);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c) || LL_I2C_IsActiveFlag_BERR(i2c.i2c)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.i2c);
	Await(1);
	while(!LL_I2C_IsActiveFlag_SB(i2c.i2c)) asm("NOP");
	LL_I2C_TransmitData8(i2c.i2c,addr);
	
	SetTimeout();
	
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c)) 
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
		{
			break;
		}
	}
	if(!timeout) ret = true;
	Await(5);
	
	LL_I2C_GenerateStopCondition(i2c.i2c);
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c) && !timeout) ;
	
	return ret;
}
//

void I2CController::Await(uint8_t msec)
{
	timeout = false;
	LL_TIM_DisableCounter(i2c.help_tim);
	LL_TIM_SetAutoReload(i2c.help_tim,msec);
	LL_TIM_SetCounter(i2c.help_tim,0);
	LL_TIM_EnableCounter(i2c.help_tim);
	while(!timeout) ;
	LL_TIM_SetAutoReload(i2c.help_tim,I2C_DEFAULT_AWAITER*10);
	timeout = false;
}
//
