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
	gpio.Alternate = i2c.i2c_af;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = i2c.scl_pin;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(i2c.scl_gpio,&gpio);
	
	gpio.Pin = i2c.sda_pin;
	LL_GPIO_Init(i2c.sda_gpio,&gpio);
}
//

void I2CController::InitI2C()
{
	LL_I2C_DisableOwnAddress1(i2c.i2c);
	LL_I2C_DisableGeneralCall(i2c.i2c);
	//LL_I2C_EnableClockStretching(i2c.i2c);
	LL_I2C_InitTypeDef i2c_ini;
	i2c_ini.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	i2c_ini.DigitalFilter = 0;
	i2c_ini.OwnAddress1 = 0;
	i2c_ini.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	i2c_ini.PeripheralMode = LL_I2C_MODE_I2C;
	i2c_ini.Timing = i2c.timing;
	i2c_ini.TypeAcknowledge = LL_I2C_ACK;
	LL_I2C_Init(i2c.i2c,&i2c_ini);
	//LL_I2C_EnableAutoEndMode(i2c.i2c);
	LL_I2C_SetOwnAddress2(i2c.i2c,0,LL_I2C_OWNADDRESS2_NOMASK);
	
	LL_I2C_Enable(i2c.i2c);
}
//

void I2CController::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = 20;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/1000;
	tim.RepetitionCounter = 0;
	
	LL_TIM_Init(i2c.help_tim,&tim);
	LL_TIM_SetOnePulseMode(i2c.help_tim,LL_TIM_ONEPULSEMODE_SINGLE);
	
	LL_TIM_EnableIT_UPDATE(i2c.help_tim);
	LL_TIM_ClearFlag_UPDATE(i2c.help_tim);
	EnableTimIRQn(i2c.help_tim,0);
	
}
//

void I2CController::InitDMA()
{
	
}
//

I2C_RESULT I2CController::WriteBytes(uint8_t addr,uint8_t* bytes,uint8_t len)
{
	addr = addr<<1;
	
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c)) SoftReset();
	
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_NACK(i2c.i2c);
	LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_WRITE);
	LL_I2C_SetSlaveAddr(i2c.i2c,addr);
	LL_I2C_SetTransferSize(i2c.i2c,len);
	LL_I2C_TransmitData8(i2c.i2c,bytes[0]);
	LL_I2C_GenerateStartCondition(i2c.i2c);
	
	for(int i = 1;i<len;++i)
	{
		LL_TIM_DisableCounter(i2c.help_tim);
		timeout = false;
		LL_TIM_SetCounter(i2c.help_tim,0);
		LL_TIM_EnableCounter(i2c.help_tim);
		while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
		{
			if(timeout || LL_I2C_IsActiveFlag_NACK(i2c.i2c))
			{
				//LL_I2C_GenerateStopCondition(i2c.I2C);
				return TIMEOUT;
			}
		}
		LL_I2C_TransmitData8(i2c.i2c,bytes[i]);
		
	}
	
	
	LL_TIM_DisableCounter(i2c.help_tim);
	LL_I2C_GenerateStopCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	
	return WRITE_OK;
}
//

I2C_RESULT I2CController::ReadBytes(uint8_t addr,uint8_t len)
{
	addr = (addr<<1);
	
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c)) SoftReset();
	
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_NACK(i2c.i2c);
	LL_I2C_ClearFlag_BERR(i2c.i2c);
	LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_READ);
	LL_I2C_SetSlaveAddr(i2c.i2c,addr);
	LL_I2C_SetTransferSize(i2c.i2c,len);
	LL_I2C_GenerateStartCondition(i2c.i2c);
	
	timeout = false;
	/*LL_TIM_SetCounter(i2c.TIMER,0);
	LL_TIM_EnableCounter(i2c.TIMER);
	
	while(!LL_I2C_IsActiveFlag_ADDR(i2c.I2C)) 
	{
		if(timeout)
		{
			LL_I2C_GenerateStopCondition(i2c.I2C);
			return TIMEOUT;
		}
	}
	LL_TIM_DisableCounter(i2c.TIMER);*/
	
	for(int i = 0;i<len;++i)
	{
		LL_TIM_DisableCounter(i2c.help_tim);
		timeout = false;
		LL_TIM_SetCounter(i2c.help_tim,0);
		LL_TIM_EnableCounter(i2c.help_tim);
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.i2c))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.i2c);
	}
	
	LL_TIM_DisableCounter(i2c.help_tim);
	LL_I2C_GenerateStopCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	return RECV_OK;
}
//

I2C_RESULT I2CController::ReadRegister(uint8_t addr, uint8_t reg, uint8_t len)
{
	addr = addr<<1;
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c)) SoftReset();
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_NACK(i2c.i2c);
	LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_WRITE);
	LL_I2C_SetSlaveAddr(i2c.i2c,addr);
	LL_I2C_SetTransferSize(i2c.i2c,1);
	LL_I2C_TransmitData8(i2c.i2c,reg);
	LL_I2C_GenerateStartCondition(i2c.i2c);
	
	LL_TIM_DisableCounter(i2c.help_tim);
	timeout = false;
	LL_TIM_SetCounter(i2c.help_tim,0);
	LL_TIM_EnableCounter(i2c.help_tim);
	while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
	{
		if(timeout || LL_I2C_IsActiveFlag_NACK(i2c.i2c))
		{
			//LL_I2C_GenerateStopCondition(i2c.I2C);
			return TIMEOUT;
		}
	}
	
	
	LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_READ);
	LL_I2C_SetTransferSize(i2c.i2c,len);
	LL_I2C_GenerateStartCondition(i2c.i2c);

	
	for(int i = 0;i<len;++i)
	{
		LL_TIM_DisableCounter(i2c.help_tim);
		timeout = false;
		LL_TIM_SetCounter(i2c.help_tim,0);
		LL_TIM_EnableCounter(i2c.help_tim);
		while(!LL_I2C_IsActiveFlag_RXNE(i2c.i2c))
		{
			if(timeout)
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return TIMEOUT;
			}
		}
		received[i] = LL_I2C_ReceiveData8(i2c.i2c);
	}
	
	LL_TIM_DisableCounter(i2c.help_tim);
	LL_I2C_GenerateStopCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	return RECV_OK;
	
}
//

