#include "firmware_updater.h"

void Firmware_Update()
{
	unsigned long Addr, Data;
	unsigned short Sector, RetryNum;
		
	if (((FIRMWAREID*)(FIRMWARE_FLASH+FIRMWARE_ID_OFFSET))->ProductCode != PRODUCT_CODE) {
		// incorrect id - reset
		goto _end_flash;
	}
	
	FLASH->ACR= 0x0006;
	RetryNum= 5;
	
_Retry:
	RetryNum--;
	
	// unlock
	while ((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY);
	if (FLASH->CR & FLASH_CR_LOCK) {
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
	
	// erase 5 sectors (128kB)
	for (Sector=0;Sector<FIRMWARE_SECTOR;Sector++) {
		IWDG->KR = 0xAAAA;
		// FLASH->CR &= FLASH_CR_PSIZE_Msk;
		// FLASH->CR |= 2<<FLASH_CR_PSIZE_Pos;
		FLASH->CR &= 0xFFFFFF07;
		// FLASH->CR |= FLASH_CR_SER | (Sector<<3);
		FLASH->CR |= FLASH_CR_STRT;
		while ((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY);
		// FLASH->CR &= (~FLASH_CR_SER);
		FLASH->CR &= 0xFFFFFF07;
	}
	
	for (Addr=0;Addr<FIRMWARE_MAX_SIZE;Addr+=4) {
		if ((*(unsigned long*)Addr)!=0xFFFFFFFF && RetryNum>0) goto _Retry;
		
	}
	
	//GPIOB->MODER |= 1<<GPIO_MODER_MODE9_Pos;
	// copy 256k data
	for (Addr=0;Addr<FIRMWARE_MAX_SIZE;Addr+=4) {
		if ((Addr&0x00003FFF)==0) {
			GPIOB->ODR ^= (1<<8);
			IWDG->KR = 0xAAAA;
		}
		Data= *(unsigned long*)(FIRMWARE_FLASH+Addr);
		// FLASH->CR &= FLASH_CR_PSIZE_Msk;
		// FLASH->CR |= FLASH_CR_PSIZE_1;
		FLASH->CR |= FLASH_CR_PG;
		*(unsigned long*)Addr = Data;
		while ((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY);
		FLASH->CR &= (~FLASH_CR_PG);
	}
	
	_end_flash:
	
	// FLASH->CR &= ~FLASH_CR_SNB_Msk;
	// FLASH->CR |= (FIRMWARE_SECTOR<<FLASH_CR_SNB_Pos) | FLASH_CR_SER;
	FLASH->CR |= FLASH_CR_STRT;
	while(FLASH->SR & FLASH_SR_BSY) ;
	
	// reset
	SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |
		(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | SCB_AIRCR_SYSRESETREQ_Msk);
	
	while (1);
}
