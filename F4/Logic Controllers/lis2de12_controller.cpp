#include "lis2de12_controller.h"

void LIS2DE::Init(LIS2DE_InitStruct lis)
{
	this->lis = lis;
	delta_t_s = lis.delta_t/1000.0;
	//InitGPIO();
	InitTIM();
	CountRate();
	Setup();
}
//

void LIS2DE::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = lis.nss_pin;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(lis.nss_gpio,&gpio);
	LL_GPIO_SetOutputPin(lis.nss_gpio,lis.nss_pin);
}
//

void LIS2DE::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = lis.delta_t;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERMODE_UP;
	tim.Prescaler = SystemCoreClock/1000-1;
	tim.RepetitionCounter = 0;
	LL_TIM_Init(lis.update_tim,&tim);
	
	LL_TIM_EnableIT_UPDATE(lis.update_tim);
	LL_TIM_ClearFlag_UPDATE(lis.update_tim);
	EnableTimIRQn(lis.update_tim,4);
	LL_TIM_EnableCounter(lis.update_tim);
}
//

void LIS2DE::CountRate()
{
	double freq = 1.0/delta_t_s;
	if(freq<=1) need_rate = LIS_CTRL_REG1_ODR_1HZ_Msk;
	else if(freq>1 && freq<=10) need_rate = LIS_CTRL_REG1_ODR_10HZ_Msk;
	else if(freq>10 && freq<=25) need_rate = LIS_CTRL_REG1_ODR_25HZ_Msk;
	else if(freq>25 && freq<=50) need_rate = LIS_CTRL_REG1_ODR_50HZ_Msk;
	else if(freq>50 && freq<=100) need_rate = LIS_CTRL_REG1_ODR_100HZ_Msk;
	else if(freq>100 && freq<=200) need_rate = LIS_CTRL_REG1_ODR_200HZ_Msk;
	else if(freq>200 && freq<=400) need_rate = LIS_CTRL_REG1_ODR_400HZ_Msk;
	else if(freq>400 && freq<=1620) need_rate = LIS_CTRL_REG1_ODR_1_62kHZ_Msk;
	else if(freq>1620) need_rate = LIS_CTRL_REG1_ODR_5_376kHZ_Msk;
	
}
//

void LIS2DE::Process()
{
	if(!fully_setupped)
	{	
		Setup();
		return;
	}
	temp[0] = ReadRegister(LIS_OUT_X_H);
	temp[1] = ReadRegister(LIS_OUT_Y_H);
	temp[2] = ReadRegister(LIS_OUT_Z_H);
	
	for(int i = 0;i<3;++i) accel[i] = (inits.acc_range * (temp[i]<<8) / FLOAT_MAX_16BIT);
	
	CountRPY();
	
}
//

void LIS2DE::GetAccRange()
{
	uint8_t data = 0 ;
    uint8_t range = 0 ;
    data = ReadRegister(LIS_CTRL_REG4);
    switch((data>>LIS_CTRL_REG4_FS_Pos) & 3) {
    case LIS_CTRL_REG4_FS_2g_Msk: /* +/- 2g */
        range = 2 ; 
        break ;
    case LIS_CTRL_REG4_FS_4g_Msk: /* +/- 4g */
        range = 4 ;
        break ;
    case LIS_CTRL_REG4_FS_8g_Msk: /* +/- 8g */
        range = 8 ;
        break ;
    case LIS_CTRL_REG4_FS_16g_Msk: /* +/- 16g */
        range = 16 ;
        break ;
    default:
        break ;
    }
    inits.acc_range = range ;
}
//

void LIS2DE::SetupRate()
{
	WriteRegister(LIS_CTRL_REG1,need_rate | LIS_CTRL_REG1_LPen_Msk | LIS_CTRL_REG1_X_En_Msk | LIS_CTRL_REG1_Y_En_Msk | LIS_CTRL_REG1_Z_En_Msk);
}
//

void LIS2DE::Setup()
{
	
	if(id != 0x33)
	{	
		GetID();
		return;
	}
	if(inits.acc_range == 0) GetAccRange();
	SetupRate();
	
	/*GetStatus();
	if(stat.acc_pmu_Status == 0)
	{	
		WriteRegister(REG_CMD,0x10 | ACC_PMU_NORMAL);
		return;
	}
	else if(stat.gyr_pmu_status == 0)
	{	
		WriteRegister(REG_CMD,0x14 | GYR_PMU_NORMAL);
		return;
	}
	else
	{	
		WriteRegister(REG_CMD,0x03);
		fully_setupped = true;
	}*/
	fully_setupped = true;
}
//

void LIS2DE::GetID()
{
	id = ReadRegister(LIS_WHO_AM_I);
}
//


void LIS2DE::WriteRegister(uint8_t reg, uint8_t data)
{
	uint8_t ret[2] = {reg,data};
	lis.i2c->WriteBytes(LIS2DE_ADDRESS,ret,2);
}
//

uint8_t LIS2DE::ReadRegister(uint8_t reg)
{
	uint8_t ret;
	lis.i2c->WriteBytes(LIS2DE_ADDRESS,&reg,1);
	lis.i2c->ReadBytes(LIS2DE_ADDRESS,1);
	return lis.i2c->received[0];
}
//

void LIS2DE::CountRPY()
{
	//pitch = atan2(accel[1],sqrt(accel[0]*accel[0] + accel[2]*accel[2]));
	//roll = atan2(-accel[0],sqrt(accel[1]*accel[1] + accel[2]*accel[2]));
	pitch = 180*atan(accel[0]/sqrt(accel[1]*accel[1] + accel[2]*accel[2]))/3.14;
	roll = 180*atan(accel[1]/sqrt(accel[0]*accel[0] + accel[2]*accel[2]))/3.14;
	//yaw = 180*atan(accel[2]/sqrt(accel[0]*accel[0] + accel[2]*accel[2]))/3.14;
}
//