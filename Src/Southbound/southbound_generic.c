/*
 * southbound_generic.c
 *
 *  Created on: 21 ago. 2017
 *      Author: FLG
 */

/*Includes*/
#include "southbound_generic.h"
#include "southbound_mifare.h"
#include "ssd1306.h"
#include "fonts.h"
#include "Definitions.h"
#include "string.h"


extern char *IP_Device;
extern char *IP_Mask;
extern char *IP_Gateway;
extern char *IP_Dns;

extern char *NTP_Server_Domain;
extern char *NTP_Server_Port;
extern char *TCP_Server_Domain;
extern char *TCP_Server_Port;
extern char *IP_Local_Port;

extern uint8_t WDT_ENABLED;
extern HKStatus HK_Status;
extern TCPStatus TCP_Status;
extern unsigned char NTPpacket[NTP_PACKET_SIZE];
///////////////////////////////////////////////////////////////////////////////////////
/* Internal FLASH memory functions */
int MIC_Flash_Memory_Write(const uint8_t *data_in, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t sizeReceived=0;

	status=FlashNVM_ErasePage(ADDR_FLASH_PAGE_127);  // It is erased last page in microcontroller;

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Write(ADDR_FLASH_PAGE_127+4, data_in, size); // data are written with +4 offset from starting. First 4 bytes are used to write size of data
																// and watching if there are data or not.
	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Write(ADDR_FLASH_PAGE_127, (uint8_t* )&size, 4); // quantity of data are saved at first 4 bytes.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Read(ADDR_FLASH_PAGE_127, (uint8_t* )&sizeReceived, 4); // quantity of data are read again to verify it wrote good.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	if (status==HAL_OK) return sizeReceived; // if all goes fine, it returns size of data.

	return status;
}

int MIC_Flash_Memory_Read(const uint8_t *data_out, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t sizeReceived=0;

	status=FlashNVM_Read(ADDR_FLASH_PAGE_127+4, (uint8_t *)data_out, size); // start to read all data.
	status=FlashNVM_Read(ADDR_FLASH_PAGE_127+4, (uint8_t *)data_out, size); // start to read all data.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Read(ADDR_FLASH_PAGE_127, (uint8_t *)&sizeReceived, 4); // received quantity of saved bytes.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	if ((status==HAL_OK)&&(sizeReceived==size))
	{
		//CopyFlashMemorystruct_to_Localstruct();
		return sizeReceived; // if all goes fine, it returns size of data.
	}
	return status;
}

HAL_StatusTypeDef FlashNVM_ErasePage(uint32_t page)
{

	HAL_StatusTypeDef status;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError = 0;

	HAL_FLASH_Unlock();
	//__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	//                   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );


	EraseInitStruct.PageAddress=page;
	EraseInitStruct.NbPages=1;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;

	status = HAL_BUSY;
	while (status == HAL_BUSY) {
		status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	}
	HAL_FLASH_Lock();

	return status;
}


/**
  * @brief  Read a binary array from FLASH
  * @param  address: FLASH relative address to read
  * @param  data_out: output data array pointer
  * @param  size: array length
  * @retval operation status
  */
HAL_StatusTypeDef FlashNVM_Read(uint32_t start_address, uint8_t* data_out, uint32_t size)
{
    uint32_t sizeCounter = 0;


	// Check input data
   // if (!IS_FLASH_ADDRESS(start_address)) {
        // It's not Flash's address
    //	return HAL_ERROR;
	//}

	while (sizeCounter < size) {
	    *data_out = (*(__IO uint8_t*)start_address);
	    data_out++;
	    start_address++;
	    sizeCounter++;
	}
    return HAL_OK;
}


/**
  * @brief  write data array to PREVIOSLY ERISED FLASH memory
  * @param  fl_bank: flash area (application or its copy bank)
  * @retval true if OK,  otherwise return false
  */
