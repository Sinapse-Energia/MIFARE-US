/*
 * southbound_generic.c
 *
 *  Created on: 17 ago. 2017
 *      Author: FLG
 */

/* Includes */
#include <southbound_mifare.h>
#include <southbound_generic.h>
#include "ssd1306.h"
#include "fonts.h"
#include "string.h"
#include "stdio.h"
#include "time.h"
#include "Definitions.h"
#include "stdlib.h"

/*Global variables*/
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef structTimeRTC;
extern RTC_DateTypeDef structDateRTC;

extern uint8_t WDT_ENABLED;

extern Start_TAGS stags;
extern End_TAGS etags;

/**
 * @Description: This function set initial LCD parameters. It calls the specific LCD library.
 */
void LCD_Init(void)
{
	LCD_Set_Parameters();
	HAL_Delay(500);
	LCD_Display_Update();
}

/**
 * @Description: This function fills and updates all LCD display pixels using the specific LCD library-
 */
void LCD_Display_Update(void)
{
	//Fill and Update screen
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
}

/**
 * @Description: This function set LCD (x,y) cursor to write something
 * 				 It directly uses the ssd1306 library
 */
void LCD_SetCursor(uint8_t x, uint8_t y)
{
	ssd1306_SetCursor(x,y);
}

/**
 * @Description: This function write on LCD Display a specific message
 * 				 according the Device status. Receive as parameter:
 *
 * 				 - Device_Status status: struct that defines the different device status
 */
void LCD_Write_mifare_info(Device_Status status)
{
	char info_string[160];	//Array with properly message to display
	char temporal[3];		//Temporal array to use with 'itoa' function

	switch(status)
	{
		case Normal:	//If system initialization is OK

			strcpy(info_string, "LECTORA OK");
			LCD_Display_Update();
			LCD_SetCursor(10,30);
			LCD_Write_String(info_string, Font_11x18);
			break;

		case Reading:	//If a card is read

			strcpy(info_string,"LEYENDO TARJETA ");
			LCD_Display_Update();
			LCD_SetCursor(10,30);
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Registered: 	//When a card is registered

			//Get internal RTC
			MIC_Get_RTC (&hrtc, &structTimeRTC, &structDateRTC, RTC_FORMAT_BIN);

			strcpy(info_string,"REGISTRADO");
			LCD_Display_Update();
			LCD_SetCursor(10,20);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,30);
			strcpy(info_string,"EN SERVIDOR");
			LCD_Write_String(info_string, Font_7x10);
			//Also, display RTC time
			/*if(structTimeRTC.Hours < 10) strcpy(info_string, "0");
			strcpy(info_string, itoa(structTimeRTC.Hours, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Minutes < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Minutes, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Seconds < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Seconds, temporal, 10));
			strcat(info_string, " (UTC)");
			LCD_SetCursor(10,35);
			LCD_Write_String(info_string, Font_7x10);*/
			break;

		case Not_Registered:	//When a card is not registered

			strcpy(info_string,"NO REGISTRADO");
			LCD_Display_Update();
			LCD_SetCursor(10,20);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,30);
			strcpy(info_string,"EN SERVIDOR");
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Initialization:	//When device start

			strcpy(info_string,"INICIANDO");
			LCD_Display_Update();
			LCD_SetCursor(10,15);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,25);
			strcpy(info_string,"DISPOSITIVO");
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,45);
			strcpy(info_string,"ESPERE...");
			LCD_Write_String(info_string, Font_7x10);
			break;

		case RTC_display:	//Shows RTC time if necessary

			MIC_Get_RTC (&hrtc, &structTimeRTC, &structDateRTC, RTC_FORMAT_BIN);

			if(structTimeRTC.Hours < 10) strcpy(info_string, "0");
			strcpy(info_string, itoa(structTimeRTC.Hours, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Minutes < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Minutes, temporal, 10));
			strcat(info_string, ":");
			if(structTimeRTC.Seconds < 10) strcat(info_string, "0");
			strcat(info_string, itoa(structTimeRTC.Seconds, temporal, 10));
			strcat(info_string, " (UTC)");
			LCD_Display_Update();
			LCD_SetCursor(25,45);
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Registering:	//Registration process for a card

			strcpy(info_string,"REGISTRANDO");
			LCD_Display_Update();
			LCD_SetCursor(10,20);
			LCD_Write_String(info_string, Font_7x10);
			LCD_SetCursor(10,30);
			strcpy(info_string,"ESPERE...");
			LCD_Write_String(info_string, Font_7x10);
			break;

		case Testing:	//Test message

			strcpy(info_string,Context.Classroom_name);
			LCD_Display_Update();
			LCD_SetCursor(10,25);
			LCD_Write_String(info_string, Font_11x18);
			break;
	}
}

