#ifndef HTS221_H
#define HTS221_H

#include "defines.h"
#include "i2c_controller.h"


#define HTS_ADDR 0x5F

/*HTS221 Registers*/
#define HTS221_REG_WHO_AM_I			0x0F
#define HTS221_REG_AV_CONF			0x10
#define HTS221_REG_CTRL_REG1		0x20
#define HTS221_REG_CTRL_REG2		0x21
#define HTS221_REG_CTRL_REG3		0x22
#define HTS221_REG_STATUS			0x27
#define HTS221_REG_HUMIDITY_OUT_L	0x28
#define HTS221_REG_HUMIDITY_OUT_H	0x29
#define HTS221_REG_TEMP_OUT_L		0x2A
#define HTS221_REG_TEMP_OUT_H		0x2B
#define HTS221_REG_H0_rH_x2			0x30
#define HTS221_REG_H1_rH_x2			0x31
#define HTS221_REG_T0_degC_x8		0x32
#define HTS221_REG_T1_degC_x8		0x33
#define HTS221_REG_T1_T0_MSB		0x35
#define HTS221_REG_H0_T0_OUT_LSB	0x36
#define HTS221_REG_H0_T0_OUT_MSB	0x37
#define HTS221_REG_H1_T0_OUT_LSB	0x3A
#define HTS221_REG_H1_T0_OUT_MSB	0x3B
#define HTS221_REG_T0_OUT_LSB		0x3C
#define HTS221_REG_T0_OUT_MSB		0x3D
#define HTS221_REG_T1_OUT_LSB		0x3E
#define HTS221_REG_T1_OUT_MSB		0x3F

/********************Register Bits********************/

//Register AV_CONF
#define __HTS221_AV_CONF_AVGH0				0
#define __HTS221_AV_CONF_AVGH1				1
#define __HTS221_AV_CONF_AVGH2				2
#define __HTS221_AV_CONF_AVGT0				3
#define __HTS221_AV_CONF_AVGT1				4
#define __HTS221_AV_CONF_AVGT2				6
#define _HTS221_AV_CONF_HUMIDITY_4			0x00
#define _HTS221_AV_CONF_HUMIDITY_8			(1 << __HTS221_AV_CONF_AVGH0)
#define _HTS221_AV_CONF_HUMIDITY_16			(1 << __HTS221_AV_CONF_AVGH1)
#define _HTS221_AV_CONF_HUMIDITY_32			(1 << __HTS221_AV_CONF_AVGH0) | (1 << __HTS221_AV_CONF_AVGH1)
#define _HTS221_AV_CONF_HUMIDITY_64			(1 << __HTS221_AV_CONF_AVGH2)
#define _HTS221_AV_CONF_HUMIDITY_128		(1 << __HTS221_AV_CONF_AVGH0) | (1 << __HTS221_AV_CONF_AVGH2)
#define _HTS221_AV_CONF_HUMIDITY_256		(1 << __HTS221_AV_CONF_AVGH1) | (1 << __HTS221_AV_CONF_AVGH2)
#define _HTS221_AV_CONF_HUMIDITY_512		(1 << __HTS221_AV_CONF_AVGH0) | (1 << __HTS221_AV_CONF_AVGH1) | (1 << __HTS221_AV_CONF_AVGH2)
#define _HTS221_AV_CONF_TEMPERATURE_2		0x00
#define _HTS221_AV_CONF_TEMPERATURE_4		(1 << __HTS221_AV_CONF_AVGT0)
#define _HTS221_AV_CONF_TEMPERATURE_8		(1 << __HTS221_AV_CONF_AVGT1)
#define _HTS221_AV_CONF_TEMPERATURE_16		(1 << __HTS221_AV_CONF_AVGT0) | (1 << __HTS221_AV_CONF_AVGT1)
#define _HTS221_AV_CONF_TEMPERATURE_32		(1 << __HTS221_AV_CONF_AVGT2)
#define _HTS221_AV_CONF_TEMPERATURE_64		(1 << __HTS221_AV_CONF_AVGT0) | (1 << __HTS221_AV_CONF_AVGT2)
#define _HTS221_AV_CONF_TEMPERATURE_128		(1 << __HTS221_AV_CONF_AVGT1) | (1 << __HTS221_AV_CONF_AVGT2)
#define _HTS221_AV_CONF_TEMPERATURE_256		(1 << __HTS221_AV_CONF_AVGT0) | (1 << __HTS221_AV_CONF_AVGT1) | (1 << __HTS221_AV_CONF_AVGT2)

