/**
 *  Defines for your entire project at one place
 * 
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@version 	v1.0
 *	@ide		Keil uVision 5
 *	@license	GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

/* Put your global defines for all libraries here used in your project */
//Default SPI used
#ifndef MFRC522_SPI
#define MFRC522_SPI						SPI1
//#define MFRC522_SPI_PINSPACK			TM_SPI_PinsPack_2
#endif

//Default CS pin used
#ifndef MFRC522_CS_PIN


#define MFRC522_CS_LOW \
    HAL_GPIO_WritePin(CSS_GPIO_Port, CSS_Pin, GPIO_PIN_RESET )

#define MFRC522_CS_HIGH \
	 HAL_GPIO_WritePin(CSS_GPIO_Port, CSS_Pin, GPIO_PIN_SET )
//#define MFRC522_CS_RCC					RCC_AHB1Periph_GPIOA

#define MFRC522_CS_PORT					GPIOA
#define MFRC522_CS_PIN					GPIO_PIN_15
#endif

#endif
