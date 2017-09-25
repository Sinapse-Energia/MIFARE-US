
#include "tm_stm32f4_mfrc522.h"
#include "southbound_generic.h"

#ifndef TIM_MAJERLE
//4 bytes Serial number of card, the 5th byte is crc
extern unsigned char serNum[5];
//7 bytes Serial number of card, the 8th byte is crc
extern unsigned char serNum7[8];
//buffer
//uchar str[MAX_LEN];

extern unsigned char defaultKeyA[16];
extern unsigned char madKeyA[16];
extern unsigned char NDEFKeyA[16];
//#define MAX_LEN 16
#define byte unsigned char
#endif

extern SPI_HandleTypeDef hspi1;


void test_spi(void)
{
	uint8_t value = 0;
  while (1)
  {

	   //value= TM_MFRC522_ReadRegister(0x37);
	  MFRC522_CS_LOW;
#ifdef TIM_MAJERLE
	  value = TM_MFRC522_ReadRegister(0x37);
#else
	  value = MFRC522_ReadRegister(0x37);
#endif

	  MFRC522_CS_HIGH;

  }
}

#ifdef TIM_MAJERLE

void TM_MFRC522_Init(void) {
	TM_MFRC522_InitPins();

	TM_MFRC522_Reset();


	TM_MFRC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
    TM_MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);
	TM_MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);
	TM_MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);





	/* 48dB gain */
	TM_MFRC522_WriteRegister(MFRC522_REG_RF_CFG, 0x70);
	
	TM_MFRC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
	TM_MFRC522_WriteRegister(MFRC522_REG_MODE, 0x3D);

	TM_MFRC522_AntennaOn();		//Open the antenna
}

TM_MFRC522_Status_t TM_MFRC522_Check(uint8_t* id) {
	TM_MFRC522_Status_t status;
	//Find cards, return card type
	//while (1)
	{
	status = TM_MFRC522_Request(PICC_REQIDL, id);
	}


	if (status == MI_OK) {
		//Card detected
		//Anti-collision, return card serial number 4 bytes
		status = TM_MFRC522_Anticoll(id);	
	}
	TM_MFRC522_Halt();			//Command card into hibernation 

	return status;
}

TM_MFRC522_Status_t TM_MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID) {
	uint8_t i;
	for (i = 0; i < 5; i++) {
		if (CardID[i] != CompareID[i]) {
			return MI_ERR;
		}
	}
	return MI_OK;
}

void TM_MFRC522_InitPins(void) {
	//GPIO_InitTypeDef GPIO_InitStruct;
	//Enable clock

	//RCC_AHB1PeriphClockCmd(MFRC522_CS_RCC, ENABLE);

	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	//GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//CS pin
	//GPIO_InitStruct.GPIO_Pin = MFRC522_CS_PIN;
	//GPIO_Init(MFRC522_CS_PORT, &GPIO_InitStruct);

	MFRC522_CS_HIGH;
	//MFRC522_CS_LOW;
}

void FJ_MFRC522_WriteRegister(uint8_t addr,uint8_t N, uint8_t *val)
{


	    uint8_t i=0;


	    MFRC522_CS_LOW;


	    //MIC_SPI_Write( &hspi1,(addr << 1) & 0x7E);

	    //spi_readwrite((addr << 1) & 0x7E);
	    SPI_transfer_byte(((addr << 1) & 0x7E));


	    for (i=0;i<N;i++)
	    {

	   	//MIC_SPI_Write( &hspi1,val[i]);

	    	//spi_readwrite(val[i]);
	    	SPI_transfer_byte(val[i]);




	   }
	    //HAL_SPI_Transmit( &hspi1, (uint8_t*) &val[0], N, 1000);

		MFRC522_CS_HIGH;


}


void FJ_MFRC522_ReadRegister(uint8_t addr,uint8_t N, uint8_t *val)
{

	    uint8_t i=0;
uint8_t dummy=0;

	    MFRC522_CS_LOW;


	    //MIC_SPI_Write( &hspi1,((addr << 1) & 0x7E)|0x80);
	    //spi_readwrite(((addr << 1) & 0x7E)|0x80);
	    //dummy=spi_read();

	    dummy = SPI_transfer_byte(((addr << 1) & 0x7E)|0x80);

	    for (i=0;i<N;i++)
	    {

	    	//val[i] = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	    	val[i] = SPI_transfer_byte(((addr << 1) & 0x7E)|0x80);
	    	//spi_readwrite(((addr << 1) & 0x7E)|0x80);
	    	//val[i]=spi_read();


	    }
	    val[i] = SPI_transfer_byte(0);
	    //val[i]=spi_read();
	    //val[i] = MIC_SPI_TransmitReceive( &hspi1,0 );

		MFRC522_CS_HIGH;


}


