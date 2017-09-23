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
#include "stdlib.h"

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef structTimeRTC;
extern RTC_DateTypeDef structDateRTC;
extern IWDG_HandleTypeDef hiwdg;

extern char NTPbuffer[20];
extern uint8_t NTP_Sync_state;
extern IWDG_HandleTypeDef hiwdg;
extern uint8_t WDT_ENABLED;
extern char *IP_Device;
extern char *TCP_Server_Domain;
extern unsigned char NTPpacket[NTP_PACKET_SIZE];

extern Start_TAGS stags;
extern End_TAGS etags;

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
	char info_string[160];
	char temporal[3];

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
			strcpy(info_string,"Initializating");
			LCD_Display_Update();
			LCD_SetCursor(5,10);
			LCD_Write_String(info_string);
			LCD_SetCursor(5,20);
			strcpy(info_string,"System");
			//info_string = "IP server";
			LCD_Write_String(info_string);
			LCD_SetCursor(5,45);
			strcpy(info_string,"Please wait...");
			LCD_Write_String(info_string);
			break;

		case RTC_display:
			//strcpy(info_string, NTPbuffer);
			//strcpy(info_string, "NTP OK");

			if(structTimeRTC.Hours < 10) strcpy(info_string, "0");
			strcpy(info_string, itoa(structTimeRTC.Hours, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Minutes < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Minutes, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Seconds < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Seconds, temporal, 10));
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string);
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

	UARTStatus = MIC_UART_Get_Data(phuart1, &data, 1); //IRQ active
	UARTStatus = MIC_UART_Send_Data(phuart1, messageTX, lengthOfmessage,timeoutTx);

	while ((BufferReceptionCounter < numberOfReceivedBytes)
			& (timeout == 0))
		; /// Wait until numberOfReceivedBytes bytes is OK
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

	strftime(NTPbuffer,20,"%d/%m/%Y %X", NTP_time);

	structTimeRTC.Hours = NTP_time->tm_hour;
	structTimeRTC.Minutes = NTP_time->tm_min;
	structTimeRTC.Seconds = NTP_time->tm_sec;

	structTimeRTC.TimeFormat = RTC_HOURFORMAT12_AM;
	structTimeRTC.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	structTimeRTC.StoreOperation = RTC_STOREOPERATION_RESET;

	structDateRTC.Year = (NTP_time->tm_year + 1900 - 2000);
	structDateRTC.Month = NTP_time->tm_mon +1;
	structDateRTC.Date = NTP_time->tm_mday;

	//Set internal RTC
	MIC_Set_RTC (&hrtc, &structTimeRTC, &structDateRTC, RTC_FORMAT_BIN);

	strcpy(Context.Time_server, (const char *)NTPbuffer);
	//Update context in Flash

}


void CleanBufferReception(void) {

	//HAL_NVIC_DisableIRQ (USART3_IRQn);
	uint16_t counter = 0;
	for (counter = 0; counter < 100; counter++)
		bufferReception[counter] = 0x00;
	BufferReceptionCounter = 0;
	//HAL_NVIC_EnableIRQ(USART3_IRQn);

}

