#include "pwm_controller.h"

void PwmController::Init(PWM_InitStruct str)
{
	this->pwm = str;
	InitGPIO();
	InitTIM();
	SetRaw(0);
}
//

void PwmController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Alternate = pwm.gpio_af;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = pwm.pin;
	gpio.Pull = LL_GPIO_PULL_NO;
	gpio.Speed = LL_GPIO_SPEED_FAST;
	LL_GPIO_Init(pwm.gpio,&gpio);
}
//

void PwmController::InitTIM()
{
	LL_TIM_InitTypeDef tim;
	tim.Autoreload = pwm.period*PWM_FULL_PRESC;
	tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	tim.CounterMode = LL_TIM_COUNTERDIRECTION_UP;
	tim.Prescaler = SystemCoreClock/(1000*PWM_FULL_PRESC) - 1;
	tim.RepetitionCounter = 0;
	LL_TIM_Init(pwm.tim,&tim);
	
	LL_TIM_OC_InitTypeDef oc;
	oc.CompareValue = 0;
	oc.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	oc.OCMode = LL_TIM_OCMODE_PWM1;
	oc.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	oc.OCState = LL_TIM_OCSTATE_ENABLE;
	LL_TIM_OC_Init(pwm.tim,pwm.tim_ch,&oc);
	
	LL_TIM_EnableIT_UPDATE(pwm.tim);
	LL_TIM_ClearFlag_UPDATE(pwm.tim);
	EnableTimIRQn(pwm.tim,2);
	
	LL_TIM_EnableAllOutputs(pwm.tim);
	LL_TIM_EnableCounter(pwm.tim);
}
//

void PwmController::SetPercentage(double perc)
{
	//SetTimChannel(pwm.tim,pwm.tim_ch,pwm.period*PWM_FULL_PRESC*perc/100.0);
	if(perc != curr_percent && ctr == 0)
	{
		int16_t sign= perc-curr_percent>0?1:-1;
		if(perc-curr_percent == 0) return;
		
		uint8_t buf_ctr = 0;
		for(int i = 0;i<100;++i)
		{
			if(i == 0) dma_percent[i] = curr_percent+(i+1)*sign*PWM_INCREMENT;
			else dma_percent[i] = dma_percent[i-1]+sign*PWM_INCREMENT;
			
			if(curr_percent == 0) dma_percent[0] = PWM_MIN_SPEED;
			buf_ctr++;
			if(dma_percent[i] == perc) break;
		}
		for(int i = 0;i<buf_ctr/2;++i)
		{
			double buf = dma_percent[i];
			dma_percent[i] = dma_percent[buf_ctr-1-i];
			dma_percent[buf_ctr-1-i] = buf;
		}
		ctr = buf_ctr;
	}
}
//

void PwmController::SetRaw(double raw)
{
	uint32_t get_full = raw*PWM_FULL_PRESC;
	SetTimChannel(pwm.tim,pwm.tim_ch,get_full);
}
//

void PwmController::ProcessUpdate()
{
	if(ctr !=0)
	{
		SetTimChannel(pwm.tim,pwm.tim_ch,pwm.period*PWM_FULL_PRESC*dma_percent[ctr-1]/100.0);
		curr_percent = dma_percent[ctr-1];
		ctr--;
	}
}
//

void PwmController::Stop()
{
	SetTimChannel(pwm.tim,pwm.tim_ch,0);
	curr_percent = 0;
	ctr = 0;
}
//

