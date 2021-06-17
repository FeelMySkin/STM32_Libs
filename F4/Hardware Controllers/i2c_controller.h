#ifndef I2C_CONTROLLER_H
#define I2C_CONTROLLER_H

#include "defines.h"

struct I2CController_TypeDef
{
	I2C_TypeDef*	I2C;
	GPIO_TypeDef*	SCL_GPIO;
	GPIO_TypeDef*	SDA_GPIO;
	uint32_t		SCL_PIN;
	uint32_t		SDA_PIN;
	uint32_t		I2C_SDA_AF;
	uint32_t		I2C_SCL_AF;
	uint32_t		TIMING;
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
		void SetTimeout(uint8_t msec=100);
		void HardReset();
	
		volatile bool timeout;
		volatile uint32_t timer;
	
		I2CController_TypeDef i2c;

};


#endif