void TM_MFRC522_WriteRegister(uint8_t addr, uint8_t val) {
	//CS low

#ifdef SOFTWARE_EMULATED
	uint8_t dummy=0;

	MFRC522_CS_LOW;

	//Send address
	//TM_SPI_Send(MFRC522_SPI, (addr << 1) & 0x7E);

	//MIC_SPI_Write( &hspi1,(addr << 1) & 0x7E);
	//MIC_SPI_Write( &hspi1,val);
	//HAL_Delay(1);
	//dummy = MIC_SPI_TransmitReceive( &hspi1,(addr << 1) & 0x7E );
	//dummy = MIC_SPI_TransmitReceive( &hspi1, val );
	//spi_readwrite(((addr << 1) & 0x7E));
	//spi_readwrite(val);

	SPI_transfer_byte(((addr << 1) & 0x7E));
	SPI_transfer_byte(val);

	//HAL_Delay(1);

	//CS high

	MFRC522_CS_HIGH;
#else

	uint8_t dummy=0;

	//MIC_SPI_Write( &hspi1,(addr << 1) & 0x7E);
	//MIC_SPI_Write( &hspi1, val);
	MFRC522_CS_LOW;

	dummy = MIC_SPI_TransmitReceive(&hspi1,(addr << 1) & 0x7E  );
	dummy = MIC_SPI_TransmitReceive( &hspi1, val );

	MFRC522_CS_HIGH;
#endif
}

uint8_t TM_MFRC522_ReadRegister(uint8_t addr) {

#ifdef SOFTWARE_EMULATED
	uint8_t val;
	uint8_t dummy = 0;
	//CS low

	MFRC522_CS_LOW;

	//HAL_Delay(1);
	//TM_SPI_Send(MFRC522_SPI, ((addr << 1) & 0x7E) | 0x80);
	//MIC_SPI_Write( &hspi1,((addr << 1) & 0x7E)|0x80);
	//val = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	//dummy = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	//val = MIC_SPI_TransmitReceive( &hspi1,1 );
	//HAL_Delay(1);
	//val = TM_SPI_Send(MFRC522_SPI, MFRC522_DUMMY);
	//val= MIC_SPI_Read(&hspi1,1);
	//CS high

	//spi_readwrite(((addr << 1) & 0x7E)|0x80 );
	//val=spi_read();

	dummy=SPI_transfer_byte(((addr << 1) & 0x7E)|0x80);
	val =SPI_transfer_byte(0);


	MFRC522_CS_HIGH;





	return val;
#else

	uint8_t dummy=0;
	uint8_t value=0;

	MFRC522_CS_LOW;
	dummy = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	value = MIC_SPI_TransmitReceive( &hspi1,0 );

	//HAL_SPI_TransmitReceive( &hspi1, (uint8_t*) &byte, (uint8_t*) &receivedbyte, 1, SpixTimeout
	MFRC522_CS_HIGH;

	//MIC_SPI_Write( &hspi1, ((addr << 1) & 0x7E)|0x80);
	//value= MIC_SPI_Read(&hspi1,1);


	return value;
#endif

}

void TM_MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
	TM_MFRC522_WriteRegister(reg, TM_MFRC522_ReadRegister(reg) | mask);
}

void TM_MFRC522_ClearBitMask(uint8_t reg, uint8_t mask){
	TM_MFRC522_WriteRegister(reg, TM_MFRC522_ReadRegister(reg) & (~mask));
} 

void TM_MFRC522_AntennaOn(void) {
	uint8_t temp;

	temp = TM_MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if (!(temp & 0x03)) {
		TM_MFRC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
	}
}

void TM_MFRC522_AntennaOff(void) {
	TM_MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void TM_MFRC522_Reset(void) {
	TM_MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

TM_MFRC522_Status_t TM_MFRC522_Request(uint8_t reqMode, uint8_t* TagType) {
	TM_MFRC522_Status_t status;  
	uint16_t backBits;			//The received data bits

	TM_MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);		//TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = TM_MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10)) {    
		status = MI_ERR;
	}

	return status;
}