//Register CTRL_REG1
#define __HTS221_CTRL_REG1_ODR0				0	
#define __HTS221_CTRL_REG1_ODR1				1	
#define __HTS221_CTRL_REG1_BDU				2	
#define __HTS221_CTRL_REG1_PD				7
#define _HTS221_CTRL_REG1__One				0x00
#define _HTS221_CTRL_REG1__1Hz				(1 << __HTS221_CTRL_REG1_ODR0)
#define _HTS221_CTRL_REG1__7Hz				(1 << __HTS221_CTRL_REG1_ODR1)
#define _HTS221_CTRL_REG1__12Hz				(1 << __HTS221_CTRL_REG1_ODR0) | (1 << __HTS221_CTRL_REG1_ODR1)
#define _HTS221_CTRL_REG1_PowerDownMode		(0 << __HTS221_CTRL_REG1_PD)
#define _HTS221_CTRL_REG1_ActiveMode		(1 << __HTS221_CTRL_REG1_PD)
#define _HTS221_CTRL_REG1_ContinuousUpdate	(0 << __HTS221_CTRL_REG1_BDU)
#define _HTS221_CTRL_REG1_BlockDataUpdate	(1 << __HTS221_CTRL_REG1_BDU)

//Register CTRL_REG2
#define __HTS221_CTRL_REG2_OSE				0
#define __HTS221_CTRL_REG2_HEATER			1
#define __HTS221_CTRL_REG2_BOOT				7
#define _HTS221_CTRL_REG2_StartNewDataset	(1 << __HTS221_CTRL_REG2_OSE)
#define _HTS221_CTRL_REG2_HeaterEnable		(1 << __HTS221_CTRL_REG2_HEATER)
#define _HTS221_CTRL_REG2_HeaterDisable		(0 << __HTS221_CTRL_REG2_HEATER)

//Register CTRL_REG3
#define __HTS221_CTRL_REG3_DRDY				2
#define __HTS221_CTRL_REG3_PP_OD			6
#define __HTS221_CTRL_REG3_DRDY_H_L			7
#define _HTS221_CTRL_REG3_DataReadyEn		(1 << __HTS221_CTRL_REG3_DRDY)
#define _HTS221_CTRL_REG3_DataReadyDis		(0 << __HTS221_CTRL_REG3_DRDY)
#define _HTS221_CTRL_REG3_PIN3_PushPull		(0 << __HTS221_CTRL_REG3_PP_OD)
#define _HTS221_CTRL_REG3_PIN3_OpenDrain	(1 << __HTS221_CTRL_REG3_PP_OD)
#define _HTS221_CTRL_REG3_PIN3_ActiveHigh	(0 << __HTS221_CTRL_REG3_DRDY_H_L)
#define _HTS221_CTRL_REG3_PIN3_ActiveLow	(1 << __HTS221_CTRL_REG3_DRDY_H_L)

//Register STATUS_REG
#define _HTS221_STATUS_REG_T_DA				0
#define _HTS221_STATUS_REG_H_DA				1

struct HtsFlags
{
	bool temp_calibs_get:1;
	bool hum_calibs_get:1;
	bool inited:1;
	bool setupped:1;
	bool heating:1;
};

struct HtsVariables
{
	float T0_degC;
	float T1_degC;
	float H0_rh;
	float H1_rh;
	float T0_OUT;
	float T1_OUT;
	float H0_OUT;
	float H1_OUT;
	uint8_t conf,ctrl;
};


class HTS221
{
	public:
		void Init(I2CController*);
		void Process();
		void GetStatus(struct HtsFlags* flags);

		double temp,hum;
	
	private:
		void CheckDevice();
		void SetupDevice();
		void CountTemp();
		void CountHum();
		void SetHeater(bool);
	
		void GetTempCalibs();
		void GetHumidityCalibs();
	
		HtsFlags flags;
		HtsVariables vars;
		I2CController* i2c;
};

#endif
