#include "modbus_controller.h"

void ModbusController::Init(ModbusController_InitStruct* ini)
{
    this->mbus.etx_gpio = ini->etx_gpio;
    this->mbus.etx_pin = ini->etx_pin;
    this->mbus.uart = ini->uart;
    flags.received = false;
    flags.receiving = false;
    flags.sending = false;
    frame_ptr = 0;
	InitGPIO(ini);
	InitUART(ini);
}

void ModbusController::InitGPIO(ModbusController_InitStruct* mbus)
{
    LL_GPIO_InitTypeDef gpio;

    gpio.Alternate = mbus->uart_tx_af;
    gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pin = mbus->uart_tx_pin;
    gpio.Pull = LL_GPIO_PULL_NO;
    gpio.Speed = LL_GPIO_SPEED_FAST;
    LL_GPIO_Init(mbus->uart_tx_gpio,&gpio);

    gpio.Pin = mbus->uart_rx_pin;
    gpio.Alternate = mbus->uart_rx_af;
    LL_GPIO_Init(mbus->uart_rx_gpio,&gpio);

    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.Pin = mbus->etx_pin;
    LL_GPIO_Init(mbus->etx_gpio,&gpio);
    SetTX(false);
}

void ModbusController::InitUART(ModbusController_InitStruct* mbus)
{
    LL_USART_InitTypeDef usart;
    usart.BaudRate = mbus->speed;
    usart.DataWidth = LL_USART_DATAWIDTH_8B;
    usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart.OverSampling = LL_USART_OVERSAMPLING_8;
    usart.Parity = LL_USART_PARITY_NONE;
    usart.PrescalerValue = LL_USART_PRESCALER_DIV1;
    usart.StopBits = LL_USART_STOPBITS_1;
    usart.TransferDirection = LL_USART_DIRECTION_TX_RX;
    LL_USART_Init(mbus->uart,&usart);

    LL_USART_EnableIT_RXNE(mbus->uart);

    EnableUsartIRQn(mbus->uart, 1);
	LL_USART_Enable(mbus->uart);

}

void ModbusController::SendData(uint8_t* data, uint16_t len)
{
    if(flags.receiving) return;
    SetTX(true);
    for(int i = 0;i<len;++i) frame[i] = data[i];
    to_send = len;
    frame_ptr = 0;
    flags.sending = true;
    LL_USART_TransmitData8(mbus.uart,frame[0]);
	LL_USART_EnableIT_TC(mbus.uart);
    frame_ptr++;
}

void ModbusController::ProcessUART()
{
    if(LL_USART_IsActiveFlag_RXNE(mbus.uart))
    {
		
        uint8_t data = LL_USART_ReceiveData8(mbus.uart);
        if(flags.receiving)
        {
            awaiter = 0;
            frame[frame_ptr] = data;
            frame_ptr++;
        }
        else if(flags.sending)
        {

        }
        else
        {
            flags.received = false;
            frame_ptr = 1;
            frame[0] = data;
            flags.receiving = true;
            awaiter = 0;
        }
    }
    else if(LL_USART_IsActiveFlag_TC(mbus.uart))
    {
		LL_USART_ClearFlag_TC(mbus.uart);
        if(flags.sending)
        {
            if(frame_ptr == to_send)
            {
                SetTX(false);
                flags.sending = false;
				LL_USART_DisableIT_TC(mbus.uart);
            }
            else
            {
                LL_USART_TransmitData8(mbus.uart,frame[frame_ptr]);
                frame_ptr++;
            }
        }
    }
	else
	{
		LL_USART_Disable(mbus.uart);
		LL_USART_Enable(mbus.uart);
	}
    
}

void ModbusController::ProcessTick()
{
    if(flags.receiving)
    {
        awaiter++;
        if(awaiter>=5)
        {
            flags.receiving = false;
            if(frame_ptr <5) frame_ptr = 0;
            else flags.received = true;
        }
    }
}

void ModbusController::SetTX(bool tx)
{
    if(tx) LL_GPIO_SetOutputPin(mbus.etx_gpio, mbus.etx_pin);
    else LL_GPIO_ResetOutputPin(mbus.etx_gpio, mbus.etx_pin);
}