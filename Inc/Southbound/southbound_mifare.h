/*
 * southbound_generic.h
 *
 *  Created on: 17 ago. 2017
 *      Author: FLG
 */

#ifndef SOUTHBOUND_SOUTHBOUND_MIFARE_H_
#define SOUTHBOUND_SOUTHBOUND_MIFARE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"
#include "string.h"

extern UART_HandleTypeDef huart1;
extern unsigned char messageRX[100];
extern unsigned char bufferReception[100];
extern uint8_t  data;
extern uint16_t BufferReceptionCounter;
extern IWDG_HandleTypeDef hiwdg;
extern uint8_t timeout;

typedef enum {
	Reading = 0, Registered = 1, Not_Registered =2, Normal = 3, Init_OK = 4
}Device_Status;

typedef enum {
	HLK_OK = 0, HLK_UART_FAIL = -1
} HLKStatus;

typedef struct{

	unsigned char IP_server[30];
	unsigned char IP[15];
	unsigned char ClientType[5];
	uint32_t Local_Time;
	unsigned char Time_server;
	unsigned char Building[20];
	uint8_t Classroom_Type;
	uint8_t Classroom;
	uint8_t Serie;

}Memory_Context;

Memory_Context Context;

void LCD_Init(void);
void LCD_Display_Update(void);
void LCD_SetCursor(uint8_t x, uint8_t y);
void LCD_Write_mifare_info(Device_Status status);
void LCD_Write_String(char *string);


typedef enum {
	short_beep_1 = 0, short_beep_2 = 1, long_beep = 3
}Buzzer_tone;

void Buzzer_Control(Buzzer_tone tone);
void PWM_signal_simulator(void);
void Blink_LED_Status(Device_Status status);
uint8_t sendingATCommands(UART_HandleTypeDef *phuart1, uint32_t timeoutTx,
		uint32_t timeoutRx, uint32_t numberOfReceivedBytes,
		unsigned char *messageTX, unsigned char *messageRX);
void CleanBufferReception(void);
HLKStatus HK_Set_Config (UART_HandleTypeDef *phuart1, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_MIFARE_H_ */
