#include "flash_controller.h"

void FlashController::Init()
{
    if(FLASH->CR &FLASH_CR_LOCK)
	{
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
    FLASH->CR |= FLASH_CR_PG;
    block_ptr = 0;
    for(uint8_t i = 0;i<8;++i) block_to_write[i] = 0xFF;
}

void FlashController::ClearPage(uint8_t page)
{
	FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR &= ~(FLASH_CR_PNB_Msk);

    FLASH->CR |= FLASH_CR_PER | (page<<FLASH_CR_PNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;
    __nop();
    while(FLASH->SR & FLASH_SR_BSY1) __nop();
	
	FLASH->CR &= ~FLASH_CR_PER;
	FLASH->CR |= FLASH_CR_PG;
}

void FlashController::SetFlashPointer(uint32_t ptr)
{
    v_addr = ptr;
    alligned_addr = (ptr/8)*8;
	block_ptr = v_addr%8;
}

void FlashController::EndWriting()
{
    memcpy((uint8_t*)alligned_addr,block_to_write,8);
    block_ptr = 0;
    alligned_addr+=8;
    v_addr = alligned_addr;
    
}

uint32_t* FlashController::WriteToFlash(void* ptr, uint32_t sz)
{
    uint8_t *in_ptr = (uint8_t*)ptr;
    if(v_addr != alligned_addr)
    {
        for(int i = block_ptr; i<8 && sz;++i)
        {
            block_to_write[i] = *in_ptr;
            block_ptr++;
            in_ptr++;
            sz--;
            v_addr++;
        }
        if(block_ptr == 8)
        {
            memcpy((uint8_t*)alligned_addr,block_to_write,8);
            __nop();
            while(FLASH->SR &= FLASH_SR_BSY1) __nop();
            block_ptr = 0;
        }
        alligned_addr = (v_addr/8)*8;
    }

    while(sz)
    {
        if(sz>=8)
        {
            memcpy((uint8_t*)alligned_addr,in_ptr,8);
            __nop();
            while(FLASH->SR &= FLASH_SR_BSY1) __nop();
            alligned_addr+=8;
            v_addr+=8;
            in_ptr+=8;
            sz-=8;
            block_ptr = 0;
        }
        else
        {
            for(int i = 0;i<8;++i) block_to_write[i] = i<sz?((uint8_t*)in_ptr)[i]:0xFF;
            v_addr+=sz;
            block_ptr+=sz;
            sz = 0;
			alligned_addr = (v_addr/8)*8;
        }

    }
    return (uint32_t*)v_addr;
    
}