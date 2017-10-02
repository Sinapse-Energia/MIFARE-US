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
#include "stdio.h"
#include "time.h"
#include "Definitions.h"
#include "stdlib.h"


extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef structTimeRTC;
extern RTC_DateTypeDef structDateRTC;


extern uint8_t WDT_ENABLED;
//extern char *IP_Device;
//extern char *TCP_Server_Domain;


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
			LCD_Display_Update();
			LCD_SetCursor(15,30);
			LCD_Write_String(info_string, Font_11x18);
			break;

		case Reading:

			strcpy(info_string,"Reading Card");
			LCD_Display_Update();
			LCD_SetCursor(15,30);
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Registered:
			//info_string = "Registered";
			strcpy(info_string,"Registered");
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,33);
			strcpy(info_string,"in Server");
			LCD_Write_String(info_string, Font_7x10);
			if(structTimeRTC.Hours < 10) strcpy(info_string, "0");
			strcpy(info_string, itoa(structTimeRTC.Hours, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Minutes < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Minutes, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Seconds < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Seconds, temporal, 10));
			//LCD_Display_Update();
			LCD_SetCursor(35,45);
			LCD_Write_String(info_string, Font_7x10);

			break;

		case Not_Registered:
			//info_string = "Not Registered";
			strcpy(info_string,"Not Registered");
			LCD_Display_Update();
			LCD_SetCursor(10,23);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,33);
			strcpy(info_string,"in Server(Error)");
			//info_string = "in Server(Error)";
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Initialization:
			strcpy(info_string,"Initializating");
			LCD_Display_Update();
			LCD_SetCursor(10,10);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,20);
			strcpy(info_string,"System");
			//info_string = "IP server";
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,45);
			strcpy(info_string,"Please wait...");
			LCD_Write_String(info_string, Font_7x10);
			break;

		case RTC_display:

			if(structTimeRTC.Hours < 10) strcpy(info_string, "0");
			strcpy(info_string, itoa(structTimeRTC.Hours, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Minutes < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Minutes, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Seconds < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Seconds, temporal, 10));
			LCD_Display_Update();
			LCD_SetCursor(25,45);
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Registering:

			strcpy(info_string,"Registering");
			LCD_Display_Update();
			LCD_SetCursor(10,10);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,20);
			strcpy(info_string,"Please wait...");
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Testing:

			strcpy(info_string,"TESTING");
			LCD_Display_Update();
			LCD_SetCursor(5,25);
			LCD_Write_String(info_string, Font_7x10);
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
			break;

		case Initialization:
			break;

		case Normal:
			break;

		case Not_Registered:
			break;

		case RTC_display:
			break;

		case Testing:
			break;

		case Registering:
			break;
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



void CleanBufferReception(void) {

	//HAL_NVIC_DisableIRQ (USART3_IRQn);
	uint16_t counter = 0;
	for (counter = 0; counter < 4000; counter++)
		bufferReception[counter] = 0x00;
	BufferReceptionCounter = 0;
	//HAL_NVIC_EnableIRQ(USART3_IRQn);
}

void FillTags()
{

	strcpy(stags.tag1, "<horfeus>");
	strcpy(stags.tag2, "<firma>");
	strcpy(stags.tag3, "<aula>");
	strcpy(stags.tag4, "<aulas>");
	strcpy(stags.tag5, "<IP>");
	strcpy(stags.tag6, "<resultado>");
	strcpy(stags.tag7, "<dispositivo>");
	strcpy(stags.tag8, "<lista>");
	strcpy(stags.tag9, "<codigo>");
	strcpy(stags.tag10, "<edificio>");
	strcpy(stags.tag11, "<tipo>");
	strcpy(stags.tag12, "<nombre>");
	strcpy(stags.tag13, "<TipoCliente>");
	strcpy(stags.tag14, "<tiempo>");
	strcpy(stags.tag15, "<TipoAula>");
	strcpy(stags.tag16, "<serie>");
	strcpy(stags.tag17, "<registro>");
	strcpy(etags.tag1, "</horfeus>");
	strcpy(etags.tag2, "</firma>");
	strcpy(etags.tag3, "</aula>");
	strcpy(etags.tag4, "</aulas>");
	strcpy(etags.tag5, "</IP>");
	strcpy(etags.tag6, "</resultado>");
	strcpy(etags.tag7, "</dispositivo>");
	strcpy(etags.tag8, "</lista>");
	strcpy(etags.tag9, "</codigo>");
	strcpy(etags.tag10, "</edificio>");
	strcpy(etags.tag11, "</tipo>");
	strcpy(etags.tag12, "</nombre>");
	strcpy(etags.tag13, "</TipoCliente>");
	strcpy(etags.tag14, "</tiempo>");
	strcpy(etags.tag15, "</TipoAula>");
	strcpy(etags.tag16, "</serie>");
	strcpy(etags.tag17, "</registro>");
}