TM_MFRC522_Status_t TM_MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen) {
	TM_MFRC522_Status_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits=0;
	uint8_t n=0;
	uint8_t m=0;
	uint32_t i=0;

	switch (command) {
		case PCD_AUTHENT: {
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE: {
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
	}

	TM_MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	TM_MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	TM_MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);



	TM_MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);
/*
	if ((sendData[0]==0x93)&(sendData[1]==0x70))  //debug SPI timming
	{
		while (1)
		{
			for (i = 0; i < sendLen; i++) {
					TM_MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
				}


		}
	}
	*/
	//Writing data to the FIFO
	for (i = 0; i < sendLen; i++) {
		TM_MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
	}
	//FJ_MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA,sendLen, sendData);  // Francis

	//Execute the command
	TM_MFRC522_WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE) {    
		TM_MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);		//StartSend=1,transmission of data starts  
	}   

	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	//i=1000000;
	do {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		//HAL_Delay(1);
		n = TM_MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		m = TM_MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);

		i--;
	} while ((i!=0) &&  !(n&waitIRq) && !(n&0x01) );

	TM_MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);			//StartSend=0

	if (i != 0)  {
		if (!(TM_MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)) {
		//if ((TM_MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)==0x00) {
			status = MI_OK;
			if (n & irqEn & 0x01) {   
				status = MI_NOTAGERR;			
			}

			if (command == PCD_TRANSCEIVE) {
				n = TM_MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
				lastBits = TM_MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
				if (lastBits) { // francis
				//if (lastBits!=0) { // francis
					*backLen = (n - 1) * 8 + lastBits;   
				} else {   
					*backLen = n * 8;   
				}

				if (n == 0) {   
					n = 1;    
				}
				if (n > MFRC522_MAX_LEN) {   
					n = MFRC522_MAX_LEN;   
				}

				//Reading the received data in FIFO
				for (i = 0; i < n; i++) {
					backData[i] = TM_MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA);
				}

				//FJ_MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA,n, backData); // Francis
			}
		} else {   
			status = MI_ERR;  
		}
	}

	return status;
}

TM_MFRC522_Status_t TM_MFRC522_Anticoll(uint8_t* serNum) {
	TM_MFRC522_Status_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	TM_MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);		//TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = TM_MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK) {
		//Check card serial number
		for (i = 0; i < 4; i++) {   
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i]) {   
			status = MI_ERR;    
		}
	}
	return status;
} 

void TM_MFRC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData) {
	uint8_t i, n;

	TM_MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);			//CRCIrq = 0
	TM_MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);			//Clear the FIFO pointer
	//Write_MFRC522(CommandReg, PCD_IDLE);

	//Writing data to the FIFO	
	for (i = 0; i < len; i++) {   
		TM_MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata+i));   
	}
	TM_MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	//Wait CRC calculation is complete
	i = 0xFF;
	do {
		n = TM_MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read CRC calculation result
	pOutData[0] = TM_MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = TM_MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

uint8_t TM_MFRC522_SelectTag(uint8_t* serNum) {
	uint8_t i;
	TM_MFRC522_Status_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[10];
	buffer[9]=0;

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) {
		buffer[i+2] = *(serNum+i);
	}
	TM_MFRC522_CalculateCRC(buffer, 7, &buffer[7]);		//??
	status = TM_MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);


	if ((status == MI_OK) && (recvBits == 0x18)) {   
		size = buffer[0]; 
	} else {   
		size = 0;    
	}

	return size;
}

TM_MFRC522_Status_t TM_MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum) {
	TM_MFRC522_Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12]; 

	//Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++) {    
		buff[i+2] = *(Sectorkey+i);   
	}
	for (i=0; i<4; i++) {    
		buff[i+8] = *(serNum+i);   
	}
	status = TM_MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

	if ((status != MI_OK) || (!(TM_MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08))) {   
		status = MI_ERR;   
	}

	return status;
}

TM_MFRC522_Status_t TM_MFRC522_Read(uint8_t blockAddr, uint8_t* recvData) {
	TM_MFRC522_Status_t status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	TM_MFRC522_CalculateCRC(recvData,2, &recvData[2]);
	status = TM_MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90)) {
		status = MI_ERR;
	}

	return status;
}

