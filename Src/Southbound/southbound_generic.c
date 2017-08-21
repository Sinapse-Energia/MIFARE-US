/*
 * southbound_generic.c
 *
 *  Created on: 21 ago. 2017
 *      Author: BARCELONA2
 */

#include "southbound_generic.h"


int MIC_Flash_Memory_Write(const uint8_t *data_in, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t sizeReceived=0;

	status=FlashNVM_ErasePage(ADDR_FLASH_PAGE_127);  // It is erased last page in microcontroller;

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Write(ADDR_FLASH_PAGE_127+4, data_in, size); // data are written with +4 offset from starting. First 4 bytes are used to write size of data
																// and watching if there are data or not.
	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Write(ADDR_FLASH_PAGE_127, (uint8_t* )&size, 4); // quantity of data are saved at first 4 bytes.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Read(ADDR_FLASH_PAGE_127, (uint8_t* )&sizeReceived, 4); // quantity of data are read again to verify it wrote good.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	if (status==HAL_OK) return sizeReceived; // if all goes fine, it returns size of data.

}

int MIC_Flash_Memory_Read(const uint8_t *data_out, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t sizeReceived=0;

	status=FlashNVM_Read(ADDR_FLASH_PAGE_127+4, (uint8_t *)data_out, size); // start to read all data.
	status=FlashNVM_Read(ADDR_FLASH_PAGE_127+4, (uint8_t *)data_out, size); // start to read all data.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Read(ADDR_FLASH_PAGE_127, (uint8_t *)&sizeReceived, 4); // received quantity of saved bytes.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	if ((status==HAL_OK)&&(sizeReceived==size))
	{
		//CopyFlashMemorystruct_to_Localstruct();
		return sizeReceived; // if all goes fine, it returns size of data.
	}

}

///////////////// Flash functions.
HAL_StatusTypeDef FlashNVM_ErasePage(uint32_t page)
{

	HAL_StatusTypeDef status;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError = 0;

	HAL_FLASH_Unlock();
	//__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	//                   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );


	EraseInitStruct.PageAddress=page;
	EraseInitStruct.NbPages=1;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;

	status = HAL_BUSY;
	while (status == HAL_BUSY) {
		status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	}
	HAL_FLASH_Lock();

	return status;
}


/**
  * @brief  Read a binary array from FLASH
  * @param  address: FLASH relative address to read
  * @param  data_out: output data array pointer
  * @param  size: array length
  * @retval operation status
  */
HAL_StatusTypeDef FlashNVM_Read(uint32_t start_address, uint8_t* data_out, uint32_t size)
{
    uint32_t sizeCounter = 0;


	// Check input data
   // if (!IS_FLASH_ADDRESS(start_address)) {
        // It's not Flash's address
    //	return HAL_ERROR;
	//}

	while (sizeCounter < size) {
	    *data_out = (*(__IO uint8_t*)start_address);
	    data_out++;
	    start_address++;
	    sizeCounter++;
	}
    return HAL_OK;
}


/**
  * @brief  write data array to PREVIOSLY ERISED FLASH memory
  * @param  fl_bank: flash area (application or its copy bank)
  * @retval true if OK,  otherwise return false
  */
HAL_StatusTypeDef FlashNVM_Write(uint32_t start_address, const uint8_t* data_in, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t i;
	uint16_t *integerPointer;
	integerPointer = (uint16_t *)data_in;

	// Check input data
    //if (!IS_FLASH_ADDRESS(start_address)) {
        // It's not Flash's address
    //	return HAL_ERROR;
	//}

	HAL_FLASH_Unlock();
	//__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	//                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );


	// Write data
    for (i = 0; i < size/2; i+=1) {
    	status = HAL_BUSY;
    	while (status == HAL_BUSY) {
    		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, start_address + 2*i, *integerPointer);
    		integerPointer++;
    	}
    	if ( status != HAL_OK) {
    		break;
    	}
    }

	HAL_FLASH_Lock();

	return status;
}