/**
 * @Description: This function blinks a LED according the Device status.
 * 				 Uses 'MIC_Set_Digital_Output()' function to write GPIO
 * 				 Receive as parameter:
 *				 - Device_Status status: struct that defines the different device status
 */

void Blink_LED_Status(Device_Status status)
{
	uint8_t counter = 0;

	switch (status)
	{
		case Reading:	//Blink four short times

			for(counter = 0; counter < 4 ; counter++){
				MIC_Set_Digital_Output_status(LED_Status_Pin,0);
				HAL_Delay(150);
				MIC_Set_Digital_Output_status(LED_Status_Pin,1);
				HAL_Delay(150);
			}
			break;

		case Registered:	//Blink one long time

			MIC_Set_Digital_Output_status(LED_Status_Pin,0);
			HAL_Delay(500);
			MIC_Set_Digital_Output_status(LED_Status_Pin,1);
			break;

		case Initialization:	//Do nothing
			break;

		case Normal:			//Do nothing
			break;

		case Not_Registered:	//Do nothing
			break;

		case RTC_display:		//Do nothing
			break;

		case Testing:			//Do nothing
			break;

		case Registering:		//Do nothing
			break;
	}
}

/**
 * @Description: This function simulate PWM signal to turn buzzer on
 * 				 Uses 'MIC_Set_Digital_Output()' function to write GPIO
 */
void PWM_signal_simulator(void)
{
	uint8_t counter = 0;

	for(counter = 0; counter < 50 ; counter++){

		MIC_Set_Digital_Output_status(Buzzer_Pin,0);
		HAL_Delay(0.025); //0.025ms ==> 40 KHz
		MIC_Set_Digital_Output_status(Buzzer_Pin,1);
		HAL_Delay(0.05);
	}
}

/**
 * @Description: This function turn buzzer on according RFID registration.
 * 				 Receive as parameter:
 * 				 - Buzzer_tone tone: struct that indicates tone longitude.
 */
void Buzzer_Control(Buzzer_tone tone)
{
	uint8_t i=0;

	switch(tone)
	{
		case short_beep_1: //One short beep when RFID card is detected

			PWM_signal_simulator();
			break;

		case short_beep_2:	//Two short beeps when RFID card is registered

			while(tone !=0){
				PWM_signal_simulator();
				tone--;
				HAL_Delay(100);
			}
			break;

		case long_beep:		//Long beep when RFID card is not registered
			for(i=0;i<5;i++){
				PWM_signal_simulator();
			}
	}
}


/**
 * @Description: This function clean the buffer reception array
 */
void CleanBufferReception(void) {

	uint16_t counter = 0;
	for (counter = 0; counter < BUFFER_RECEPTION_SIZE; counter++)
		bufferReception[counter] = 0x00;
	BufferReceptionCounter = 0;
}

/**
 * @Description: This function fills the struct tags with adequate name in order to perform a payload
 *
 */
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

/**
 * @Description: This function encodes a payload which will be sent in an HTTP message
 * 				 for MIFARE application.
 * 				 Receive as parameter:
 * 				 - HTTP_METHOD method: struct that indicates HTTP method (GET or POST)
 * 				 - Memory_Context context:  struct with program Context that contains
 * 				 							some necessary parameters in MIFARE payload
 * 				 Function returns the built payload
 */
