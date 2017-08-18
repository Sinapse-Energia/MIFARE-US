/*
 * southbound_generic.h
 *
 *  Created on: 17 ago. 2017
 *      Author: BARCELONA2
 */

#ifndef SOUTHBOUND_SOUTHBOUND_MIFARE_H_
#define SOUTHBOUND_SOUTHBOUND_MIFARE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"

typedef enum {
	Reading = 0, Registered = 1, Not_Registered =2, Normal = 3
}Device_Status;

void LCD_Init(void);
void LCD_Display_Update(void);
void LCD_SetCursor(uint8_t x, uint8_t y);

void LCD_Write_mifare_info(Device_Status status);

void LCD_Write_String(char *string);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_MIFARE_H_ */
