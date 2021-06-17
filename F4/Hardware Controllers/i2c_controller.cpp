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
}
//

void I2CController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	
	gpio.Pin = i2c.SCL_PIN;
	gpio.Alternate = i2c.I2C_SCL_AF;
	LL_GPIO_Init(i2c.SCL_GPIO,&gpio);
	
	gpio.Pin = i2c.SDA_PIN;
	gpio.Alternate = i2c.I2C_SDA_AF;
	LL_GPIO_Init(i2c.SDA_GPIO,&gpio);
}
//

void I2CController::InitI2C()
{
	LL_I2C_DisableGeneralCall(i2c.I2C);
	LL_I2C_DisableClockStretching(i2c.I2C);
	
	LL_I2C_InitTypeDef i2c_ini;
	i2c_ini.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	i2c_ini.DigitalFilter = 15;
	i2c_ini.OwnAddress1 = 0;
	i2c_ini.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	i2c_ini.DutyCycle = LL_I2C_DUTYCYCLE_2;
	i2c_ini.PeripheralMode = LL_I2C_MODE_I2C;
	i2c_ini.ClockSpeed = 400000;
	i2c_ini.TypeAcknowledge = LL_I2C_ACK;
	LL_I2C_Init(i2c.I2C,&i2c_ini);
	
		
	LL_I2C_Enable(i2c.I2C);
}
//

void I2CController::InitDMA()
{
	
}
//

I2C_RESULT I2CController::WriteBytes(uint8_t addr,uint8_t* bytes,uint8_t len)
{
	addr = addr<<1;
	LL_I2C_ClearFlag_STOP(i2c.I2C);
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.I2C) || LL_I2C_IsActiveFlag_BERR(i2c.I2C)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.I2C);
	while(!LL_I2C_IsActiveFlag_SB(i2c.I2C)) asm("NOP");
	
	LL_I2C_TransmitData8(i2c.I2C,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.I2C))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.I2C))
		{
			LL_I2C_GenerateStopCondition(i2c.I2C);
			return I2C_TIMEOUT;
		}
	}
	
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	LL_I2C_TransmitData8(i2c.I2C,bytes[0]);
	
	for(int i = 1;i<len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_TXE(i2c.I2C))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.I2C);
				return I2C_TIMEOUT;
			}
		}
		LL_I2C_TransmitData8(i2c.I2C,bytes[i]);
		
	}
	
	while(!LL_I2C_IsActiveFlag_BTF(i2c.I2C)) asm("NOP");
	LL_I2C_GenerateStopCondition(i2c.I2C);
	return I2C_WRITE_OK;
}
//

I2C_RESULT I2CController::ReadBytes(uint8_t addr,uint8_t len)
{
	addr = 0x01 | (addr<<1);
	LL_I2C_ClearFlag_STOP(i2c.I2C);
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.I2C) || LL_I2C_IsActiveFlag_BERR(i2c.I2C)) HardReset();
	if(len == 1) LL_I2C_AcknowledgeNextData(i2c.I2C,LL_I2C_NACK);
	else LL_I2C_AcknowledgeNextData(i2c.I2C,LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(i2c.I2C);
	while(!LL_I2C_IsActiveFlag_SB(i2c.I2C)) asm("NOP");
	
	LL_I2C_TransmitData8(i2c.I2C,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.I2C))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.I2C))
		{
			LL_I2C_GenerateStopCondition(i2c.I2C);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	SetTimeout();
	
	for(int i = 0;i<len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.I2C))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.I2C);
				return I2C_TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.I2C);
		if(i==len-2) LL_I2C_AcknowledgeNextData(i2c.I2C,LL_I2C_NACK);
	}
	
	LL_I2C_GenerateStopCondition(i2c.I2C);
	//while(!LL_I2C_IsActiveFlag_STOP(i2c.I2C)) ;
	return I2C_RECV_OK;
}
//

