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

extern struct _spiControl spiControl;

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
				strcpy(msgTX ,"at+Netmode=1\r\n");
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
	char *buffer=NULL;
	char msgTX[50];
		for (int i=0; i<50;i++) msgTX[i]=0;


	switch(mode)
	{

		case UART0_to_ETH:

			//Select UART0 to send AT messages
			MIC_Set_Digital_Output_status(2,0);
			HAL_Delay(2500);
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

	return HK_OK;
}


HKStatus HK_Connect(HK_Working_Mode mode, HK_Network_Mode netmode, UART_HandleTypeDef *phuart, uint32_t retries,
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
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 34,(uint8_t*) msgTX,
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
						sendingATCommands(&huart1, timeoutTx, timeoutRx, 34,(uint8_t*) msgTX,
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


///// SPI functions.
#ifdef FIRST_SPI_IMPLEMENTATION
#define SPIx_TIMEOUT_MAX                      ((uint32_t)0x1000)
uint32_t SpixTimeout = SPIx_TIMEOUT_MAX; /*<! Value of Timeout when SPI communication fails */

//Actual structure that handles SPI communication
struct _spiControl spiControl = { 0, _spi_initialize, _spi_write_read ,_spi_write, _spi_read, 0 };


 void _spi_initialize( SPI_HandleTypeDef *hspi, onSPIError funcSPIErrorCallback )
 {
	 spiControl.SPI_Handle = hspi;
	 spiControl.funcSPIErrorCallback = funcSPIErrorCallback;
 }

	/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
uint8_t _spi_write_read( uint8_t byte )
{

	uint8_t receivedbyte = 0x00;
  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive( spiControl.SPI_Handle, (uint8_t*) &byte, (uint8_t*) &receivedbyte, 1, SpixTimeout) != HAL_OK)
  {
    SPIx_Error(spiControl.SPI_Handle);
  }

  return receivedbyte;
}

/**
  * @brief SPI Write a byte to device
  * @param Value: value to be written
  * @retval None
  */
void _spi_write( uint8_t value){
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit( spiControl.SPI_Handle, (uint8_t*) &value, 1, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    SPIx_Error(spiControl.SPI_Handle);
  }
}


/**
  * @brief SPI Read 4 bytes from device
  * @param  ReadSize Number of bytes to read (max 4 bytes)
  * @retval Value read on the SPI
  */
uint8_t _spi_read(uint8_t readSize){

  HAL_StatusTypeDef status = HAL_OK;
  uint32_t readvalue;

  status = HAL_SPI_Receive( spiControl.SPI_Handle, (uint8_t*) &readvalue, readSize, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    SPIx_Error(spiControl.SPI_Handle);
  }
  return readvalue;
}

void SPIx_Error(SPI_HandleTypeDef *hspi )
{
  /* De-initialize the SPI comunication BUS */
  HAL_SPI_DeInit(hspi);

  /* Re- Initiaize the SPI comunication BUS */
	if( spiControl.funcSPIErrorCallback!= 0 ){
		spiControl.funcSPIErrorCallback();
	}
  //MX_SPI1_Init( );
}

#endif

#define SPIx_TIMEOUT_MAX                      ((uint32_t)0x1000)
uint32_t SpixTimeout = SPIx_TIMEOUT_MAX; /*<! Value of Timeout when SPI communication fails */





void MIC_SPI_Write( SPI_HandleTypeDef *hspi1,uint8_t value){
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit( hspi1, (uint8_t*) &value, 1, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the BUS */

  }
}


/**
  * @brief SPI Read 4 bytes from device
  * @param  ReadSize Number of bytes to read (max 4 bytes)
  * @retval Value read on the SPI
  */
uint8_t MIC_SPI_Read(SPI_HandleTypeDef *hspi1,uint8_t readSize)
{

  HAL_StatusTypeDef status = HAL_OK;
  uint32_t readvalue;

  status = HAL_SPI_Receive( hspi1, (uint8_t*) &readvalue, readSize, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the BUS */

  }
  return readvalue;
}


uint8_t MIC_SPI_TransmitReceive( SPI_HandleTypeDef *hspi1,uint8_t byte )
{

	uint8_t receivedbyte = 0x00;
  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive( hspi1, (uint8_t*) &byte, (uint8_t*) &receivedbyte, 1, SpixTimeout) != HAL_OK)
  {

  }

  return receivedbyte;
}



/////////////////////



void spi_readwrite(char value)
{
//register unsigned char temp;

char temp = value;


for (int bitCount= 0; bitCount< 8; bitCount++)
 {
  if(temp& 0x80) HAL_GPIO_WritePin(PORT_spiTX, PIN_spiTX,GPIO_PIN_SET);
  else HAL_GPIO_WritePin(PORT_spiTX, PIN_spiTX,GPIO_PIN_RESET);
 //runNops(1);
 //asm volatile("nop");
 HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_SET);
 temp <<= 1;
 //runNops(1);
 //asm volatile("nop");
 HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_RESET);
 }
}

char spi_read(void)
{

	//register unsigned char temp=0;
	char temp=0;
	 for (int bitCount= 0; bitCount<8; bitCount++)
	 {
	     temp <<= 1;
	     HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_RESET);
	   //  asm volatile("nop");
	     temp = temp | HAL_GPIO_ReadPin(PORT_spiRX, PIN_spiRX);
	   //  asm volatile("nop");
	     HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_SET);
	   //  asm volatile("nop");
	 }
	 HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_RESET);
	// asm volatile("nop");
	 return temp;

}


////////////////// New


write_MOSI (GPIO_PinState value)
{

	if (value==GPIO_PIN_RESET) HAL_GPIO_WritePin(PORT_spiTX, PIN_spiTX,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(PORT_spiTX, PIN_spiTX,GPIO_PIN_SET);

}

write_SCLK (GPIO_PinState value)
{

	if (value==GPIO_PIN_RESET) HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(PORT_spiClock, PIN_spiClock,GPIO_PIN_SET);

}

GPIO_PinState read_MISO (void)
{

	return HAL_GPIO_ReadPin(PORT_spiRX, PIN_spiRX);

}

/*
 * Simultaneously transmit and receive a byte on the SPI.
 *
 * Polarity and phase are assumed to be both 0, i.e.:
 *   - input data is captured on rising edge of SCLK.
 *   - output data is propagated on falling edge of SCLK.
 *
 * Returns the received byte.
 */
uint8_t SPI_transfer_byte(uint8_t byte_out)
{
    uint8_t byte_in = 0;
    uint8_t bit;

    for (bit = 0x80; bit; bit >>= 1) {
        /* Shift-out a bit to the MOSI line */
        write_MOSI((byte_out & bit) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        /* Delay for at least the peer's setup time */


        /* Pull the clock line high */
        write_SCLK(GPIO_PIN_SET);

        /* Shift-in a bit from the MISO line */
        if (read_MISO() == GPIO_PIN_SET)
            byte_in |= bit;

        /* Delay for at least the peer's hold time */


        /* Pull the clock line low */
        write_SCLK(GPIO_PIN_RESET);
    }

    return byte_in;
}
