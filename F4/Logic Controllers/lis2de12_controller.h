#ifndef LIS2DE_CONTROLLER_H
#define LIS2DE_CONTROLLER_H

#include "defines.h"
#include "lis2de12_commands.h"
#include "i2c_controller.h"

struct LIS2DE_InitStruct
{
	GPIO_TypeDef* nss_gpio;
	uint32_t nss_pin;
	I2CController* i2c;
	TIM_TypeDef* update_tim;
	uint8_t delta_t;
};

struct LIS2_INIT_STRUCT
{
	uint8_t acc_range:4;
	uint8_t acc_rate:4;
};

class LIS2DE
{
	public:
		void Init(LIS2DE_InitStruct);
		void Process();
		
		double roll, pitch;
	
	private:
		void InitGPIO();
		void InitTIM();
		void Setup();
		void GetID();
		void CountRate();
		void GetAccRange();
		void SetupRate();
		void WriteRegister(uint8_t reg, uint8_t data);
		void CountRPY();
		uint8_t ReadRegister(uint8_t reg);
	
		LIS2DE_InitStruct lis;
		LIS2_INIT_STRUCT inits;
		int8_t temp[3];
		double accel[3];
		uint8_t id;
		uint8_t need_rate;
		double delta_t_s;
		bool fully_setupped;
};





#endif