char *Encode_Payload(HTTP_METHOD method, Memory_Context context)
{
	char payload_msg[MAX_PAYLOAD_SIZE]; 	//Payload array
	for (int i = 0; i < MAX_PAYLOAD_SIZE ; i++)	//Fill with zeros the payload array
		payload_msg[i]=0;

	switch (method)
	{
		case GET:	//GET method

			//MIFARE payload is: <horfeus><aulas><IP>XX.XX.XX.XX.</IP></aulas></horfeus>
			strcpy(payload_msg, stags.tag1);
			strcat(payload_msg, stags.tag4);
			strcat(payload_msg, stags.tag5);
			strcat(payload_msg, context.IP);
			strcat(payload_msg, etags.tag5);
			strcat(payload_msg, etags.tag4);
			strcat(payload_msg, etags.tag1);
			break;

		case POST:	//POST method

			//MIFARE payload is: <horfeus><firma><IP>XX.XX.XX.XX</IP><TipoCliente>XX</TipoCliente>
			//					 <tiempo>DD/MM/AA HH:MM:SS<tiempo><edificio>XX</edificio><TipoAula>
			//					 XX</TipoAula><aula>XXXX</aula><serie>XXXXXX</serie></firma></horfeus>
			strcpy(payload_msg, stags.tag1);
			strcat(payload_msg, stags.tag2);
			strcat(payload_msg, stags.tag5);
			strcat(payload_msg, context.IP);
			strcat(payload_msg, etags.tag5);
			strcat(payload_msg, stags.tag13);
			strcat(payload_msg, context.ClientType);
			strcat(payload_msg, etags.tag13);
			strcat(payload_msg, stags.tag14);
			strcat(payload_msg, context.Time_server);
			strcat(payload_msg, etags.tag14);
			strcat(payload_msg, stags.tag10);
			strcat(payload_msg, context.Building);
			strcat(payload_msg, etags.tag10);
			strcat(payload_msg, stags.tag15);
			strcat(payload_msg, context.Classroom_Type);
			strcat(payload_msg, etags.tag15);
			strcat(payload_msg, stags.tag3);
			strcat(payload_msg, context.Classroom_code);
			strcat(payload_msg, etags.tag3);
			strcat(payload_msg, stags.tag16);
			strcat(payload_msg, context.Serie);
			strcat(payload_msg, etags.tag16);
			strcat(payload_msg, etags.tag2);
			strcat(payload_msg, etags.tag1);

			break;
	}
	//Return payload
	char *array = payload_msg;
	return array;
}

/**
 * @Description: This function decodes a server response. MIFARE application has two process:
 * 				 1) First Process: device sinchronyzation. Here, valid tags in server response
 * 				 are '<codigo>', '<edificio>', '<tipo>'. If tags found, server response is OK.
 * 				 decodenumbertags will be 3.
 * 				 2) Second Process: card registration. When a card is registered, valid tags
 * 				 in server response are '<edificio>', '<registro>'. If tags found, server response
 * 				 is OK. decodenumbertags willbe 2.
 *
 * 				 Receives as parameter:
 * 				 - char *bufferdecode: array with server response.
 *
 * 				 Function returns number of tags found.
 */