char *Encode_Payload(HTTP_METHOD method, Memory_Context context)
{
	//uint8_t counter = 0;

	char XML_msg[400]; //This size was 300
	for (int i=0; i<400;i++) XML_msg[i]=0;

	switch (method)
	{
		case GET:

			strcpy(XML_msg, stags.tag1);
			strcat(XML_msg, stags.tag4);
			strcat(XML_msg, stags.tag5);
			//strcat(XML_msg, IP_Device);
			strcat(XML_msg, Context.IP);
			strcat(XML_msg, etags.tag5);
			strcat(XML_msg, etags.tag4);
			strcat(XML_msg, etags.tag1);
			break;

		case POST:

			strcpy(XML_msg, stags.tag1);
			strcat(XML_msg, stags.tag2);
			strcat(XML_msg, stags.tag5);
			//strcat(XML_msg, IP_Device);
			strcat(XML_msg, Context.IP);
			strcat(XML_msg, etags.tag5);
			//strcat(XML_msg, "hola");
			strcat(XML_msg, stags.tag13);
			strcat(XML_msg, context.ClientType);
			strcat(XML_msg, etags.tag13);
			strcat(XML_msg, stags.tag14);
			//strcat(XML_msg, context.Time_server);
			strcat(XML_msg, "28/09/2017-14:45:00");
			strcat(XML_msg, etags.tag14);
			strcat(XML_msg, stags.tag10);
			strcat(XML_msg, context.Building);
			strcat(XML_msg, etags.tag10);
			strcat(XML_msg, stags.tag15);
			strcat(XML_msg, context.Classroom_Type);
			strcat(XML_msg, etags.tag15);
			strcat(XML_msg, stags.tag3);
			strcat(XML_msg, context.Classroom);
			strcat(XML_msg, etags.tag3);
			strcat(XML_msg, stags.tag16);
			strcat(XML_msg, context.Serie);
			strcat(XML_msg, etags.tag16);
			strcat(XML_msg, etags.tag2);
			strcat(XML_msg, etags.tag1);

			break;
	}
	char *array = XML_msg;
	return array;

}

uint8_t decodeServerResponse(char *bufferdecode)
{
	uint8_t decodenumbertags = 0;
	char *p1;
	char *p2;
	uint8_t countermax = 0;

	//Find tag <codigo>
	p1 = (strstr(bufferdecode, stags.tag9));
	if(p1)
	{
		p2  =(strstr(bufferdecode, etags.tag9));

		countermax = strlen(p1) - strlen(p2)-8;

		char stringcode[countermax+1];
		memcpy(stringcode, (p1+8), countermax);
		memcpy(stringcode+countermax, "\0",1);
		strcpy(Context.Classroom, stringcode);

		p1 = NULL; p2 = NULL;
		decodenumbertags+=1;
	}

	//Find tag <edificio>
	p1 = (strstr(bufferdecode, stags.tag10));

	if(p1){

		p2  =(strstr(bufferdecode, etags.tag10));
		countermax = strlen(p1) - strlen(p2)-10;

		char stringbuilding[countermax+1];
		memcpy(stringbuilding, (p1+10), countermax);
		memcpy(stringbuilding+countermax, "\0",1);
		strcpy(Context.Building, stringbuilding);
		p1 = NULL; p2 = NULL;
		decodenumbertags+= 1;
	}

	//Find tag <tipo>
	p1 = (strstr(bufferdecode, stags.tag11));

	if(p1){

		p2  =(strstr(bufferdecode, etags.tag11));
		countermax = strlen(p1) - strlen(p2)-6;

		char stringClasstype[countermax+1];
		memcpy(stringClasstype, (p1+6), countermax);
		memcpy(stringClasstype+countermax, "\0",1);
		strcpy(Context.Classroom_Type, stringClasstype);
		p1 = NULL; p2 = NULL;
		decodenumbertags +=1;
	}

	//Find tag <registro>
	p1 = (strstr(bufferdecode, stags.tag17));

	if(p1){

		p2  =(strstr(bufferdecode, etags.tag17));
		decodenumbertags =1;
	}
	MIC_Flash_Memory_Write((const uint8_t *) &Context, sizeof(Context));
	return decodenumbertags;
}

void GPIO_default(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PA0 | GPIO_PA1 | GPIO_PA2 | GPIO_PA3 | GPIO_PA4
							| GPIO_PA5 | GPIO_PA6 | GPIO_PA7 | GPIO_PA8 | GPIO_PA11 | GPIO_PA12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PB8 | GPIO_PB10 | GPIO_PB11 | GPIO_PB12 | GPIO_PB14
								| GPIO_PB15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PC15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