TM_MFRC522_Status_t TM_MFRC522_Write(uint8_t blockAddr, uint8_t* writeData) {
	TM_MFRC522_Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18]; 

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	TM_MFRC522_CalculateCRC(buff, 2, &buff[2]);
	status = TM_MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {   
		status = MI_ERR;   
	}

	if (status == MI_OK) {
		//Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++) {    
			buff[i] = *(writeData+i);   
		}
		TM_MFRC522_CalculateCRC(buff, 16, &buff[16]);
		status = TM_MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {   
			status = MI_ERR;   
		}
	}

	return status;
}

void TM_MFRC522_Halt(void) {
	uint16_t unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	TM_MFRC522_CalculateCRC(buff, 2, &buff[2]);

	TM_MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}


#else

////////////////////////////////////////////////////////////////// Original code copied by TIM below
/// https://github.com/rena2019/myarduino/blob/master/RC522DumpMifare.ino
void loop(void)
{
    int status;
    unsigned char buffer[MAX_LEN];
    if (selectCard(1))
    {
         for(int block=0; block < 64; block++)
         {
              status = MFRC522_Auth(PICC_AUTHENT1A, block, defaultKeyA, serNum); //auth with default key
              if (status != MI_OK)
              {
                   selectCard(0);
                  status = MFRC522_Auth(PICC_AUTHENT1A, block, madKeyA, serNum); //auth with MAD key
              }
              if (status != MI_OK)
              {
                  selectCard(0);
                   status = MFRC522_Auth(PICC_AUTHENT1A, block, NDEFKeyA, serNum); //auth NDEF data key
              }
              if (status == MI_OK)
              {
                   status = MFRC522_Read(block, buffer);
                   if (status == MI_OK)
                   {
                        if (block % 4 == 0)
                        {
                            //Serial.print("Sector ");
                            //Serial.print(block / 4, DEC);
                            //Serial.println(": ");

                        }
                        dumpHex((char*)buffer, MAX_LEN);
                    }
                    else
                    {
                        //Serial.println("Read failed");
                        break;
                    }
              }
              else
              {
                  //Serial.println("Auth failed");
                  //TODO Mifare Ultra-Light
                  //MIFARE Ultralight C http://www.nxp.com/documents/short_data_sheet/MF0ICU2_SDS.pdf
                  break;
              }
         }//for
         HAL_Delay(1000);
    }
    else
    {
        //Serial.println("no card select");
    }
    //reset/init for next loop
    MFRC522_Init();
    HAL_Delay(500);
}

int selectCard(int dumpInfo)
{
	int status;
	unsigned char buffer[MAX_LEN];
  //Search card, return card types
  status = MFRC522_Request(PICC_REQIDL, buffer);//ShortFrame: 0x26 REQA (Request Type A)
  //status = MFRC522_Request(PICC_REQALL, buffer);//0x52 WUPA (Wake-Up)
  if (status == MI_OK)
  {
     if (dumpInfo)
     {
         //Serial.print("Card detected.\r\n ATQA:");
         //dumpHex((char*)buffer, 2);
         //Serial.println(" ");
     }
     //Prevent conflict, return the 4 bytes Serial number of the card
     status = MFRC522_Anticoll(buffer);
     if (status == MI_OK)
     {
         memcpy(serNum, buffer, 5);
         unsigned char sak = 0;
         status = MFRC522_SelectTag(serNum, &sak);
         if (status == MI_OK && ((sak & 0x04) == 0x00))
         {
             if (dumpInfo)
             {
                 //Serial.print(" UID: ");
                 //dumpHex((char*)serNum, 4);
                 //Serial.println("");
             }
             if ((sak & 0x20) == 0x20)
             {
                 //ISO/IEC FCD 14443-3: Table 9 — Coding of SAK
                 //if (dumpInfo)
                 //    Serial.println(" UID complete, PICC compliant with ISO/IEC 14443-4");
                 //send RATS (Request for Answer To Select)
            	 unsigned char ats[MAX_LEN];
                 uint unLen = 0;
                 status = MFRC522_RATS(ats, &unLen);
                 if (status == MI_OK && dumpInfo)
                 {
                      //Serial.println(" ATS: ");
                      dumpHex((char*)ats, ats[0]);
                      //Serial.println("");
                 }
             }
             if (dumpInfo)
             {
                 //Serial.print(" SAK: ");
                 //Serial.print(sak, HEX);
                 //Serial.println("");
             }
             return 1;
         }
         else
         {
             //cascading level 2
             memcpy(serNum7, &serNum[1], 3);//cascading L1
             status = MFRC522_Anticoll2(buffer);
             if (status == MI_OK)
             {
                 memcpy(&serNum7[3], buffer, 4);
                 status = MFRC522_SelectTag2(&serNum7[3], &sak);
                 if (dumpInfo)
                 {
                    //Serial.print(" UID: ");
                    dumpHex((char*)serNum7, 7);
                    //Serial.println("");
                    //Serial.print(" SAK: ");
                    //Serial.print(sak, HEX);
                    //Serial.println("");
                 }
                 return 1;
             }
             else
             {
                 //Serial.println("ANTICOLL error: cascading level 2");
             }
         }
   }//Anticoll
   else
   {
      //Serial.print("ANTICOLL failed");
   }
 }
 else
 {
     //Serial.print("-");
 }
 return 0;
}//selectCard

