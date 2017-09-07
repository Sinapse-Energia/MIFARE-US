/**	
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
#include "mfrc522.h"



void spi_readwrite(char value)
{
//register unsigned char temp;



char temp = value;


for (int bitCount= 0; bitCount< 8; bitCount++)
 {
  if(temp& 0x80) HAL_GPIO_WritePin(MFRC522_MOSI_PORT, MFRC522_MOSI_PIN,GPIO_PIN_SET);
  else HAL_GPIO_WritePin(MFRC522_MOSI_PORT, MFRC522_MOSI_PIN,GPIO_PIN_RESET);
 //runNops(1);
 //asm volatile("nop");
 HAL_GPIO_WritePin(MFRC522_CLK_PORT, MFRC522_CLK_PIN,GPIO_PIN_SET);
 temp <<= 1;
 //runNops(1);
 //asm volatile("nop");
 HAL_GPIO_WritePin(MFRC522_CLK_PORT, MFRC522_CLK_PIN,GPIO_PIN_RESET);
 }
}

char spi_read(void)
{

	//register unsigned char temp=0;
	char temp=0;
	 for (int bitCount= 0; bitCount<8; bitCount++)
	 {
	     temp <<= 1;
	     HAL_GPIO_WritePin(MFRC522_CLK_PORT, MFRC522_CLK_PIN,GPIO_PIN_RESET);
	   //  asm volatile("nop");
	     temp = temp | HAL_GPIO_ReadPin(MFRC522_MISO_PORT, MFRC522_MISO_PIN);
	   //  asm volatile("nop");
	     HAL_GPIO_WritePin(MFRC522_CLK_PORT, MFRC522_CLK_PIN,GPIO_PIN_SET);
	   //  asm volatile("nop");
	 }
	 HAL_GPIO_WritePin(MFRC522_CLK_PORT, MFRC522_CLK_PIN,GPIO_PIN_RESET);
	// asm volatile("nop");
	 return temp;

}

char readRegister(char address)
{
	char value;
	MFRC522_SELECT();
	spi_readwrite(address);
	value=spi_read();
	MFRC522_UNSELECT();

	return value;

}
