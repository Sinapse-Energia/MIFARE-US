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
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
}

void LCD_Write_String(Device_Status status)
{
	switch(status)
	{
		case Normal:
			ssd1306_Fill(White);
			ssd1306_UpdateScreen();
			ssd1306_SetCursor(10,23);
			ssd1306_WriteString("System OK",Font_7x10,Black);
			ssd1306_UpdateScreen();
			break;

		case Reading:
			ssd1306_Fill(White);
			ssd1306_UpdateScreen();
			ssd1306_SetCursor(10,23);
			ssd1306_WriteString("Reading Card",Font_7x10,Black);
			ssd1306_UpdateScreen();
			break;

		case Registered:
			ssd1306_Fill(White);
			ssd1306_UpdateScreen();
			ssd1306_SetCursor(10,23);
			ssd1306_WriteString("Registered",Font_7x10,Black);
			ssd1306_SetCursor(10,33);
			ssd1306_WriteString("in Server",Font_7x10,Black);
			ssd1306_UpdateScreen();
			break;

		case Not_Registered:
			ssd1306_Fill(White);
			ssd1306_UpdateScreen();
			ssd1306_SetCursor(10,23);
			ssd1306_WriteString("Not Registered",Font_7x10,Black);
			ssd1306_SetCursor(10,33);
			ssd1306_WriteString("in Server(Error)",Font_7x10,Black);
			ssd1306_UpdateScreen();
			break;
	}

}
