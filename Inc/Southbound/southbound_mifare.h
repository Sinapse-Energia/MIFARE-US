/*
 * southbound_generic.h
 *
 *  Created on: 17 ago. 2017
 *      Author: BARCELONA2
 */

#ifndef SOUTHBOUND_SOUTHBOUND_MIFARE_H_
#define SOUTHBOUND_SOUTHBOUND_MIFARE_H_


typedef enum {
	Reading = 0, Registered = 1, Not_Registered =2, Normal = 3
}Device_Status;

void LCD_Init(void);

void LCD_Write_String(Device_Status status);

#endif /* SOUTHBOUND_SOUTHBOUND_MIFARE_H_ */
