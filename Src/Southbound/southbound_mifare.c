/*
 * southbound_generic.c
 *
 *  Created on: 17 ago. 2017
 *      Author: BARCELONA2
 */

#include <southbound_mifare.h>
#include "ssd1306.h"
#include "fonts.h"


void LCD_Init(void)
{
	ssd1306_Init();
	HAL_Delay(1000);
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

void LCD_Write_String(char *string)
{
	ssd1306_WriteString(string, Font_7x10, Black);
	ssd1306_UpdateScreen();
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
