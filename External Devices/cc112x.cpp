#include "cc112x.h"

CC112xController::CC112xController()
{
	cal_freq = 0xFFFFFFFF;
}

void CC112xController::Init(CC112X_InitTypeDef ini)
{
	this->cc112x = ini;
	InitGPIO();
	uint8_t writeByte, read_bytes;

	uint8_t i;

	cc112x.mode &= 0xC0;

	online = false;

	Strobe(CC112X_SRES);
	//TODO: if (!TimeoutTim) return; //Transceiver does not respond

	online = true;

	if (cal_freq == 0xFFFFFFFF) {
		//Calibration
		for (i = 0; i < (sizeof  Calibration/sizeof(registerSetting_t)); i++) {
			writeByte =  Calibration[i].data; 
			WriteRegister( Calibration[i].addr, &writeByte, 1);
			ReadRegister(Calibration[i].addr,&read_bytes, 1);
		}
	}
	else if (!(cc112x.mode & 0x40)) {
		//Main 868 or 915
		for (i = 0; i < (sizeof  MainRadio868/sizeof(registerSetting_t)); i++) {
		writeByte =  MainRadio868[i].data; 
		WriteRegister( MainRadio868[i].addr, &writeByte, 1);
			ReadRegister(MainRadio868[i].addr,&read_bytes, 1);
		}
	}
	else {
		//Fast 868 or 915
		for (i = 0; i < (sizeof  FastRadio868/sizeof(registerSetting_t)); i++) {
		writeByte =  FastRadio868[i].data; 
		WriteRegister( FastRadio868[i].addr, &writeByte, 1);
			ReadRegister(FastRadio868[i].addr,&read_bytes, 1);
		}
	}
	
	Errata();
}

void CC112xController::InitGPIO()
{
	LL_GPIO_InitTypeDef gpio;
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = cc112x.cs_pin;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(cc112x.cs_gpio,&gpio);
	LL_GPIO_SetOutputPin(cc112x.cs_gpio,cc112x.cs_pin);

	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pin = cc112x.res_pin;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(cc112x.res_gpio,&gpio);
	LL_GPIO_SetOutputPin(cc112x.res_gpio,cc112x.res_pin);
}

void CC112xController::SetFrequency(uint32_t Frequency, bool NeedErrata)
{
  Frequency = __REV(Frequency); 
  WriteRegister(CC112X_FREQ2, &(((uint8_t*)&Frequency)[1]), 3);
  TODO: if (NeedErrata) Errata(); // necessary if frequency jump is high
}

uint8_t CC112xController::ReadRegister(uint16_t reg, uint8_t* pData, uint8_t len)
{
	uint8_t tempExt  = (uint8_t)(reg >> 8);
	uint8_t tempAddr = (uint8_t)(reg & 0x00FF);
	uint8_t rc;

	/* Checking if this is a FIFO access -> returns chip not ready  */
	if ((CC112X_SINGLE_TXFIFO <= tempAddr) && (tempExt == 0)) return STATUS_CHIP_RDYn_BM;

	/* Decide what register space is accessed */
	if (!tempExt) {
	rc = RegAccess((RADIO_BURST_ACCESS|RADIO_READ_ACCESS),tempAddr,pData,len);
	}
	else if (tempExt == 0x2F) {
	rc = RegAccess((RADIO_BURST_ACCESS|RADIO_READ_ACCESS),tempExt,tempAddr,pData,len);
	}
	return (rc);
}

uint8_t CC112xController::WriteRegister(uint16_t reg, uint8_t* data, uint8_t len)
{
	 uint8_t tempExt  = (uint8_t)(reg>>8);
	uint8_t tempAddr = (uint8_t)(reg & 0x00FF);
	uint8_t rc;

	/* Checking if this is a FIFO access - returns chip not ready */
	if ((CC112X_SINGLE_TXFIFO <= tempAddr) && (tempExt == 0)) return STATUS_CHIP_RDYn_BM;

	/* Decide what register space is accessed */  
	if (!tempExt) {
	rc = RegAccess((RADIO_BURST_ACCESS|RADIO_WRITE_ACCESS),tempAddr,data,len);
	}
	else if (tempExt == 0x2F) {
	rc = RegAccess((RADIO_BURST_ACCESS|RADIO_WRITE_ACCESS),tempExt,tempAddr,data,len);
	}
	return (rc);
}

uint8_t CC112xController::RegAccess(uint8_t accessType, uint8_t addrByte, uint8_t *pData, uint16_t len)
{
	uint8_t readValue;

	/* Pull CS_N low and wait for SO to go low before communication starts */
	LL_GPIO_ResetOutputPin(cc112x.cs_gpio, cc112x.cs_pin);
	//TODO: if (!TimeoutTim) return TRANSCEIVER_FAIL;

	/* send register address byte and store status*/
	readValue = cc112x.spi->Transmit(accessType|addrByte);
	
	ReadWriteBurstSingle(accessType|addrByte,pData,len);
	LL_GPIO_SetOutputPin(cc112x.cs_gpio, cc112x.cs_pin);
	/* return the status byte value */
	return (readValue);
}