uint8_t decodeServerResponse(char *bufferdecode)
{
	uint8_t decodenumbertags = 0;	//Counter tags
	char *p1;	//Auxiliar pointer
	char *p2;	//Auxiliar pointer
	uint8_t TagValue_size = 0;		//String TagValue size

	//Find start tag <codigo>
	p1 = (strstr(bufferdecode, stags.tag9));
	if(p1)
	{
		//Fing end tag </codigo>
		p2  =(strstr(bufferdecode, etags.tag9));

		if(p2){

			TagValue_size = strlen(p1) - strlen(p2)-8;
			//Save 'codigo' tag value
			char TagCodigoValue[TagValue_size+1];
			memcpy(TagCodigoValue, (p1+8), TagValue_size);
			memcpy(TagCodigoValue+TagValue_size, "\0",1);
			//Copy tag value in Context variable
			strcpy(Context.Classroom_code, TagCodigoValue);
			decodenumbertags+=1;
		}
		p1 = NULL; p2 = NULL;

		//Find tag <nombre>
		p1 = (strstr(bufferdecode, stags.tag12));

		if(p1){
			//Find end tag </tipo>
			p2  =(strstr(bufferdecode, etags.tag12));

			if(p2){

				TagValue_size = strlen(p1) - strlen(p2)-8;

				char TagTipoValue[TagValue_size+1];
				memcpy(TagTipoValue, (p1+8), TagValue_size);
				memcpy(TagTipoValue+TagValue_size, "\0",1);
				strcpy(Context.Classroom_name, TagTipoValue);
				decodenumbertags +=1;
			}
			p1 = NULL; p2 = NULL;
		}
	}

	//Find start tag <edificio>
	p1 = (strstr(bufferdecode, stags.tag10));

	if(p1){
		//Find end tag </edificio>
		p2  =(strstr(bufferdecode, etags.tag10));

		if(p2){

			TagValue_size = strlen(p1) - strlen(p2)-10;

			char TagEdificioValue[TagValue_size+1];
			memcpy(TagEdificioValue, (p1+10), TagValue_size);
			memcpy(TagEdificioValue+TagValue_size, "\0",1);
			strcpy(Context.Building, TagEdificioValue);
			decodenumbertags+= 1;
		}
		p1 = NULL; p2 = NULL;
	}

	//Find start tag <tipo>
	p1 = (strstr(bufferdecode, stags.tag11));

	if(p1){
		//Find end tag </tipo>
		p2  =(strstr(bufferdecode, etags.tag11));

		if(p2){

			TagValue_size = strlen(p1) - strlen(p2)-6;

			char TagTipoValue[TagValue_size+1];
			memcpy(TagTipoValue, (p1+6), TagValue_size);
			memcpy(TagTipoValue+TagValue_size, "\0",1);
			strcpy(Context.Classroom_Type, TagTipoValue);
			decodenumbertags +=1;
		}
		p1 = NULL; p2 = NULL;
	}



	//Find tag <registro>
	p1 = (strstr(bufferdecode, stags.tag17));

	if(p1){
		//Find end tag </registro>
		p2  =(strstr(bufferdecode, etags.tag17));

		if(p2){
			decodenumbertags +=1;
		}
		p1 = NULL; p2 = NULL;
	}
	//Save and Update in FLASH the memory Context
	MIC_Flash_Memory_Write((const uint8_t *) &Context, sizeof(Context));
	return decodenumbertags;
}

/**
 * @Description: This function set low level the unused microcontroller I/O
 */
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

	GPIO_InitStruct.Pin = GPIO_PB8 | GPIO_PB10 | GPIO_PB11 | GPIO_PB12 | GPIO_PB14| GPIO_PB15;
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

void RTC_Save (void)
{
	char RTC_string[20];
	char temporal[10];

	MIC_Get_RTC (&hrtc, &structTimeRTC, &structDateRTC, RTC_FORMAT_BIN);

	if(structDateRTC.Date < 10) strcpy(RTC_string, "0");
	strcpy(RTC_string, itoa(structDateRTC.Date, temporal, 10));
	strcat(RTC_string, "/");
	if(structDateRTC.Month < 10) strcat(RTC_string, "0");
	strcat(RTC_string, itoa(structDateRTC.Month, temporal, 10));
	strcat(RTC_string, "/");
	strcat(RTC_string, itoa(((structDateRTC.Year)+2000), temporal, 10));
	strcat(RTC_string, "-");
	if(structTimeRTC.Hours < 10) strcat(RTC_string, "0");
	strcat(RTC_string, itoa(structTimeRTC.Hours, temporal, 10));
	strcat(RTC_string, ":");
	if(structTimeRTC.Minutes < 10) strcat(RTC_string, "0");
	strcat(RTC_string, itoa(structTimeRTC.Minutes, temporal, 10));
	strcat(RTC_string, ":");
	if(structTimeRTC.Seconds < 10) strcat(RTC_string, "0");
	strcat(RTC_string, itoa(structTimeRTC.Seconds, temporal, 10));

	strcpy(Context.Time_server, RTC_string);
}

