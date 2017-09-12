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


typedef enum {
	Reading = 0, Registered = 1, Not_Registered =2, Normal = 3, Init_OK = 4
}Device_Status;


typedef struct{

	char IP_server[30];
	char IP[15];
	char ClientType[5];
	uint32_t Local_Time;
	unsigned char Time_server;
	char Building[20];
	uint8_t Classroom_Type;
	uint8_t Classroom;
	uint8_t Serie;

}Memory_Context;

Memory_Context Context;


typedef struct
{

  unsigned li   : 2;       // Only two bits. Leap indicator.
  unsigned vn   : 3;       // Only three bits. Version number of the protocol.
  unsigned mode : 3;       // Only three bits. Mode. Client will pick mode 3 for client.

  uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  uint8_t precision;       // Eight bits. Precision of the local clock.

  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.

  uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.

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

void CleanBufferReception(void);





#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_MIFARE_H_ */
