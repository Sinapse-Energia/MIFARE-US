/*
 * southbound_generic.h
 *
 *  Created on: 21 ago. 2017
 *      Author: FLG
 */

#ifndef SOUTHBOUND_GENERIC_H_
#define SOUTHBOUND_GENERIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"
#include "string.h"
#include "Definitions.h"


extern UART_HandleTypeDef huart1;
extern unsigned char messageRX[SIZE_BUFFER_HTTP];
extern unsigned char bufferReception[SIZE_BUFFER_HTTP];
extern uint8_t  data;
extern uint16_t BufferReceptionCounter;
extern IWDG_HandleTypeDef hiwdg;
extern uint8_t timeout;

//// Flash init Address
#define ADDR_FLASH_PAGE_127    ((uint32_t)0x0803f800) /* last flash sector 2k */
////////////////////////////////////////////////////////////////////////////////////////////////////
/* FLASH functions definitios */
HAL_StatusTypeDef FlashNVM_ErasePage(uint32_t page);
HAL_StatusTypeDef FlashNVM_Read(uint32_t start_address, uint8_t* data_out, uint32_t size);
HAL_StatusTypeDef FlashNVM_Write(uint32_t start_address, const uint8_t* data_in, uint32_t size);

int MIC_Flash_Memory_Write(const uint8_t *data_in, uint32_t size);
int MIC_Flash_Memory_Read(const uint8_t *data_out, uint32_t size);
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {
	OFF = 0, ON = 1
}PIN_Status;


/////////////////////////////////////////////////////////////////////////////////////////////////////
/*LCD functions definitions*/
void LCD_Set_Parameters(void);
void LCD_Write_String(char *string);


typedef enum {
	LED_Status_Pin = 0, Buzzer_Pin = 1, ES0_UART0 = 2
}GPIO_Pin_Select;

void MIC_Set_Digital_Output_status(GPIO_Pin_Select pin, PIN_Status status);

uint8_t MIC_UART_Send_Data(UART_HandleTypeDef *huart, unsigned char* messageTX, uint8_t lengthOfmessage, uint32_t timeoutTX);
uint8_t MIC_UART_Get_Data(UART_HandleTypeDef *huart, unsigned char* messageRX, uint8_t Size);

typedef enum {
	UART0_to_ETH = 0
}HK_Working_Mode;

typedef enum {
	TCP = 0, UDP = 1
}HK_Network_Mode;

typedef enum {
	HK_OK = 0, HK_UART_FAIL = -1
} HKStatus;

HKStatus HK_Set_Config (HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX);

HKStatus HK_Connect(HK_Working_Mode mode, HK_Network_Mode netmode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX);

HKStatus HK_Get_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart1, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, unsigned char *messageRX);

uint8_t sendingATCommands(UART_HandleTypeDef *phuart1, uint32_t timeoutTx,
		uint32_t timeoutRx, uint32_t numberOfReceivedBytes,
		unsigned char *messageTX, unsigned char *messageRX);

//***********************************************************************************************************
// DESCRIPTION: Functions that Set & Get internal RTC time and date. This function receive as parameters
// the RTC handle and RTC date & time structs.

void MIC_Set_RTC (RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime,RTC_DateTypeDef *sDate, uint32_t Format);
void MIC_Get_RTC(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime,RTC_DateTypeDef *sDate, uint32_t Format);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_GENERIC_H_ */
