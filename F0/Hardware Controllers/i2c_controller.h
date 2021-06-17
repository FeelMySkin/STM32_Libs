#ifndef I2C_CONTROLLER_H
#define I2C_CONTROLLER_H

#include "defines.h"

struct I2CController_TypeDef
{
	I2C_TypeDef*	i2c;
	TIM_TypeDef*	help_tim;
	GPIO_TypeDef*	scl_gpio;
	GPIO_TypeDef*	sda_gpio;
	uint32_t		scl_pin;
	uint32_t		sda_pin;
	uint32_t		i2c_af;
	uint32_t		timing;
};

enum I2C_RESULT
{
	TIMEOUT,
	RECV_OK,
	WRITE_OK
};

class I2CController
{
    public:
        I2CController();
        ~I2CController();
        void Init(I2CController_TypeDef);
		I2C_RESULT WriteBytes(uint8_t addr,uint8_t* bytes,uint8_t len);
		I2C_RESULT ReadBytes(uint8_t addr,uint8_t len);
		I2C_RESULT ReadRegister(uint8_t addr, uint8_t reg, uint8_t len);
		I2C_RESULT WriteRegister(uint8_t addr, uint8_t reg, uint8_t* bytes, uint8_t len, bool restart=false);
		void TimeoutHandler();
		void SoftReset();
		void GetAddressList();
		void Await(uint8_t msec);
	
		uint8_t received[10];
		uint8_t addr_list[16];

    private:
		void InitGPIO();
		void InitI2C();
		void InitTIM();
		void InitDMA();
	
		volatile bool timeout;
	
		I2CController_TypeDef i2c;

};


#endif
