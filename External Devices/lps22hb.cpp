#include "lps22hb.h"

void LPS22HB::Init(I2CController* i2c)
{
	this->i2c = i2c;
	CheckDevice();
	SetupDevice();
}
//

/*Check if device is LPS22HB*/
void LPS22HB::CheckDevice()
{
	/*If already checked return*/
	if(flags.inited) return;
	
	/*Check WHOAMI register*/
	if(i2c->ReadRegister(LPS22HB_ADDR,LPS22HB_REG_WHO_AM_I,1) == I2C_RECV_OK)
	{
		//GPIOB->BRR = (1<<HLR);
		if(i2c->received[0] == LPS22HB_WHO_AM_I)
		{
			flags.inited = true;
		}
		else flags.inited = false;
		//i2c_hw.StopLine();
	}
}
//

/*Setup Device*/
void LPS22HB::SetupDevice()
{
	
	/*If not inited init or if already setupped return*/
	if(!flags.inited)
	{		
		CheckDevice();
		return;
	}
	if(flags.setupped) return;
	
	/*Setup and launch LPS22HB (read datasheet)*/
	uint8_t send[2];
	send[0] = _LPS22HB_CTRL_REG1_BDU_EN | _LPS22HB_CTRL_REG1_LPFP_EN | _LPS22HB_CTRL_REG1_ODR_10Hz;
	send[1] = _LS22HB_CTRL_REG2_ADD_INC_DIS;
	
	i2c->WriteRegister(LPS22HB_ADDR,LPS22HB_REG_CTRL_REG2,send+1,1);
	i2c->WriteRegister(LPS22HB_ADDR,LPS22HB_REG_CTRL_REG1,send,1);
	
	/*Check if everything is OK*/
	if(i2c->ReadRegister(LPS22HB_ADDR,LPS22HB_REG_CTRL_REG1,1) == I2C_RECV_OK)
	{
		if(i2c->received[0] == send[0]) flags.setupped = true;
	}
}
//

/*Read pressure and temperature*/
void LPS22HB::Process()
{
	
	//temp=25.0;
	//pressure = 1020.0;
	//return;
	
	/*If not inited or setupped init or setup*/
	if(!flags.inited)
	{		
		CheckDevice();
		return;
	}
	if(!flags.setupped)
	{		
		SetupDevice();
		return;
	}
	
	/*Read status register*/
	uint8_t status;
	if(i2c->ReadRegister(LPS22HB_ADDR,LPS22HB_REG_STATUS,1) == I2C_RECV_OK) status = i2c->received[0];
	else return;
	
	/*If data is ready read it*/
	if(status &(1<<__LPS22HB_STATUS_P_DA)) CountPressure();
	if(status &(1<<__LPS22HB_STATUS_T_DA)) CountTemp();
}
//

/*Count pressure*/
void LPS22HB::CountPressure()
{
	
	/*Read pressure data*/
	int P_OUT = 0;
	if(i2c->ReadRegister(LPS22HB_ADDR,LPS22HB_REG_PRESS_OUT_XL,3) != I2C_RECV_OK) return;
	P_OUT = (i2c->received[2]<<16) | (i2c->received[1]<<8) | (i2c->received[0]);
	
	/*Count pressure (see datasheet)*/
	if(P_OUT &0x00800000) P_OUT |= 0xFF000000;
	pressure = P_OUT/4096.f;
}
//

/*Count temperature*/
void LPS22HB::CountTemp()
{
	/*Read temperature*/
	uint16_t T_OUT = 0;
	if(i2c->ReadRegister(LPS22HB_ADDR,LPS22HB_REG_TEMP_OUT_L,2) != I2C_RECV_OK) return;
	T_OUT = (i2c->received[1]<<8) | (i2c->received[0]);
	
	/*Count temperature (see datasheet)*/
	temp = T_OUT/100.0f;
}
//
void LPS22HB::GetStatus(struct LpsFlags* flags)
{
	flags = &(this->flags);
}
