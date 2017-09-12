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
#include "string.h"
#include "time.h"
#include "Definitions.h"


extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef structTimeRTC;
extern RTC_DateTypeDef structDateRTC;
extern char*bufferNTP;

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
	char info_string[40];
	char ntp_info[20];
	switch(status)
	{
		case Normal:
			strcpy(info_string, "System OK");
			//info_string = "System OK";
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			break;

		case Reading:
			//info_string = "Reading Card";
			strcpy(info_string,"Reading Card");
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			break;

		case Registered:
			//info_string = "Registered";
			strcpy(info_string,"Registered");
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			LCD_SetCursor(10,33);
			strcpy(info_string,"in Server");
			//info_string = "in Server";
			LCD_Write_String(info_string);
			break;

		case Not_Registered:
			//info_string = "Not Registered";
			strcpy(info_string,"Not Registered");
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			LCD_SetCursor(10,33);
			strcpy(info_string,"in Server(Error)");
			//info_string = "in Server(Error)";
			LCD_Write_String(info_string);
			break;

		case Init_OK:
			strcpy(info_string, "TESTING");
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
			//LCD_SetCursor(10,33);
			//strcpy(info_string, Context.IP_server);
			//info_string = "IP server";
			//LCD_Write_String(info_string);
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


void Get_NTP_Time(unsigned char *buffer)
{
	uint32_t NTP_highReceived = 0;
	uint32_t NTP_lowReceived = 0;
	uint32_t NTP_timestampUnix = 0;
	uint32_t NTP_result = 0;


	NTP_highReceived = bufferReception[41] | bufferReception[40] << 8;
	NTP_lowReceived = bufferReception[43] | bufferReception[42] << 8;
	NTP_timestampUnix = NTP_highReceived << 16 | NTP_lowReceived;

	NTP_result = NTP_timestampUnix - NTP_SEVENTYYEARS;
	struct tm* NTP_time = gmtime((const time_t *)&NTP_result);

	bufferNTP = ctime(&NTP_result);
	LCD_Write_mifare_info(4);
	/*NTP_hours = ((NTP_time % 86400UL) / 3600);
	NTP_minutes = ((NTP_time % 3600) / 60);
	NTP_seconds = (NTP_time % 60);*/

	structTimeRTC.Hours = NTP_time->tm_hour;
	structTimeRTC.Minutes = NTP_time->tm_min;
	structTimeRTC.Seconds = NTP_time->tm_sec;

	structTimeRTC.TimeFormat = RTC_HOURFORMAT12_AM;
	structTimeRTC.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	structTimeRTC.StoreOperation = RTC_STOREOPERATION_RESET;

	structDateRTC.Year = (NTP_time->tm_year + 1900 - 2000);
	structDateRTC.Month = NTP_time->tm_mon +1;
	structDateRTC.Date = NTP_time->tm_mday;

	MIC_Set_RTC (&hrtc, &structTimeRTC, &structDateRTC, RTC_FORMAT_BIN);

}




void CleanBufferReception(void) {

	//HAL_NVIC_DisableIRQ (USART3_IRQn);
	uint16_t counter = 0;
	for (counter = 0; counter < 100; counter++)
		bufferReception[counter] = 0x00;
	BufferReceptionCounter = 0;
	//HAL_NVIC_EnableIRQ(USART3_IRQn);

}
