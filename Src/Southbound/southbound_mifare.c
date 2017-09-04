/*
 * southbound_generic.c
 *
 *  Created on: 17 ago. 2017
 *      Author: FLG
 */

#include <southbound_mifare.h>
#include <southbound_generic.h>
#include "ssd1306.h"
#include "fonts.h"


void LCD_Init(void)
{
	LCD_Set_Parameters();
	HAL_Delay(500);
	LCD_Display_Update();
}

void LCD_Display_Update(void)
{
	//Fill and Update screen
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
}

void LCD_SetCursor(uint8_t x, uint8_t y)
{
	ssd1306_SetCursor(x,y);
}

void LCD_Write_mifare_info(Device_Status status)
{
	char *info_string = NULL;

	switch(status)
	{
		case Normal:
			info_string = "System OK";
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			break;

		case Reading:
			info_string = "Reading Card";
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			break;

		case Registered:
			info_string = "Registered";
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			LCD_SetCursor(10,33);
			info_string = "in Server";
			LCD_Write_String(info_string);
			break;

		case Not_Registered:
			info_string = "Not Registered";
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			LCD_SetCursor(10,33);
			info_string = "in Server(Error)";
			LCD_Write_String(info_string);
			break;

		case Init_OK:
			info_string = "Initialization";
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			LCD_SetCursor(10,33);
			info_string = "OK";
			LCD_Write_String(info_string);
			break;
	}

}


void Blink_LED_Status(Device_Status status)
{
	uint8_t counter = 0;

	switch (status)
	{
		case Reading:

			for(counter = 0; counter < 4 ; counter++){
				MIC_Set_Digital_Output_status(0,0);
				HAL_Delay(150);
				MIC_Set_Digital_Output_status(0,1);
				HAL_Delay(150);
			}
			break;

		case Registered:

			MIC_Set_Digital_Output_status(0,0);
			HAL_Delay(500);
			MIC_Set_Digital_Output_status(0,1);

	}
}

void PWM_signal_simulator(void)
{
	uint8_t counter = 0;

	for(counter = 0; counter < 50 ; counter++){
		MIC_Set_Digital_Output_status(1,0);
		HAL_Delay(0.025);
		MIC_Set_Digital_Output_status(1,1);
		HAL_Delay(0.05);
	}
}
void Buzzer_Control(Buzzer_tone tone)
{
	uint8_t i=0;

	switch(tone)
	{
		case short_beep_1:

			PWM_signal_simulator();
			break;

		case short_beep_2:

			while(tone !=0){
				PWM_signal_simulator();
				tone--;
				HAL_Delay(100);
			}
			break;

		case long_beep:
			for(i=0;i<5;i++){
				PWM_signal_simulator();
			}
	}
}


uint8_t sendingATCommands(UART_HandleTypeDef *phuart1, uint32_t timeoutTx,
		uint32_t timeoutRx, uint32_t numberOfReceivedBytes,
		unsigned char *messageTX, unsigned char *messageRX) {
	int lengthOfmessage = 0;


	HAL_StatusTypeDef UARTStatus;

	while (messageTX[lengthOfmessage++] != '\r')
		;

	UARTStatus = MIC_UART_Get_Data(phuart1, &data, 1);
	//UARTStatus = HAL_UART_Receive_IT(phuart1, &dato, 1); // activo interrupcion
	UARTStatus = MIC_UART_Send_Data(phuart1, messageTX, lengthOfmessage,timeoutTx);

	while ((BufferReceptionCounter < numberOfReceivedBytes)
			& (timeout == 0))
		; /// Espera activa hasta que al menos me hayan llegado numberOfReceivedBytes bytes
	HAL_Delay(timeoutRx); // además 100ms

	messageRX = bufferReception;

	if (UARTStatus == HAL_OK)
		return 1;
	else
		return 0;

}


HLKStatus HK_Set_Config (UART_HandleTypeDef *phuart1, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX)
{
	uint8_t responseOK = 0;
	uint8_t reintentos = 0;

	//Select UART0 to send AT messages
	MIC_Set_Digital_Output_status(2,0);
	HAL_Delay(2500);
	MIC_Set_Digital_Output_status(2,1);
	HAL_Delay(1000);

	//HAL_IWDG_Refresh(&hiwdg);
	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 13,(uint8_t*) "at+Netmode=1\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RNetmode")) responseOK = 1;
		else reintentos++;

	}

	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 11,(uint8_t*) "at+Dhcpc=0\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RDhcpc")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 23,(uint8_t*) "at+WANIp=192.168.1.200\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RWANIp")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 27,(uint8_t*) "at+WANIpMask=255.255.255.0\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RWANIpMask")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 19,(uint8_t*) "at+SGw=192.168.1.1\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSGw")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 21,(uint8_t*) "at+SDnsF=192.168.1.1\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSDnsF")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 15,(uint8_t*) "at+NProType0=2\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RNProType0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 27,(uint8_t*) "at+NDomain0=89.248.100.11\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RNDomain0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 13,(uint8_t*) "at+NRPort=80\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RNRPort")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 17,(uint8_t*) "at+NLPort0=8080\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RNLPort")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 14,(uint8_t*) "at+NTcpTo0=0\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RNTcpTo0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 17,(uint8_t*) "at+SBaud0=115200\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSBaud0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 13,(uint8_t*) "at+SWidth0=8\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSWidth0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 12,(uint8_t*) "at+SPari0=0\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSPari0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 12,(uint8_t*) "at+SStop0=1\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSStop0")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 10,(uint8_t*) "at+Save=1\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RSave")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);

	while ((reintentos < retries) & (responseOK == 0))
	{
		sendingATCommands(&huart1, timeoutTx, timeoutRx, 11,(uint8_t*) "at+Apply=1\r\n",
				messageRX);

		if(strstr((const char *)bufferReception, (const char *)"at+RApply")) responseOK = 1;
		else reintentos++;

	}
	if (responseOK == 0)
	{
		return HLK_UART_FAIL;
	}
	CleanBufferReception();
	reintentos = 0;
	responseOK = 0;
	//HAL_IWDG_Refresh(&hiwdg);


	sendingATCommands(&huart1, timeoutTx, timeoutRx, 0,(uint8_t*) "at+Reboot=1\r\n",
				messageRX);

	//HAL_IWDG_Refresh(&hiwdg);

	return HLK_OK;
}



void CleanBufferReception(void) {

	//HAL_NVIC_DisableIRQ (USART3_IRQn);
	uint16_t counter = 0;
	for (counter = 0; counter < 100; counter++)
		bufferReception[counter] = 0x00;
	BufferReceptionCounter = 0;
	//HAL_NVIC_EnableIRQ(USART3_IRQn);

}