HAL_StatusTypeDef FlashNVM_Write(uint32_t start_address, const uint8_t* data_in, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t i;
	uint16_t *integerPointer;
	integerPointer = (uint16_t *)data_in;

	// Check input data
    //if (!IS_FLASH_ADDRESS(start_address)) {
        // It's not Flash's address
    //	return HAL_ERROR;
	//}

	HAL_FLASH_Unlock();
	//__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	//                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );


	// Write data
    for (i = 0; i < size/2; i+=1) {
    	status = HAL_BUSY;
    	while (status == HAL_BUSY) {
    		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, start_address + 2*i, *integerPointer);
    		integerPointer++;
    	}
    	if ( status != HAL_OK) {
    		break;
    	}
    }

	HAL_FLASH_Lock();

	return status;
}
///////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
/* Generic function thats write 1/0 over GPIO_Pin*/
void MIC_Set_Digital_Output_status(GPIO_Pin_Select pin, PIN_Status status)
{
	switch (pin)
	{
		case 0:

			if(status == 0) HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);
			if(status == 1) HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
			break;

		case 1:

			if(status == 0) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			if(status == 1) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
			break;

		case 2:

			if(status == 0) HAL_GPIO_WritePin(ES0_GPIO_Port, ES0_Pin, GPIO_PIN_RESET);
			if(status == 1) HAL_GPIO_WritePin(ES0_GPIO_Port, ES0_Pin, GPIO_PIN_SET);
			break;

		case 3:

			if(status == 0) HAL_GPIO_WritePin(ES1_GPIO_Port, ES1_Pin, GPIO_PIN_RESET);
			if(status == 1) HAL_GPIO_WritePin(ES1_GPIO_Port, ES1_Pin, GPIO_PIN_SET);
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/*SB generic function to manage LCD Display*/
 void LCD_Set_Parameters(void)
{
	ssd1306_Init();
}


void LCD_Write_String(char *string)
{
	ssd1306_WriteString(string, Font_7x10, Black);
	ssd1306_UpdateScreen();
}
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/* SB generic function to transmit and receive data via UART*/
uint8_t MIC_UART_Send_Data(UART_HandleTypeDef *huart, unsigned char* messageTX, uint8_t lengthOfmessage, uint32_t timeoutTX)
{
	HAL_UART_Transmit(huart, messageTX,lengthOfmessage, timeoutTX);
}

uint8_t MIC_UART_Get_Data(UART_HandleTypeDef *huart, unsigned char* messageRX, uint8_t Size)
{
	HAL_UART_Receive_IT(huart, messageRX, Size);
}


HKStatus HK_Set_Config (HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX)
{
	uint8_t responseOK = 0;
	uint8_t retries_counter = 0;
	char msgTX[50];
	for (int i=0; i<50;i++) msgTX[i]=0;
	switch(mode)
	{
		case UART0_to_ETH:

			//RESET ES0 pin less 6 seconds to select UART0 interface
			MIC_Set_Digital_Output_status(2,0);
			HAL_Delay(2500);
			MIC_Set_Digital_Output_status(2,1);
			HAL_Delay(1000);

			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Network mode settings: 0 default, 1 ETH, 2 WIFI STA, 3 WIFI AP, 4 WIFI AP Client
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX ,"at+Netmode=0\r\n");
				sendingATCommands(phuart, timeoutTx, timeoutRx, 13, (uint8_t *)msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RNetmode")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0)  return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;

			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set the WAN port IP address acquisition mode: 0 Dynamic (default), 1 Static
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX,"at+Dhcpc=0\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 11,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RDhcpc")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0)  return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set the static IP address of the WAN port
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+WANIp=");
				strcat(msgTX,IP_Device);
				strcat(msgTX, "\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 23,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RWANIp")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0)  return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;

			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set the Subnet mask of the WAN port static IP
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+WANIpMask=");
				strcat(msgTX,IP_Mask);
				strcat(msgTX, "\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 27,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RWANIpMask")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0)  return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set the WAN port static Ip gateway
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+SGw=");
				strcat(msgTX,IP_Gateway);
				strcat(msgTX, "\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 19,(uint8_t*)msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RSGw")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0)  return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set static main DNS
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+SDnsF=");
				strcat(msgTX,IP_Dns);
				strcat(msgTX, "\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 21,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RSDnsF")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0) return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set serial Baudrate: 115200 default
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+SBaud0=115200\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 17,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RSBaud0")) responseOK = 1;
				else retries_counter++;

			}

			if (responseOK == 0) return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set serial data length: default value 8
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+SWidth0=8\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 13,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RSWidth0")) responseOK = 1;
				else retries_counter++;
			}

			if (responseOK == 0) return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set serial check parity: 0 default
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+SPari0=0\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 12,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RSPari0")) responseOK = 1;
				else retries_counter++;

			}

			if (responseOK == 0) return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			//Set serial stop length: 1 default
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX, "at+SStop0=1\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 12,(uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RSStop0")) responseOK = 1;
				else retries_counter++;

			}

			if (responseOK == 0) return HK_UART_FAIL;

			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);
	}

	/*After sending the configuration command must be saved and then submitted*/
	//Save settings
	while ((retries_counter < retries) & (responseOK == 0))
	{
		strcpy(msgTX,"at+Save=1\r\n");
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 10,(uint8_t*) msgTX,
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSave")) responseOK = 1;
		else retries_counter++;
	}

	if (responseOK == 0) return HK_UART_FAIL;

	CleanBufferReception();
	retries_counter = 0;
	responseOK = 0;
	if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

	//Apply settings
	while ((retries_counter < retries) & (responseOK == 0))
	{
		strcpy(msgTX,"at+Apply=1\r\n");
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 11,(uint8_t*) msgTX,
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RApply")) responseOK = 1;
		else retries_counter++;

	}

	if (responseOK == 0) return HK_UART_FAIL;

	CleanBufferReception();
	retries_counter = 0;
	responseOK = 0;
	if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

	//Reboot it's necessary. No response is expected
	sendingATCommands(&huart1, timeoutTx, timeoutRx, 0,(uint8_t*) "at+Reboot=1\r\n",messageRX);
	CleanBufferReception();

	return HK_OK;
}