/*
 * Function：Write_MFRC5200
 * Description：write a byte data into one register of MR RC522
 * Input parameter：addr--register address；val--the value that need to write in
 * Return：Null
 */
void Write_MFRC522(unsigned char addr, unsigned char val)
{
	//digitalWrite(chipSelectPin, LOW);

	//address format：0XXXXXX0
	//SPI.transfer((addr<<1)&0x7E);
	//SPI.transfer(val);

	//digitalWrite(chipSelectPin, HIGH);

#ifdef SOFTWARE_EMULATED
	uint8_t dummy=0;

	MFRC522_CS_LOW;

	//Send address
	//TM_SPI_Send(MFRC522_SPI, (addr << 1) & 0x7E);

	//MIC_SPI_Write( &hspi1,(addr << 1) & 0x7E);
	//MIC_SPI_Write( &hspi1,val);
	//HAL_Delay(1);
	//dummy = MIC_SPI_TransmitReceive( &hspi1,(addr << 1) & 0x7E );
	//dummy = MIC_SPI_TransmitReceive( &hspi1, val );
	//spi_readwrite(((addr << 1) & 0x7E));
	//spi_readwrite(val);

	SPI_transfer_byte(((addr << 1) & 0x7E));
	SPI_transfer_byte(val);

	//HAL_Delay(1);

	//CS high

	MFRC522_CS_HIGH;
#else

	uint8_t dummy=0;

	//MIC_SPI_Write( &hspi1,(addr << 1) & 0x7E);
	//MIC_SPI_Write( &hspi1, val);
	MFRC522_CS_LOW;

	dummy = MIC_SPI_TransmitReceive(&hspi1,(addr << 1) & 0x7E  );
	dummy = MIC_SPI_TransmitReceive( &hspi1, val );

	MFRC522_CS_HIGH;
#endif
}


/*
 * Function：Read_MFRC522
 * Description：read a byte data into one register of MR RC522
 * Input parameter：addr--register address
 * Return：return the read value
 */
unsigned char Read_MFRC522(unsigned char addr)
{
	//unsigned char val;

	//digitalWrite(chipSelectPin, LOW);

	//address format：1XXXXXX0
	//SPI.transfer(((addr<<1)&0x7E) | 0x80);
	//val =SPI.transfer(0x00);

	//digitalWrite(chipSelectPin, HIGH);

#ifdef SOFTWARE_EMULATED
	uint8_t val;
	uint8_t dummy = 0;
	//CS low

	MFRC522_CS_LOW;

	//HAL_Delay(1);
	//TM_SPI_Send(MFRC522_SPI, ((addr << 1) & 0x7E) | 0x80);
	//MIC_SPI_Write( &hspi1,((addr << 1) & 0x7E)|0x80);
	//val = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	//dummy = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	//val = MIC_SPI_TransmitReceive( &hspi1,1 );
	//HAL_Delay(1);
	//val = TM_SPI_Send(MFRC522_SPI, MFRC522_DUMMY);
	//val= MIC_SPI_Read(&hspi1,1);
	//CS high

	//spi_readwrite(((addr << 1) & 0x7E)|0x80 );
	//val=spi_read();

	dummy=SPI_transfer_byte(((addr << 1) & 0x7E)|0x80);
	val =SPI_transfer_byte(0);


	MFRC522_CS_HIGH;





	return val;
#else

	uint8_t dummy=0;
	uint8_t value=0;

	MFRC522_CS_LOW;
	dummy = MIC_SPI_TransmitReceive( &hspi1,((addr << 1) & 0x7E)|0x80 );
	value = MIC_SPI_TransmitReceive( &hspi1,0 );

	//HAL_SPI_TransmitReceive( &hspi1, (uint8_t*) &byte, (uint8_t*) &receivedbyte, 1, SpixTimeout
	MFRC522_CS_HIGH;

	//MIC_SPI_Write( &hspi1, ((addr << 1) & 0x7E)|0x80);
	//value= MIC_SPI_Read(&hspi1,1);


	return value;
#endif


}

