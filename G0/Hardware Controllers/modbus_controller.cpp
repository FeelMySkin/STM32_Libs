#include "modbus_controller.h"

void ModbusController::Init(ModbusController_InitStruct* ini)
{
    this->mbus.etx_gpio = ini->etx_gpio;
    this->mbus.etx_pin = ini->etx_pin;
    this->mbus.uart = ini->uart;
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

}

void ModbusController::SetTX(bool tx)
{
    if(tx) LL_GPIO_ResetOutputPin(mbus.etx_gpio, mbus.etx_pin);
    else LL_GPIO_SetOutputPin(mbus.etx_gpio, mbus.etx_pin);
}