HKStatus HK_Get_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart1, uint32_t retries, uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX)
{

	uint8_t responseOK = 0;
	uint8_t retries_counter = 0;
	unsigned char *buffer=NULL;
	char msgTX[50];
		for (int i=0; i<50;i++) msgTX[i]=0;


	switch(mode)
	{

		case UART0_to_ETH:

			//Select UART0 to send AT messages
			MIC_Set_Digital_Output_status(2,0);
			HAL_Delay(1000);
			MIC_Set_Digital_Output_status(2,1);
			HAL_Delay(1000);

			//Read the static IP address of the WAN Port
			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX,"at+WANIp=?\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 20, (uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RWANIp")) responseOK = 1;
				else retries_counter++;

			}
			if (responseOK == 0)	return HK_UART_FAIL;

			//Response OK, save the IP into Context.
			else
			{
				buffer = bufferReception+10;
				char erase[4] = "\r\n";

				buffer= strtok (buffer, erase);
				strcpy(Context.IP, buffer);

			}
			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			while ((retries_counter < retries) & (responseOK == 0))
			{
				strcpy(msgTX,"at+NDomain0=?\r\n");
				sendingATCommands(&huart1, timeoutTx, timeoutRx, 20, (uint8_t*) msgTX,
						messageRX);

				if(strstr((const char *)bufferReception, (const char *)"at+RNDomain0")) responseOK = 1;
				else retries_counter++;

			}
			if (responseOK == 0)	return HK_UART_FAIL;

			//Response OK, save the IP into Context.
			else
			{
				buffer = bufferReception+13;
				char erase[4] = "\r\n";

				buffer= strtok (buffer, erase);
				strcpy(Context.IP_server, buffer);

			}
			CleanBufferReception();
			retries_counter = 0;
			responseOK = 0;
			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);
			break;
	}
	MIC_Flash_Memory_Write((const uint8_t *) &Context, sizeof(Context));

	return HK_OK;
}