/*
 * Function：SetBitMask
 * Description：set RC522 register bit
 * Input parameter：reg--register address;mask--value
 * Return：null
 */
void SetBitMask(unsigned char reg, unsigned char mask)
{
	unsigned char tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}


/*
 * Function：ClearBitMask
 * Description：clear RC522 register bit
 * Input parameter：reg--register address;mask--value
 * Return：null
 */
void ClearBitMask(unsigned char reg, unsigned char mask)
{
    unsigned char tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
}


/*
 * Function：AntennaOn
 * Description：Turn on antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameter：null
 * Return：null
 */
void AntennaOn(void)
{
	unsigned char temp;

	temp = Read_MFRC522(TxControlReg);
	if (!(temp & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}


/*
 * Function：AntennaOff
 * Description：Turn off antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameter：null
 * Return：null
 */
void AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}


/*
 * Function：ResetMFRC522
 * Description： reset RC522
 * Input parameter：null
 * Return：null
 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}


/*
 * Function：InitMFRC522
 * Description：initilize RC522
 * Input parameter：null
 * Return：null
 */
void MFRC522_Init(void)
{
   //digitalWrite(NRSTPD,HIGH);

   MFRC522_Reset();

    //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);
    Write_MFRC522(TReloadRegH, 0);

    Write_MFRC522(TxAutoReg, 0x40);		//100%ASK
    Write_MFRC522(ModeReg, 0x3D);		//CRC initilizate value 0x6363	???

    //ClearBitMask(Status2Reg, 0x08);		//MFCrypto1On=0
    //Write_MFRC522(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
    //Write_MFRC522(RFCfgReg, 0x7F);   		//RxGain = 48dB

    AntennaOn();		//turn on antenna
}


/*
 * Function：MFRC522_Request
 * Description：Searching card, read card type
 * Input parameter：reqMode--search methods，
 *			 TagType--return card types
 *			 	0x4400 = Mifare_UltraLight
 *				0x0400 = Mifare_One(S50)
 *				0x0200 = Mifare_One(S70)
 *				0x0800 = Mifare_Pro(X)
 *				0x4403 = Mifare_DESFire
 * return：return MI_OK if successed
 */
int MFRC522_Request(int reqMode, unsigned char *TagType)
{
	int status;
	uint backBits;			//the data bits that received

	Write_MFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
	{
	     status = MI_ERR;
/*
             Serial.print("status: ");
             Serial.print(status, HEX);
             Serial.print(" backBits: ");
             Serial.print(backBits, HEX);
             Serial.println("");
*/
	}
	return status;
}


/*
 * Function：MFRC522_ToCard
 * Description：communicate between RC522 and ISO14443
 * Input parameter：command--MF522 command bits
 *			 sendData--send data to card via rc522
 *			 sendLen--send data length
 *			 backData--the return data from card
 *			 backLen--the length of return data
 * return：return MI_OK if successed
 */
int MFRC522_ToCard(int command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, uint *backLen)
{
    int status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitIRq = 0x00;
    unsigned char lastBits;
    unsigned char n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:		//verify card password
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
	case PCD_TRANSCEIVE:	//send data in the FIFO
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
	default:
			break;
    }

    Write_MFRC522(CommIEnReg, irqEn|0x80);	//Allow interruption
    ClearBitMask(CommIrqReg, 0x80);			//Clear all the interrupt bits
    SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO initilizate

    Write_MFRC522(CommandReg, PCD_IDLE);	//NO action;cancel current command	???

    //write data into FIFO
    for (i=0; i<sendLen; i++)
    {
        Write_MFRC522(FIFODataReg, sendData[i]);
    }

    //procceed it
    Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
    }

    //waite receive data is finished
    i = 2000;	//i should adjust according the clock, the maxium the waiting time should be 25 ms???
    do
    {
	//CommIrqReg[7..0]
	//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);			//StartSend=0

    if (i != 0)
    {
        if(!(Read_MFRC522(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
               status = MI_NOTAGERR;			//??
            }

            if (command == PCD_TRANSCEIVE)
            {
               	n = Read_MFRC522(FIFOLevelReg);
              	lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {
		   *backLen = (n-1)*8 + lastBits;
		}
                else
                {
		   *backLen = n*8;
		}

                if (n == 0)
                {
		    n = 1;
		}
                if (n > MAX_LEN)
                {
		    n = MAX_LEN;
		}

		//read the data from FIFO
                for (i=0; i<n; i++)
                {
		    backData[i] = Read_MFRC522(FIFODataReg);
		}
            }
        }
        else
        {
	    status = MI_ERR;
	}

    }
    else
    {
         //Serial.print("i=0");
    }

    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE);

    return status;
}


