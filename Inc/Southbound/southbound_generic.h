/*
 * southbound_generic.h
 *
 *  Created on: 21 ago. 2017
 *      Author: BARCELONA2
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



#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SOUTHBOUND_SOUTHBOUND_GENERIC_H_ */