void FillTags()
{

	strcpy(stags.tag1, "%3Chorfeus%3E");
	strcpy(stags.tag2, "%3Cfirma%3E");
	strcpy(stags.tag3, "%3Caula%3E");
	strcpy(stags.tag4, "%3Caulas%3E");
	strcpy(stags.tag5, "%3CIP%3E");
	strcpy(stags.tag6, "%3Cresultado%3E");
	strcpy(stags.tag7, "%3Cdispositivo%3E");
	strcpy(stags.tag8, "%3Clista%3E");
	strcpy(stags.tag9, "%3Ccodigo%3E");
	strcpy(stags.tag10, "%3Cedificio%3E");
	strcpy(stags.tag11, "%3Ctipo%3E");
	strcpy(stags.tag12, "%3Cnombre%3E");
	strcpy(stags.tag13, "%3CTipoCliente%3E");
	strcpy(stags.tag14, "%3Ctiempo%3E");
	strcpy(stags.tag15, "%3CTipoAula%3E");
	strcpy(stags.tag16, "%3Cserie%3E");
	strcpy(etags.tag1, "%3C%2Fhorfeus%3E");
	strcpy(etags.tag2, "%3C%2Ffirma%3E");
	strcpy(etags.tag3, "%3C%2Faula%3E");
	strcpy(etags.tag4, "%3C%2Faulas%3E");
	strcpy(etags.tag5, "%3C%2FIP%3E");
	strcpy(etags.tag6, "%3C%2Fresultado%3E");
	strcpy(etags.tag7, "%3C%2Fdispositivo%3E");
	strcpy(etags.tag8, "%3C%2Flista%3E");
	strcpy(etags.tag9, "%3C%2Fcodigo%3E");
	strcpy(etags.tag10, "%3C%2Fedificio%3E");
	strcpy(etags.tag11, "%3C%2Ftipo%3E");
	strcpy(etags.tag12, "%3C%2Fnombre%3E");
	strcpy(etags.tag13, "%3C%2FTipoCliente%3E");
	strcpy(etags.tag14, "%3C%2Ftiempo%3E");
	strcpy(etags.tag15, "%3C%2FTipoAula%3E");
	strcpy(etags.tag16, "%3C%2Fserie%3E");

}

char *Encode_XML(HTTP_METHOD method, Memory_Context context)
{
	uint8_t counter = 0;

	char XML_msg[300];
	for (int i=0; i<300;i++) XML_msg[i]=0;

	memcpy(context.ClientType,"4",1);
	memcpy(context.Building ,"43",2);
	memcpy(context.Classroom_Type, "6",1);
	memcpy(context.Classroom , "RM120022",8);
	memcpy(context.Serie , "00394492", 8);

	switch (method)
	{
		case GET:

			memcpy(XML_msg+counter, stags.tag1, 13); counter+=13;
			memcpy(XML_msg+counter, stags.tag4,11); counter+=11;
			memcpy(XML_msg+counter, stags.tag5,8); counter+=8;
			memcpy(XML_msg+counter, IP_Device, strlen(IP_Device));
			counter+=strlen(IP_Device);
			//memcpy(XML_msg+counter, Context.IP, strlen(Context.IP));
			//counter+=strlen(Context.IP);
			memcpy(XML_msg+counter, etags.tag5, 11); counter+=11;
			memcpy(XML_msg+counter, etags.tag4, 14); counter+=14;
			memcpy(XML_msg+counter, etags.tag1, 16); counter+=16;
			memcpy(XML_msg+counter, "\0",1);

			break;

		case POST:

			memcpy(XML_msg+counter, stags.tag1, 13); counter+=13;
			memcpy(XML_msg+counter, stags.tag2,11); counter+=11;
			memcpy(XML_msg+counter, stags.tag5,8); counter+=8;
			memcpy(XML_msg+counter, IP_Device, strlen(IP_Device));
			counter+=strlen(IP_Device);
			memcpy(XML_msg+counter, etags.tag5, 11); counter+=11;
			memcpy(XML_msg+counter, stags.tag13, 17); counter+=17;
			memcpy(XML_msg+counter, context.ClientType, strlen(context.ClientType));
			counter+=strlen(context.ClientType);
			memcpy(XML_msg+counter, etags.tag13, 20); counter+=20;
			memcpy(XML_msg+counter, stags.tag14, 12); counter+=12;
			memcpy(XML_msg+counter, context.Time_server, strlen(context.Time_server));
			counter+=strlen(context.Time_server);
			memcpy(XML_msg+counter, etags.tag14, 15); counter+=15;
			memcpy(XML_msg+counter, stags.tag10, 14); counter+=14;
			memcpy(XML_msg+counter, context.Building, strlen(context.Building));
			counter+=strlen(context.Building);
			memcpy(XML_msg+counter, etags.tag10, 17); counter+=17;
			memcpy(XML_msg+counter, stags.tag15, 14); counter+=14;
			memcpy(XML_msg+counter, context.Classroom_Type, strlen(context.Classroom_Type));
			counter+=strlen(context.Classroom_Type);
			memcpy(XML_msg+counter, etags.tag15, 17); counter+=17;
			memcpy(XML_msg+counter, stags.tag3, 10); counter+=10;
			memcpy(XML_msg+counter, context.Classroom, strlen(context.Classroom));
			counter+=strlen(context.Classroom);
			memcpy(XML_msg+counter, etags.tag3, 13); counter+=13;
			memcpy(XML_msg+counter, stags.tag16, 11); counter+=11;
			memcpy(XML_msg+counter, context.Serie, strlen(context.Serie));
			counter+=strlen(context.Serie);
			memcpy(XML_msg+counter, etags.tag16, 14); counter+=14;
			memcpy(XML_msg+counter, "\0",1);

			break;
	}
	char *array = XML_msg;
	return array;

}