/*
 * Function：MFRC522_Anticoll
 * Description：Prevent conflict, read the card serial number
 * Input parameter：serNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * return：return MI_OK if successed
 */
int MFRC522_Anticoll(unsigned char *serNum)
{
    int status;
    unsigned char i;
    unsigned char serNumCheck=0;
    uint unLen;


    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);		//ValuesAfterColl
    Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
	 //Verify card serial number
         for (i=0; i<4; i++)
	 {
	     serNumCheck ^= serNum[i];
	 }
	 if (serNumCheck != serNum[i])
	 {
	     status = MI_ERR;
	 }
    }
    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1
    return status;
}

//ANTICOLL cascading level 2
int MFRC522_Anticoll2(unsigned char *serNum)
{
    int status;
    unsigned char i;
    unsigned char serNumCheck=0;
    uint unLen;


    //ClearBitMask(Status2Reg, 0x08);		//TempSensclear
    //ClearBitMask(CollReg,0x80);		//ValuesAfterColl
    Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL2;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
	 //Verify card serial number
         for (i=0; i<4; i++)
	 {
	     serNumCheck ^= serNum[i];
	 }
	 if (serNumCheck != serNum[i])
	 {
	     status = MI_ERR;
	 }
    }
    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1
    return status;
}

//send RATS and returns ATS
int MFRC522_RATS(unsigned char *recvData, uint *pLen)
{
    int status;
    uint unLen = 0;

    recvData[0] = 0xE0; // RATS
    recvData[1] = 0x50; // FSD=128, CID=0
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
    /*
    Serial.print("unLen: ");
    Serial.print(unLen, HEX);
    /*Serial.print(" status: ");
    Serial.print(status, HEX);
    Serial.println("");
    */
    //TODO
    //if ((status != MI_OK) || (unLen != 0x90))
    //{
    //    status = MI_ERR;
    //}
    return status;
}


/*
 * Function：CalulateCRC
 * Description：Use MF522 to caculate CRC
 * Input parameter：pIndata--the CRC data need to be read，len--data length，pOutData-- the caculated result of CRC
 * return：Null
 */
void CalulateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
    unsigned char i, n;

    ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);			//Clear FIFO pointer
    //Write_MFRC522(CommandReg, PCD_IDLE);

	//Write data into FIFO
    for (i=0; i<len; i++)
    {
        Write_MFRC522(FIFODataReg, *(pIndata+i));
    }
    Write_MFRC522(CommandReg, PCD_CALCCRC);

	//waite CRC caculation to finish
    i = 0xFF;
    do
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//read CRC caculation result
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
 * Function：MFRC522_SelectTag
 * Description：Select card, read card storage volume
 * Input parameter：serNum--Send card serial number
 * sak see ISO14443-3 Table 9 — Coding of SAK
 * return return MI_OK if successed
 */
int MFRC522_SelectTag(unsigned char *serNum, unsigned char *sak)
{
    unsigned char i;
    int status;
    //unsigned char size;
    uint recvBits;
    unsigned char buffer[9];
    //unsigned char buffCheck=0;

    //ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
    CalulateCRC(buffer, 7, &buffer[7]);		//??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    //TODO: the above call returns 2 instead of MI_OK -> why?
    status = MI_OK;
    //Serial.print("recvBits: ");
    //Serial.print(recvBits, DEC);
    /*
    for (i=0; i<recBits / 8; i++)
    {
    	buff[i] = *(buffer+i);
    }*/
    //dumpHex((char*)buffer, recvBits / 8);
    *sak = buffer[0];
    //Verify received buffer
    /* TODO
    for (i=0; i< recvBits/8; i++)
    {
       buffCheck ^= buffer[i];
    }
    if (buffCheck != buffer[i])
    {
       status = MI_ERR;
    }*/
    return status;
}

