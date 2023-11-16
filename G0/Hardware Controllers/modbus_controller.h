#ifndef MODBUS_CONTROLLER_H
#define MODBUS_CONTROLLER_H

#include "defines.h"

struct ModbusController_InitStruct
{
    GPIO_TypeDef*   uart_tx_gpio;
    GPIO_TypeDef*   uart_rx_gpio;
    GPIO_TypeDef*   etx_gpio;
    uint32_t        uart_tx_pin;
    uint32_t        uart_rx_pin;
    uint32_t        etx_pin;
    uint32_t        uart_tx_af;
    uint32_t        uart_rx_af;
    USART_TypeDef*  uart;
    uint32_t        speed;
};

class ModbusController
{
    public:
        void Init(ModbusController_InitStruct*);

    private:
        void InitGPIO(ModbusController_InitStruct*);
        void InitUART(ModbusController_InitStruct*);
        void SetTX(bool);

        uint8_t frame[256];

        struct
        {
            GPIO_TypeDef*       etx_gpio;
            uint32_t            etx_pin;
            USART_TypeDef*      uart;
        }mbus;
};

#endif
