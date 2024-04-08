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
        void SendData(uint8_t* data, uint16_t len);
        uint8_t* GetData(){flags.received = false; return frame;};
        uint16_t GetLen(){return frame_ptr;}
        bool IsReceived() {return flags.received;}
        bool IsSending() {return flags.sending;}
        bool IsReceiving() {return flags.receiving;}
        void ProcessUART();
        void ProcessTick();


    private:
        void InitGPIO(ModbusController_InitStruct*);
        void InitUART(ModbusController_InitStruct*);
        void SetTX(bool);

        uint8_t frame[256], awaiter;
        uint16_t frame_ptr;
        uint16_t to_send;

        struct  __attribute__((packed)){
            bool sending:1;
            bool receiving:1;
            bool received:1;
        }flags;

        struct
        {
            GPIO_TypeDef*       etx_gpio;
            uint32_t            etx_pin;
            USART_TypeDef*      uart;
        }mbus;
};

#endif
