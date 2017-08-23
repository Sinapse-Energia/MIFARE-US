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

