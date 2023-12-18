#ifndef FLASH_CONTROLLER_H
#define FLASH_CONTROLLER_H

#include "defines.h"

#ifndef FLASH_KEY1
#define FLASH_KEY1 					0x45670123
#endif
#ifndef FLASH_KEY2
#define FLASH_KEY2 					0xCDEF89AB
#endif


class FlashController
{
    public:
        /**
         * @brief Initialize FlashController Class
         * 
         */
        void Init();

        /**
         * @brief Set Pointer to flash memory
         * 
         * @param ptr pointer to memory (any aligning)
         */
        void SetFlashPointer(uint32_t ptr);

        /**
         * @brief Clear page
         * 
         * @param page_n Page number
         */
        void ClearPage(uint8_t page_n);

        /**
         * @brief Write data to falsh
         * 
         * @param ptr pointer to data to write
         * @param size size to write
         * @returns pointer to flash next to write
         */
        uint32_t* WriteToFlash(void* ptr, uint32_t size);

        /**
         * @brief Writes last aligned data to flash
         * 
         */
        void EndWriting();

    private:

        uint8_t block_to_write[8];
        uint8_t block_ptr;
        uint32_t v_addr;
        uint32_t alligned_addr;

};

#endif