I2C_RESULT I2CController::WriteRegister(uint8_t addr, uint8_t reg, uint8_t* bytes, uint8_t len, bool restart)
{
	addr = addr<<1;
	if(LL_I2C_IsActiveFlag_BUSY(i2c.i2c)) SoftReset();
	LL_I2C_ClearFlag_STOP(i2c.i2c);
	LL_I2C_ClearFlag_NACK(i2c.i2c);
	LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_WRITE);
	LL_I2C_SetSlaveAddr(i2c.i2c,addr);
	if(!restart)
	{
		LL_I2C_SetTransferSize(i2c.i2c,len+1);
		LL_I2C_TransmitData8(i2c.i2c,reg);
		LL_I2C_GenerateStartCondition(i2c.i2c);
		
		for(int i = 0;i<len+1;++i)
		{
			LL_TIM_DisableCounter(i2c.help_tim);
			timeout = false;
			LL_TIM_SetCounter(i2c.help_tim,0);
			LL_TIM_EnableCounter(i2c.help_tim);
			while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
			{
				if(timeout || LL_I2C_IsActiveFlag_NACK(i2c.i2c))
				{
					LL_I2C_GenerateStopCondition(i2c.i2c);
					return TIMEOUT;
				}
			}
			if(i == len) break;;
			LL_I2C_TransmitData8(i2c.i2c,bytes[i]);
			
		}
		
		
		
	}
	else
	{
		LL_TIM_DisableCounter(i2c.help_tim);
		timeout = false;
		LL_TIM_SetCounter(i2c.help_tim,0);
		LL_TIM_EnableCounter(i2c.help_tim);
		while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
		{
			if(timeout || LL_I2C_IsActiveFlag_NACK(i2c.i2c))
			{
				LL_I2C_GenerateStopCondition(i2c.i2c);
				return TIMEOUT;
			}
		}
			
		LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_WRITE);
		LL_I2C_SetTransferSize(i2c.i2c,len);
		LL_I2C_TransmitData8(i2c.i2c,bytes[0]);
		LL_I2C_GenerateStartCondition(i2c.i2c);
		
		for(int i = 1;i<len;++i)
		{
			LL_TIM_DisableCounter(i2c.help_tim);
			timeout = false;
			LL_TIM_SetCounter(i2c.help_tim,0);
			LL_TIM_EnableCounter(i2c.help_tim);
			while(!LL_I2C_IsActiveFlag_TXE(i2c.i2c))
			{
				if(timeout || LL_I2C_IsActiveFlag_NACK(i2c.i2c))
				{
					LL_I2C_GenerateStopCondition(i2c.i2c);
					return TIMEOUT;
				}
			}
			LL_I2C_TransmitData8(i2c.i2c,bytes[i]);
			
		}
	}
	
	LL_TIM_DisableCounter(i2c.help_tim);
	LL_I2C_GenerateStopCondition(i2c.i2c);
	while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c)) ;
	return I2C_RESULT::WRITE_OK;
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

void I2CController::GetAddressList()
{
	for(int i = 0;i<16;++i) addr_list[i] = 0;
	
	for(int i = 0;i<16;++i)
	{
		for(int j = 0;j<8;++j)
		{
			LL_I2C_ClearFlag_STOP(i2c.i2c);
			LL_I2C_ClearFlag_NACK(i2c.i2c);
			LL_I2C_SetTransferRequest(i2c.i2c,LL_I2C_REQUEST_WRITE);
			LL_I2C_SetTransferSize(i2c.i2c,0);
			LL_I2C_SetSlaveAddr(i2c.i2c,(j + 8*i)<<1);
			LL_I2C_GenerateStartCondition(i2c.i2c);
			
			timeout = false;
			LL_TIM_SetCounter(i2c.help_tim,0);
			LL_TIM_EnableCounter(i2c.help_tim);
			
			while(!LL_I2C_IsActiveFlag_ADDR(i2c.i2c)) 
			{
				if(timeout || LL_I2C_IsActiveFlag_NACK(i2c.i2c) || LL_I2C_IsActiveFlag_STOP(i2c.i2c))
				{
					break;
				}
			}
			LL_TIM_DisableCounter(i2c.help_tim);
			if(!timeout && !LL_I2C_IsActiveFlag_NACK(i2c.i2c)) addr_list[i]|=1<<j;
			Await(5);
			
			//LL_I2C_GenerateStopCondition(i2c.I2C);
			while(!LL_I2C_IsActiveFlag_STOP(i2c.i2c))
			{
				
			}
		}
	}
}
//

void I2CController::Await(uint8_t msec)
{
	timeout = false;
	LL_TIM_SetAutoReload(i2c.help_tim,msec);
	LL_TIM_SetCounter(i2c.help_tim,0);
	LL_TIM_EnableCounter(i2c.help_tim);
	while(!timeout) ;
	LL_TIM_SetAutoReload(i2c.help_tim,10);
	timeout = false;
}
//
