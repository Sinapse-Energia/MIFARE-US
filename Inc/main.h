/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define txMicro_Pin GPIO_PIN_9
#define txMicro_GPIO_Port GPIOA
#define rxMicro_Pin GPIO_PIN_10
#define rxMicro_GPIO_Port GPIOA
#define txRFID_Pin GPIO_PIN_3
#define txRFID_GPIO_Port GPIOB
#define rxRFID_Pin GPIO_PIN_4
#define rxRFID_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define CSS_Pin GPIO_PIN_13
#define CSS_GPIO_Port GPIOB

#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB
#define LED_STATUS_Pin GPIO_PIN_15
#define LED_STATUS_GPIO_Port GPIOA

#define ES0_Pin GPIO_PIN_1
#define ES0_GPIO_Port GPIOB
#define ES1_Pin GPIO_PIN_0
#define ES1_GPIO_Port GPIOB
//#define PERST_Pin GPIO_PIN_14
//#define PERST_GPIO_Port GPIOC
//#define PORST_Pin GPIO_PIN_13
//#define PORST_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

//Unused GPIO's

#define GPIO_PA0 GPIO_PIN_0
#define GPIO_PA0_GPIO_Port GPIOA
#define GPIO_PA1 GPIO_PIN_1
#define GPIO_PA1_GPIO_Port GPIOA
#define GPIO_PA2 GPIO_PIN_2
#define GPIO_PA2_GPIO_Port GPIOA
#define GPIO_PA3 GPIO_PIN_3
#define GPIO_PA3_GPIO_Port GPIOA
#define GPIO_PA4 GPIO_PIN_4
#define GPIO_PA4_GPIO_Port GPIOA
#define GPIO_PA5 GPIO_PIN_5
#define GPIO_PA5_GPIO_Port GPIOA
#define GPIO_PA6 GPIO_PIN_6
#define GPIO_PA6_GPIO_Port GPIOA
#define GPIO_PA7 GPIO_PIN_7
#define GPIO_PA7_GPIO_Port GPIOA
#define GPIO_PA8 GPIO_PIN_8
#define GPIO_PA8_GPIO_Port GPIOA
#define GPIO_PA11 GPIO_PIN_11
#define GPIO_PA11_GPIO_Port GPIOA
#define GPIO_PA12 GPIO_PIN_12
#define GPIO_PA12_GPIO_Port GPIOA

#define GPIO_PB8 GPIO_PIN_8
#define GPIO_PB8_GPIO_Port GPIOB
#define GPIO_PB10 GPIO_PIN_10
#define GPIO_PB10_GPIO_Port GPIOB
#define GPIO_PB11 GPIO_PIN_12
#define GPIO_PB11_GPIO_Port GPIOB
#define GPIO_PB12 GPIO_PIN_12
#define GPIO_PB12_GPIO_Port GPIOB
#define GPIO_PB14 GPIO_PIN_14
#define GPIO_PB14_GPIO_Port GPIOB
#define GPIO_PB15 GPIO_PIN_15
#define GPIO_PB15_GPIO_Port GPIOB

#define GPIO_PC15 GPIO_PIN_15
#define GPIO_PC15_GPIO_Port GPIOC
/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