char *Build_HTTP_msg(char *Payload, HTTP_METHOD method)
{
	char HTTP_array[HTTP_ARRAY_SIZE];

	switch(method)
	{
		case GET:

			strcpy(HTTP_array, HTTP_METHOD_GET);
			strcat(HTTP_array, " ");
			strcat(HTTP_array,HTTP_HEADER_DESTINATION);
			strcat(HTTP_array, Payload);
			strcat(HTTP_array, " ");
			strcat(HTTP_array, HTTP_HEADER_PROTOCOL);
			strcat(HTTP_array, CARRIAGE_RETURN);
			strcat(HTTP_array, NEWLINE);
			strcat(HTTP_array, TCP_Server_Domain);
			strcat(HTTP_array, CARRIAGE_RETURN);
			strcat(HTTP_array, NEWLINE);
			strcat(HTTP_array, CARRIAGE_RETURN);
			strcat(HTTP_array, NEWLINE);

			break;

		case POST:

			strcpy(HTTP_array, HTTP_METHOD_POST);
			strcat(HTTP_array, " ");
			strcat(HTTP_array,HTTP_HEADER_DESTINATION);
			strcat(HTTP_array, Payload);
			strcat(HTTP_array, " ");
			strcat(HTTP_array, HTTP_HEADER_PROTOCOL);
			strcat(HTTP_array, CARRIAGE_RETURN);
			strcat(HTTP_array, NEWLINE);
			strcat(HTTP_array, TCP_Server_Domain);
			strcat(HTTP_array, CARRIAGE_RETURN);
			strcat(HTTP_array, NEWLINE);
			strcat(HTTP_array, CARRIAGE_RETURN);
			strcat(HTTP_array, NEWLINE);

	}
	char *HTTP_msg = HTTP_array;
	return HTTP_msg;


}

uint8_t HTTP_request(char *HTTPbuffer)
{
	char *mensaje = malloc(strlen(HTTPbuffer));
	strcpy(mensaje, HTTPbuffer);

	//CleanBufferReception();
	//Select UART0 interface to send message
	MIC_Set_Digital_Output_status(2,0);
	HAL_Delay(1000);
	MIC_Set_Digital_Output_status(2,1);
	HAL_Delay(1000);
	sendingATCommands(&huart1, 100, 500,14, (uint8_t *)"at+SAtMode0=0\r\n", messageRX);
	CleanBufferReception();
	while(BufferReceptionCounter == 0)
	{
		//Active interruption
		MIC_UART_Get_Data(&huart1, &data, 1);

		//MIC_UART_Send_Data(&huart1,(uint8_t*)&NTPpacket,48,100);
		//MIC_UART_Send_Data(&huart1,(uint8_t*)"GET /index.htm HTTP/1.1\r\nHost: 192.168.1.116\r\n\r\n",48,100);
		MIC_UART_Send_Data(&huart1,(uint8_t*)mensaje,strlen(mensaje),100);

		HAL_Delay(500);
		if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);

		if (BufferReceptionCounter>0)
		{
			free(mensaje);
			//HAL_Delay(4000);
		}
		if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);
	}
}
