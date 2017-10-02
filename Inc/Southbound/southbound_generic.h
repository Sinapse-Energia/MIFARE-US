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
#include "fonts.h"
#include "tm_stm32f4_mfrc522.h"

extern UART_HandleTypeDef huart1;
extern char messageRX[SIZE_BUFFER_RECEPTION];
extern char bufferReception[SIZE_BUFFER_RECEPTION];
extern uint8_t  data;
extern uint16_t BufferReceptionCounter;
extern IWDG_HandleTypeDef hiwdg;
extern uint8_t timeoutUART;

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


typedef enum {
	LED_Status_Pin = 0, Buzzer_Pin = 1, ES0_UART0 = 2, ES1_UART0 = 3
}GPIO_Pin_Select;

typedef enum{
	GET = 0, POST = 1
}HTTP_METHOD;

HTTP_METHOD method;

typedef enum {
	UART0_to_ETH = 0
}HK_Working_Mode;

typedef enum {
	TCP = 0, UDP = 1
}Network_Mode;

typedef enum {
	HK_OK = 0, HK_UART_FAIL = -1
} HKStatus;
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*LCD functions definitions*/
void LCD_Set_Parameters(void);
void LCD_Write_String(char *string, FontDef sizefont);



void MIC_Set_Digital_Output_status(GPIO_Pin_Select pin, PIN_Status status);

uint8_t MIC_UART_Send_Data(UART_HandleTypeDef *huart, unsigned char* messageTX, uint16_t lengthOfmessage, uint32_t timeoutTX);
uint8_t MIC_UART_Get_Data(UART_HandleTypeDef *huart, char* messageRX, uint8_t Size);



HKStatus HK_Set_Config (HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, char *messageRX);

HKStatus HK_Connect(HK_Working_Mode mode, Network_Mode netmode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, char *messageRX);

HKStatus HK_Get_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart1, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, char *messageRX);

uint8_t sendingATCommands(UART_HandleTypeDef *phuart1, uint32_t timeoutTx,
		uint32_t timeoutRx, uint32_t numberOfReceivedBytes,
		unsigned char *messageTX, char *messageRX);

typedef enum {
	TCP_OK = 0, TCP_FAIL = 1
} TCPStatus;

typedef enum {
	NTP_OK = 0, NTP_FAIL = 1
} NTPStatus;

TCPStatus TCP_Connect(HK_Working_Mode mode, Network_Mode netmode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, char *messageRX);

TCPStatus TCP_Set_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries,
		uint32_t timeoutTx, uint32_t timeoutRx, char *messageRX);

TCPStatus TCP_Get_Config(HK_Working_Mode mode, UART_HandleTypeDef *phuart, uint32_t retries, uint32_t timeoutTx,
		uint32_t timeoutRx, char *messageRX);

int RFID_Read_Memory_Block(int blockTrail, int blockRead, unsigned char *buffer);
void Get_NTP_Time(char *buffer);
uint8_t NTP_Sync(void);
char *Build_HTTP_msg(HTTP_METHOD method, char *Payload);

uint8_t HTTP_request1(char *HTTPbuffer);
uint8_t HTTP_request2(char *HTTPbuffer);
//***********************************************************************************************************
// DESCRIPTION: Functions that Set & Get internal RTC time and date. This function receive as parameters
// the RTC handle and RTC date & time structs.

void MIC_Set_RTC (RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime,RTC_DateTypeDef *sDate, uint32_t Format);
void MIC_Get_RTC(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime,RTC_DateTypeDef *sDate, uint32_t Format);


#ifdef FIRST_SPI_IMPLEMENTATION
/// SPI functions.

typedef void (*onSPIError)(void);

void SPIx_Error(SPI_HandleTypeDef *hspi );

/*
 * Function pointers for structure
*/
typedef uint8_t (*type_spi_write_read)( uint8_t byte );
typedef void (*type_spi_write)( uint8_t value);
typedef uint8_t (*type_spi_read)(uint8_t readSize);
typedef void (*type_spi_initialize)( SPI_HandleTypeDef *hspi, onSPIError funcSPIErrorCallback );

/*
 *	Actual functions that read/write to the SPI
*/
uint8_t _spi_write_read( uint8_t byte );
void _spi_write( uint8_t value);
uint8_t _spi_read(uint8_t readSize);
void _spi_initialize( SPI_HandleTypeDef *hspi, onSPIError funcSPIErrorCallback );

/*
 * Helper structure to call SPI in object like fashion
 */
struct _spiControl {
	SPI_HandleTypeDef *SPI_Handle;
	type_spi_initialize initialize;
	type_spi_write_read writeRead;
	type_spi_write write;
	type_spi_read read;
	onSPIError funcSPIErrorCallback;

}  ;

extern struct _spiControl spiControl;
#endif



void MIC_SPI_Write( SPI_HandleTypeDef *hspi1,uint8_t value);
uint8_t MIC_SPI_Read(SPI_HandleTypeDef *hspi1,uint8_t readSize);
uint8_t MIC_SPI_TransmitReceive( SPI_HandleTypeDef *hspi1,uint8_t byte );



#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_GENERIC_H_ */