I2C_RESULT I2CController::TransceiveBytes(uint8_t addr,uint8_t* s_bytes, uint8_t s_len, uint8_t r_len)
{
	addr = addr<<1;
	LL_I2C_ClearFlag_STOP(i2c.I2C);
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.I2C) || LL_I2C_IsActiveFlag_BERR(i2c.I2C)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.I2C);
	while(!LL_I2C_IsActiveFlag_SB(i2c.I2C)) asm("NOP");
	
	LL_I2C_TransmitData8(i2c.I2C,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.I2C))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.I2C))
		{
			LL_I2C_GenerateStopCondition(i2c.I2C);
			return I2C_TIMEOUT;
		}
	}
	
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	LL_I2C_TransmitData8(i2c.I2C,s_bytes[0]);
	
	for(int i = 1;i<s_len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_TXE(i2c.I2C))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.I2C);
				return I2C_TIMEOUT;
			}
		}
		LL_I2C_TransmitData8(i2c.I2C,s_bytes[i]);
		
	}
	while(!LL_I2C_IsActiveFlag_BTF(i2c.I2C)) asm("NOP");
	
	addr |= 0x01;
	//LL_I2C_ClearFlag_STOP(i2c.I2C);
	//LL_I2C_ClearFlag_ADDR(i2c.I2C);
	if(r_len == 1) LL_I2C_AcknowledgeNextData(i2c.I2C,LL_I2C_NACK);
	else LL_I2C_AcknowledgeNextData(i2c.I2C,LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(i2c.I2C);
	while(!LL_I2C_IsActiveFlag_SB(i2c.I2C)) asm("NOP");
	
	LL_I2C_TransmitData8(i2c.I2C,addr);
	
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.I2C))
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.I2C))
		{
			LL_I2C_GenerateStopCondition(i2c.I2C);
			return I2C_TIMEOUT;
		}
	}
	LL_I2C_ClearFlag_ADDR(i2c.I2C);
	SetTimeout();
	
	for(int i = 0;i<r_len;++i)
	{
		SetTimeout();
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.I2C))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.I2C);
				return I2C_TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.I2C);
		if(i==r_len-2) LL_I2C_AcknowledgeNextData(i2c.I2C,LL_I2C_NACK);
	}
	
	LL_I2C_GenerateStopCondition(i2c.I2C);
	//while(!LL_I2C_IsActiveFlag_STOP(i2c.I2C)) ;
	return I2C_RECV_OK;
}
//

void I2CController::SetTimeout(uint8_t msec)
{
	timeout = false;
	timer = msec;
}
//

void I2CController::TimeoutHandler()
{
	if(timer)
	{
		timer--;
		if(!timer) timeout = true;
	}
}
//

void I2CController::SoftReset()
{
	LL_I2C_Disable(i2c.I2C);
	while(LL_I2C_IsEnabled(i2c.I2C)) ;
	LL_I2C_Enable(i2c.I2C);
}
//

void I2CController::HardReset()
{
	LL_I2C_ClearFlag_AF(i2c.I2C);
	LL_I2C_ClearFlag_ARLO(i2c.I2C);
	LL_I2C_ClearFlag_BERR(i2c.I2C);
	LL_I2C_ClearFlag_STOP(i2c.I2C);
	LL_I2C_Disable(i2c.I2C);
	while(LL_I2C_IsEnabled(i2c.I2C)) ;
	
	LL_GPIO_SetPinMode(i2c.SCL_GPIO,i2c.SCL_PIN,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(i2c.SDA_GPIO,i2c.SDA_PIN,LL_GPIO_MODE_OUTPUT);
	
	for(int i = 0;i<50;++i)
	{
		LL_GPIO_TogglePin(i2c.SCL_GPIO,i2c.SCL_PIN);
		Await(1);
		LL_GPIO_TogglePin(i2c.SDA_GPIO,i2c.SDA_PIN);
		Await(1);
	}
	
	LL_GPIO_SetPinMode(i2c.SCL_GPIO,i2c.SCL_PIN,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(i2c.SDA_GPIO,i2c.SDA_PIN,LL_GPIO_MODE_ALTERNATE);
	
	LL_I2C_EnableReset(i2c.I2C);
	LL_I2C_DisableReset(i2c.I2C);
	InitI2C();
	
	//LL_I2C_Enable(i2c.I2C);
}
//

void I2CController::GetAddressList()
{
	
	for(int i = 0;i<16;++i) addr_list[i] = 0;
	
	for(int i = 0;i<16;++i)
	{
		for(int j = 0;j<8;++j)
		{
			if(IsOnline(j + 8*i)) addr_list[i]|=1<<j;
		}
	}
}
//

bool I2CController::IsOnline(uint8_t addr)
{
	bool ret = false;
	addr = addr<<1;
	if(LL_I2C_IsActiveFlag_STOP(i2c.I2C)) LL_I2C_ClearFlag_STOP(i2c.I2C);
	if(LL_I2C_IsActiveFlag_BUSY(i2c.I2C) || LL_I2C_IsActiveFlag_BERR(i2c.I2C)) HardReset();
	LL_I2C_GenerateStartCondition(i2c.I2C);
	Await(1);
	while(!LL_I2C_IsActiveFlag_SB(i2c.I2C)) asm("NOP");
	LL_I2C_TransmitData8(i2c.I2C,addr);
	
	SetTimeout();
	
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.I2C)) 
	{
		if(timeout || LL_I2C_IsActiveFlag_STOP(i2c.I2C))
		{
			break;
		}
	}
	if(!timeout) ret = true;
	Await(5);
	
	LL_I2C_GenerateStopCondition(i2c.I2C);
	SetTimeout();
	while(!LL_I2C_IsActiveFlag_STOP(i2c.I2C) && !timeout) ;
	
	return ret;
}
//

void I2CController::Await(uint8_t msec)
{
	SetTimeout(msec);
	while(!timeout) asm("NOP");
	timeout = false;
}
//