int MFRC522_SelectTag2(unsigned char *serNum, unsigned char *sak)
{
    unsigned char i;
    int status;
    //unsigned char size;
    uint recvBits;
    unsigned char buffer[9];
    //unsigned char buffCheck=0;

    //ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_ANTICOLL2;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
    	buffer[i+2] = *(serNum+i);
    }
    CalulateCRC(buffer, 7, &buffer[7]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    //TODO: the above call returns 2 instead of MI_OK -> why?
    status = MI_OK;
    //Serial.print("recvBits: ");
    //Serial.print(recvBits, DEC);
    /*
    for (i=0; i<recBits / 8; i++)
    {
    	buff[i] = *(buffer+i);
    }*/
    //dumpHex((char*)buffer, recvBits / 8);
    *sak = buffer[0];
    //Verify received buffer
    /* TODO
    for (i=0; i< recvBits/8; i++)
    {
       buffCheck ^= buffer[i];
    }
    if (buffCheck != buffer[i])
    {
       status = MI_ERR;
    }*/
    return status;
}


/*
 * Function：MFRC522_Auth
 * Description：verify card password
 * Input parameters：authMode--password verify mode
                 0x60 = verify A passowrd key
                 0x61 = verify B passowrd key
             BlockAddr--Block address
             Sectorkey--Block password
             serNum--Card serial number ，4 bytes
 * return：return MI_OK if successed
 */
int MFRC522_Auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
    int status;
    uint recvBits;
    unsigned char i;
    unsigned char buff[12];

    //Verify command + block address + buffer password + card SN
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {
        buff[i+2] = *(Sectorkey+i);
    }
    for (i=0; i<4; i++)
    {
        buff[i+8] = *(serNum+i);
    }
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function：MFRC522_Read
 * Description：Read data
 * Input parameters：blockAddr--block address;recvData--the block data which are read
 * return：return MI_OK if successed
 */
int MFRC522_Read(unsigned char blockAddr, unsigned char *recvData)
{
    int status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function：MFRC522_Write
 * Description：write block data
 * Input parameters：blockAddr--block address;writeData--Write 16 bytes data into block
 * return：return MI_OK if successed
 */
unsigned char MFRC522_Write(unsigned char blockAddr, unsigned char *writeData)
{
    int status;
    uint recvBits;
    unsigned char i;
    unsigned char buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        for (i=0; i<16; i++)		//Write 16 bytes data into FIFO
        {
        	buff[i] = *(writeData+i);
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
			status = MI_ERR;
		}
    }

    return status;
}

/*
 * Function：MFRC522_Halt
 * Description：Command the cards into sleep mode
 * Input parameters：null
 * return：null
 */
void MFRC522_Halt(void)
{
    int status;
    uint unLen;
    unsigned char buff[4];

    //ISO14443-3: 6.4.3 HLTA command
    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);

    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

void dumpHex(char* buffer, int len)
{
  for(byte i=0; i < len; i++) {
     char text[4];
     if (i % 16 == 0) {
        //Serial.print(" ");
     }
     sprintf(text, "%02X \x00", (byte)(*(buffer + i)));
     //Serial.print(text);

     if (i % 16 == 15) {
        //Serial.println("");
     }
  }
  //Serial.println(" ");

}




void StopCrypto1(void)
{
	  ClearBitMask(Status2Reg, 0x08);


}


int RFID_Read_Memory_Block(int blockTrail, int blockRead, unsigned char *buffer)
{
	 int status=MI_ERR;

	  if (selectCard(1))
	    {

             status = MFRC522_Auth(PICC_AUTHENT1A, blockTrail, defaultKeyA, serNum); //auth with default key
             if (status != MI_OK)
             {
                 selectCard(0);
	             status = MFRC522_Auth(PICC_AUTHENT1A, blockTrail, madKeyA, serNum); //auth with MAD key
              }
             if (status != MI_OK)
             {
                 selectCard(0);
                 status = MFRC522_Auth(PICC_AUTHENT1A, blockTrail, NDEFKeyA, serNum); //auth NDEF data key
             }

             if (status == MI_OK)
	         {
                 status = MFRC522_Read(blockRead, buffer);
                 StopCrypto1();
             if (status == MI_OK)
             {
                 return MI_OK;
              }
              else
             {

                   return MI_ERR;
              }
         }
         else
         {
                return MI_ERR;
          }


	    }
	    else
	    {
	        return MI_ERR;
	    }


}

#endif