HKStatus HK_Connect(HK_Working_Mode mode, Network_Mode netmode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX)
{
	uint8_t responseOK = 0;
	uint8_t retries_counter = 0;
	char msgTX[50];
	for (int i=0; i<50;i++) msgTX[i]=0;

	switch(mode)
	{
		case UART0_to_ETH:

			//RESET ES0 pin less 6 seconds to select UART0 interface
			MIC_Set_Digital_Output_status(2,0);
			HAL_Delay(1000);
			MIC_Set_Digital_Output_status(2,1);
			HAL_Delay(1000);

			if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

			switch (netmode)
			{
				case (TCP):

					//Set Transparency Socket protocol Type
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NProType0=2\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 15,(uint8_t*)msgTX ,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNProType0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set socket remote domain name or IP
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NDomain0=");
						strcat(msgTX, TCP_Server_Domain);
						strcat(msgTX, "\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx,27 ,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNDomain0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set Socket remote terminal
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NRPort0=");
						strcat(msgTX, TCP_Server_Port);
						strcat(msgTX, "\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 13,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNRPort0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set Socket local terminal
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NLPort0=");
						strcat(msgTX, IP_Local_Port);
						strcat(msgTX, "\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 17,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNLPort0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set Socket tcp connection time out
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NTcpTo0=0\r\n");

						sendingATCommands(&huart1, timeoutTx, timeoutRx, 14,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNTcpTo0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					break;

				case (UDP):

					//Set Transparency Socket protocol Type
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NProType0=4\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 15,(uint8_t*)msgTX ,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNProType0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set socket remote domain name or IP
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NDomain0=");
						strcat(msgTX, NTP_Server_Domain);
						strcat(msgTX, "\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 29,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNDomain0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set Socket remote terminal
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NRPort0=");
						strcat(msgTX, NTP_Server_Port);
						strcat(msgTX, "\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 13,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNRPort0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set Socket local terminal
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NLPort0=");
						strcat(msgTX, IP_Local_Port);
						strcat(msgTX, "\r\n");
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 17,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNLPort0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					//Set Socket UDP connection time out
					while ((retries_counter < retries) & (responseOK == 0))
					{
						strcpy(msgTX,"at+NTcpTo0=0\r\n");

						sendingATCommands(&huart1, timeoutTx, timeoutRx, 14,(uint8_t*) msgTX,
								messageRX);

						if(strstr((const char *)bufferReception, (const char *)"at+RNTcpTo0")) responseOK = 1;
						else retries_counter++;
					}

					if (responseOK == 0)  return HK_UART_FAIL;

					CleanBufferReception();
					retries_counter = 0;
					responseOK = 0;
					if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

					break;
			}
	}

	/*After sending the configuration command must be saved and then submitted*/

	//Save settings
	while ((retries_counter < retries) & (responseOK == 0))
	{
		strcpy(msgTX,"at+Save=1\r\n");
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 10,(uint8_t*) msgTX,
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSave")) responseOK = 1;
		else retries_counter++;
	}

	if (responseOK == 0)  return HK_UART_FAIL;

	CleanBufferReception();
	retries_counter = 0;
	responseOK = 0;
	if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

	//Apply settings
	while ((retries_counter < retries) & (responseOK == 0))
	{
		strcpy(msgTX,"at+Apply=1\r\n");
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 11,(uint8_t*) msgTX,
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RApply")) responseOK = 1;
		else retries_counter++;

	}

	if (responseOK == 0)  return HK_UART_FAIL;

	CleanBufferReception();
	retries_counter = 0;
	responseOK = 0;
	if (WDT_ENABLED==1) HAL_IWDG_Refresh(&hiwdg);

	//Reboot it's necessary. No response is expected
	strcpy(msgTX,"at+Reboot=1\r\n");
	sendingATCommands(&huart1, timeoutTx, timeoutRx, 0,(uint8_t*) msgTX,messageRX);

	CleanBufferReception();

	return HK_OK;
}

void MIC_Get_RTC(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime,RTC_DateTypeDef *sDate, uint32_t Format)
{
	//Get Time & Date from Internal_RTC
	HAL_RTC_GetTime(hrtc, sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, sDate, RTC_FORMAT_BIN);
}

void MIC_Set_RTC (RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime,RTC_DateTypeDef *sDate, uint32_t Format)
{
	//Set Time & Date to Internal_RTC
	HAL_RTC_SetTime(hrtc, sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(hrtc, sDate, RTC_FORMAT_BIN);
}

/*TCP generic functions*/

TCPStatus TCP_Connect(HK_Working_Mode mode, Network_Mode netmode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX)
{
	HK_Status = HK_Connect(mode, netmode,  phuart, retries, timeoutTx, timeoutRx, messageRX);

	if (HK_Status == 0) TCP_Status = TCP_OK;
	if (HK_Status == -1) TCP_Status = TCP_FAIL;

	return TCP_Status;
}

TCPStatus TCP_Set_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX)
{
	HK_Status = HK_Set_Config(mode, phuart, retries, timeoutTx, timeoutRx, messageRX);

	if (HK_Status == 0) TCP_Status = TCP_OK;
	if (HK_Status == -1) TCP_Status = TCP_FAIL;

	return TCP_Status;
}

TCPStatus TCP_Get_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries, uint32_t timeoutTx,
		uint32_t timeoutRx, unsigned char *messageRX)
{
	HK_Status = HK_Get_Config(mode, phuart, retries, timeoutTx, timeoutRx, messageRX);

	if (HK_Status == 0) TCP_Status = TCP_OK;
	if (HK_Status == -1) TCP_Status = TCP_FAIL;

	return TCP_Status;
}

uint8_t NTP_Sync(void)
{
	//Connect to NTP server
	while(TCP_Status != 0 )
		TCP_Status = TCP_Connect(0, 1,  &huart1, 2, 100, 500, messageRX);

	//NTP server connection OK, TCP_Status = 0
	if (TCP_Status == 0)
	{
		CleanBufferReception();
		//Wait for NTP response, size 48 bytes
		while(BufferReceptionCounter == 0)
		{
			HAL_Delay(500);
			MIC_UART_Get_Data(&huart1, &data, 1);

			MIC_UART_Send_Data(&huart1,(uint8_t*)&NTPpacket,48,100);
			HAL_Delay(500);

			if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);
			if (BufferReceptionCounter>0)
			{
				Get_NTP_Time(bufferReception);
				HAL_Delay(100);
				//CleanBufferReception();
			}
			if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);
		}

	}
	return TCP_Status;
}
