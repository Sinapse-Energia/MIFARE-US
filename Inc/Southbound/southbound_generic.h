/*
 * southbound_generic.h
 *
 *  Created on: 21 ago. 2017
 *      Author: FLG
 */

#ifndef SOUTHBOUND_SOUTHBOUND_GENERIC_H_
#define SOUTHBOUND_SOUTHBOUND_GENERIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"

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
	LED_Status_Pin = 0, Buzzer_Pin = 1
}GPIO_Pin_Select;

void MIC_Set_Digital_Output_status(GPIO_Pin_Select pin, PIN_Status status);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_GENERIC_H_ */
