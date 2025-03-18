#include "hts221.h"

void HTS221::Init(I2CController* i2c)
{
	this->i2c = i2c;
	CheckDevice();
	SetupDevice();
}
//

void HTS221::CheckDevice()
{
	if(flags.inited == true) return;
	
	/*Else read WHOAMI register*/
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_WHO_AM_I,1) == I2C_RECV_OK)
	{
		if(i2c->received[0] == 0xBC)
		{
			flags.inited = true;
		}
		else flags.inited = false;
		//i2c_hw.StopLine();
	}
}
//

/*Setup device*/
void HTS221::SetupDevice()
{
	/*if not checcked check*/
	if(!flags.inited)
	{
		CheckDevice();
		return;
	}
	/*if already setupped return*/
	if(flags.setupped) return;
	
	/*Write configs (see datasheet)*/
	uint8_t send = _HTS221_AV_CONF_HUMIDITY_512 | _HTS221_AV_CONF_TEMPERATURE_256;
	i2c->WriteRegister(HTS_ADDR,HTS221_REG_AV_CONF,&send,1);
	
	uint8_t send2 = _HTS221_CTRL_REG1__1Hz | _HTS221_CTRL_REG1_ActiveMode | _HTS221_CTRL_REG1_BlockDataUpdate;
	i2c->WriteRegister(HTS_ADDR,HTS221_REG_CTRL_REG1,&send2,1);
	
	/*Check if configs are written*/
	i2c->ReadRegister(HTS_ADDR,HTS221_REG_AV_CONF,1);
	if(send != i2c->received[0]) return;
	i2c->ReadRegister(HTS_ADDR,HTS221_REG_CTRL_REG1,1);
	if(send2 != i2c->received[0]) return;
	
	
		
	/*Read all tweaking data from HTS221*/
	GetTempCalibs();
	GetHumidityCalibs();
	
	
	//i2c_hw.StopLine();
	if(flags.hum_calibs_get && flags.temp_calibs_get) flags.setupped = true;
}
//

/*Get Temperature Calibrations*/
void HTS221::GetTempCalibs()
{
	uint8_t reg_t0_l, reg_t1_l;
	
	flags.temp_calibs_get = false;
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_T0_degC_x8,1) != I2C_RECV_OK) return;
	reg_t0_l = i2c->received[0];
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_T1_degC_x8,1) != I2C_RECV_OK) return;
	reg_t1_l = i2c->received[0];
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_T1_T0_MSB,1) != I2C_RECV_OK) return;
	
	vars.T0_degC = static_cast<float>((i2c->received[0]&0x3)<<8 | reg_t0_l)/8.f;
	vars.T1_degC = static_cast<float>(((i2c->received[0]&0xC)>>2)<<8 | reg_t1_l)/8.f;
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_T0_OUT_LSB,2) != I2C_RECV_OK) return;
	vars.T0_OUT = static_cast<float>((i2c->received[1]<<8) | i2c->received[0]);
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_T1_OUT_LSB,2) != I2C_RECV_OK) return;
	vars.T1_OUT = static_cast<float>((i2c->received[1]<<8) | i2c->received[0]);
	
	
	
	flags.temp_calibs_get = true;
}
//

/*Get Humidity Calibrations*/
void HTS221::GetHumidityCalibs()
{
	
	flags.hum_calibs_get = false;
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_H0_rH_x2,1) != I2C_RECV_OK) return;
	vars.H0_rh = i2c->received[0]/2.0f;
		
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_H1_rH_x2,1) != I2C_RECV_OK) return;
	vars.H1_rh = i2c->received[0]/2.f;
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_H0_T0_OUT_LSB,2) != I2C_RECV_OK) return;
	//f(i2c->ReadRegister(HTS_ADDR,HTS221_REG_H0_T0_OUT_MSB,1) == I2C_RECV_OK) cals[1] = i2c->received[0];
	vars.H0_OUT = static_cast<float>((i2c->received[1]<<8) | i2c->received[0]);
	
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_H1_T0_OUT_LSB,2) != I2C_RECV_OK) return;
	//if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_H1_T0_OUT_MSB,1) == I2C_RECV_OK) cals[1] = i2c->received[0];
	vars.H1_OUT = static_cast<float>((i2c->received[1]<<8) | i2c->received[0]);
	
	flags.hum_calibs_get = true;

}
//

void HTS221::Process()
{
	//temp = 25.0;
	//hum = 20.0;
	//return;
	
	/*If not checked and not setupped - check and setup*/
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
	

	
	/*read status registers*/
	volatile uint8_t status;
	volatile uint8_t h;
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_STATUS,1) == I2C_RECV_OK) status = i2c->received[0];
	else return;
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_CTRL_REG1,1) == I2C_RECV_OK) h = i2c->received[0];
	else return;
	if(h & 2) flags.heating = true;
	
	if(flags.heating) SetHeater(false);
	
	/*Count temp and humidity*/
	if((status &1)) CountTemp();
	if(status &2) CountHum();
	//i2c_hw.StopLine();
}
//

/*Count Temperature*/
void HTS221::CountTemp()
{
	int16_t T_OUT = 0;
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_TEMP_OUT_L,1) != I2C_RECV_OK) return;
	T_OUT = (i2c->received[1]<<8) | i2c->received[0];

	/*Linear Interpolation. Used tweak values*/
	temp = ((vars.T1_degC - vars.T0_degC)*T_OUT + ((vars.T1_OUT*vars.T0_degC) - (vars.T0_OUT*vars.T1_degC)))/(vars.T1_OUT-vars.T0_OUT);
}
//

/*Count humidity*/
void HTS221::CountHum()
{
	
	int16_t H_OUT = 0;
	if(i2c->ReadRegister(HTS_ADDR,HTS221_REG_HUMIDITY_OUT_L,2) != I2C_RECV_OK) return;
	H_OUT |= (i2c->received[1]<<8) | i2c->received[0];
	
	/*Linear Interpolation. Used tweak values*/
	hum = ((vars.H1_rh - vars.H0_rh)*H_OUT + ((vars.H1_OUT*vars.H0_rh) - (vars.H0_OUT*vars.H1_rh)))/(vars.H1_OUT-vars.H0_OUT);
	
	if(hum<0) hum = 0;
	if(hum>100) hum = 100;
	
	/*Enable  for activating heater when humidity is over 70%*/
	if(hum>80) SetHeater(true);
}
//

/*Activate heater (for drying humidity sensor)*/
void HTS221::SetHeater(bool stat)
{
	uint8_t stt = stat?1:0;
	uint8_t send[1] = {stt};
	if(stat)
	{		
		if(i2c->WriteRegister(HTS_ADDR,HTS221_REG_CTRL_REG2,send,1) == I2C_WRITE_OK) flags.heating = true;
	}
	else if(i2c->WriteRegister(HTS_ADDR,HTS221_REG_CTRL_REG2,send,1) == I2C_WRITE_OK) flags.heating = false;
}
//

void HTS221::GetStatus(struct HtsFlags* flags)
{
	flags = &(this->flags);
}
