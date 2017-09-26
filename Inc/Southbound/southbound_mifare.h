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
	Reading = 0, Registered = 1, Not_Registered =2, Normal = 3, Initialization = 4, RTC_display = 5
}Device_Status;


typedef struct{

	char IP_server[30];
	char IP[15];
	char ClientType[5];
	uint32_t Local_Time;
	char Time_server[20];
	char Building[20];
	char Classroom_Type[5];
	char Classroom[10];
	char Serie[8];

}Memory_Context;

Memory_Context Context;

/*typedef struct{

	char tag1[13]; //<horfeus>
	char tag2[11]; //<firma>
	char tag3[10]; //<aula>
	char tag4[11]; //<aulas>
	char tag5[8]; //<IP>
	char tag6[15]; //<resultado
	char tag7[17]; //<dispositivo>
	char tag8[11]; //<lista>
	char tag9[12]; //<codigo>
	char tag10[14]; //<edificio>
	char tag11[10]; //<tipo>
	char tag12[12]; //<nombre>
	char tag13[17]; //<TipoCliente>
	char tag14[12]; //<tiempo>
	char tag15[14]; //<TipoAula>
	char tag16[11]; //<serie>

}Start_TAGS;*/
typedef struct{

	char tag1[9]; //<horfeus>
	char tag2[7]; //<firma>
	char tag3[6]; //<aula>
	char tag4[7]; //<aulas>
	char tag5[4]; //<IP>
	char tag6[11]; //<resultado>
	char tag7[13]; //<dispositivo>
	char tag8[7]; //<lista>
	char tag9[8]; //<codigo>
	char tag10[10]; //<edificio>
	char tag11[6]; //<tipo>
	char tag12[8]; //<nombre>
	char tag13[13]; //<TipoCliente>
	char tag14[8]; //<tiempo>
	char tag15[10]; //<TipoAula>
	char tag16[7]; //<serie>

}Start_TAGS;

/*typedef struct{

	char tag1[16]; //</horfeus>
	char tag2[14]; //</firma>
	char tag3[13]; //</aula>
	char tag4[14]; //</aulas>
	char tag5[11]; //</IP>
	char tag6[18]; //</resultado
	char tag7[20]; //</dispositivo>
	char tag8[14]; //</lista>
	char tag9[15]; //</codigo>
	char tag10[17]; //</edificio>
	char tag11[13]; //</tipo>
	char tag12[15]; //</nombre>
	char tag13[20]; //</TipoCliente>
	char tag14[15]; //</tiempo>
	char tag15[17]; //</TipoAula>
	char tag16[14]; //</serie>

}End_TAGS;*/
typedef struct{

	char tag1[10]; //</horfeus>
	char tag2[8]; //</firma>
	char tag3[7]; //</aula>
	char tag4[8]; //</aulas>
	char tag5[5]; //</IP>
	char tag6[12]; //</resultado>
	char tag7[14]; //</dispositivo>
	char tag8[8]; //</lista>
	char tag9[9]; //</codigo>
	char tag10[11]; //</edificio>
	char tag11[7]; //</tipo>
	char tag12[9]; //</nombre>
	char tag13[14]; //</TipoCliente>
	char tag14[9]; //</tiempo>
	char tag15[11]; //</TipoAula>
	char tag16[8]; //</serie>

}End_TAGS;

typedef enum{
	GET = 0, POST = 1
}HTTP_METHOD;

HTTP_METHOD method;


void FillTags();
void encode_GET_msg(char *GET_msg);

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
	short_beep_1 = 1, short_beep_2 = 2, long_beep = 3
}Buzzer_tone;

void Buzzer_Control(Buzzer_tone tone);
void PWM_signal_simulator(void);
void Blink_LED_Status(Device_Status status);

void CleanBufferReception(void);

void Get_NTP_Time(unsigned char *buffer);
char *Buil_HTTP_msg(char *Payload, HTTP_METHOD method);

uint8_t HTTP_request(char *HTTPbuffer);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_MIFARE_H_ */