uint8_t CC112xController::RegAccess(uint8_t accessType, uint8_t extAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
	uint8_t readValue;

	LL_GPIO_ResetOutputPin(cc112x.cs_gpio, cc112x.cs_pin);
	//TODO: if (!TimeoutTim) return TRANSCEIVER_FAIL;

	/* send extended address byte with access type bits set and store status */
	readValue = cc112x.spi->Transmit(accessType|extAddr);
	cc112x.spi->Transmit(regAddr);

	/* Communicate len number of bytes */
	ReadWriteBurstSingle(accessType|extAddr,pData,len);
	LL_GPIO_SetOutputPin(cc112x.cs_gpio, cc112x.cs_pin);

	/* return the status byte value */
	return (readValue);
}

uint8_t CC112xController::Strobe(uint8_t comm)
{
	uint8_t rc;
	LL_GPIO_ResetOutputPin(cc112x.cs_gpio, cc112x.cs_pin);
	//TODO:if (!TimeoutTim) return TRANSCEIVER_FAIL;
	rc = cc112x.spi->Transmit(comm);
	LL_GPIO_SetOutputPin(cc112x.cs_gpio, cc112x.cs_pin);
	return (rc);
}

uint8_t CC112xController::CheckState(uint8_t state)
{
  uint8_t marcstate=0;

  //TODO:?os_dly_wait(10); //"Cooldown" transceiver, necessary!
  ReadRegister(CC112X_MARCSTATE, &marcstate, 1);
  if (state <= MARCSTATE_MASK) {
    if ((marcstate & MARCSTATE_MASK) == state) return 1;
  }
  else {
    if ((marcstate & STATE2PIN_MASK) == state) return 1;
  }
  return 0;
}


uint8_t CC112xController::ReadFIFO(uint8_t* pData, uint8_t len)
{
	return RegAccess(0x00,CC112X_BURST_RXFIFO, pData, len);
}

uint8_t CC112xController::WriteFIFO(uint8_t* pData, uint8_t len)
{
	return RegAccess(0x00,CC112X_BURST_TXFIFO, pData, len);
}

//TODO: maybe not required, return it to read and write reg?
void CC112xController::ReadWriteBurstSingle(uint8_t addr,uint8_t *pData,uint16_t len)
{
  uint16_t i;
  /* Communicate len number of bytes: if RX - the procedure sends 0x00 to push bytes from slave*/
  if (addr&RADIO_READ_ACCESS) {
  
    if (addr&RADIO_BURST_ACCESS) {
      for (i = 0; i < len; i++) {
		 *pData =cc112x.spi->Transmit(0);
          pData++;
      }
    }
    else {
      *pData = cc112x.spi->Transmit(0);
    }
    
  }
  else {
  
    if (addr&RADIO_BURST_ACCESS) {
      /* Communicate len number of bytes: if TX - the procedure doesn't overwrite pData */
      for (i = 0; i < len; i++) {
        cc112x.spi->Transmit(*pData);
        pData++;
      }
    }
    else {
      cc112x.spi->Transmit(*pData);
    }
    
  }
  return;
}

void CC112xController::Errata()
{
	uint8_t original_fs_cal2;
	uint8_t calResults_for_vcdac_start_high[3];
	uint8_t calResults_for_vcdac_start_mid[3];
	uint8_t writeByte;
	// 1) Set VCO cap- array to 0 (FS_VCO2 = 0x00)
	writeByte = 0x00;
	WriteRegister(CC112X_FS_VCO2, &writeByte, 1);
	// 2) Start with high VCDAC (original VCDAC_START + 2):
	ReadRegister(CC112X_FS_CAL2, &original_fs_cal2, 1);
	writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
	WriteRegister(CC112X_FS_CAL2, &writeByte, 1);
	// 3) Calibrate and wait for calibration to be done (radio back in IDLE state)
	Strobe(CC112X_SCAL);
	while(!CheckState(STATE2PIN_IDLE));
	// 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with high VCDAC_START value
	ReadRegister(CC112X_FS_VCO2, &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
	ReadRegister(CC112X_FS_VCO4, &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
	ReadRegister(CC112X_FS_CHP, &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);
	// 5) Set VCO cap- array to 0 (FS_VCO2 = 0x00)
	writeByte = 0x00 ;
	WriteRegister(CC112X_FS_VCO2, &writeByte, 1);
	// 6) Continue with mid VCDAC (original VCDAC_START):
	writeByte = original_fs_cal2;
	WriteRegister(CC112X_FS_CAL2, &writeByte, 1);
	// 7) Calibrate and wait for calibration to be done (radio back in IDLE state)
	Strobe(CC112X_SCAL);
	while(!CheckState(STATE2PIN_IDLE));
	// 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with mid VCDAC_START value
	ReadRegister(CC112X_FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
	ReadRegister(CC112X_FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
	ReadRegister(CC112X_FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);
	// 9) Write back highest FS_VCO2 and corresponding FS_VCO and FS_CHP result
	if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] > calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
	writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
	WriteRegister(CC112X_FS_VCO2, &writeByte,1);
	writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
	WriteRegister(CC112X_FS_VCO4, &writeByte,1);
	writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
	WriteRegister(CC112X_FS_CHP, &writeByte, 1);
	}
	else {
	writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
	WriteRegister(CC112X_FS_VCO2, &writeByte,1);
	writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
	WriteRegister(CC112X_FS_VCO4, &writeByte, 1);
	writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
	WriteRegister(CC112X_FS_CHP, &writeByte,1);
	}
}