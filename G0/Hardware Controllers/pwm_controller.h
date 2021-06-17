#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include "defines.h"

#define PWM_FULL_PRESC 	100.0
#define PWM_INCREMENT  	2
#define PWM_MIN_SPEED	10

struct PWM_InitStruct
{
	GPIO_TypeDef*	gpio;
	uint32_t		pin;
	TIM_TypeDef*	tim;
	uint32_t		tim_ch;
	uint32_t		gpio_af;
	uint8_t 		period;
};

class PwmController
{
	public:
		void Init(PWM_InitStruct);
		void SetPercentage(double);
		void Stop();
		void SetRaw(double);
		void ProcessUpdate();
	
	private:
		void InitGPIO();
		void InitTIM();
		
		PWM_InitStruct pwm;
		double dma_percent[100];
		uint8_t ctr;
		double curr_percent;
};
//


#endif
