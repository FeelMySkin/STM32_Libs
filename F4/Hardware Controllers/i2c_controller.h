#ifndef I2C_CONTROLLER_H
#define I2C_CONTROLLER_H

#include "defines.h"

#define I2C_DEFAULT_AWAITER 50
#define I2C_DEFAULT_PAUSE	2

struct I2CController_TypeDef
{
	I2C_TypeDef*	i2c;
	TIM_TypeDef*	help_tim;
	GPIO_TypeDef*	scl_gpio;
	GPIO_TypeDef*	sda_gpio;
	uint32_t		scl_pin;
	uint32_t		sda_pin;
	uint32_t		i2c_sda_af;
	uint32_t		i2c_scl_af;
	uint32_t		speed;
};

enum I2C_RESULT
{
	I2C_TIMEOUT,
	I2C_RECV_OK,
	I2C_WRITE_OK
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
		I2C_RESULT WriteRegister(uint8_t addr, uint8_t reg, uint8_t* bytes, uint8_t len);
		I2C_RESULT TransceiveBytes(uint8_t addr,uint8_t* s_bytes, uint8_t s_len, uint8_t r_len);
		void TimeoutHandler();
		void SoftReset();
		void GetAddressList();
		void Await(uint8_t msec);
		bool IsOnline(uint8_t addr);
	
		uint8_t received[128];
		uint8_t addr_list[16];

    private:
		void InitGPIO();
		void InitI2C();
		void InitDMA();
		void InitTIM();
		void HardReset();
		void SetTimeout();
	
		volatile bool timeout;
		volatile uint32_t timer;
	
		I2CController_TypeDef i2c;

};


#endif
