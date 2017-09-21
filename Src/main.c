/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "southbound_mifare.h"
#include "tm_stm32f4_mfrc522.h"
#include "main.h"
#include "stm32f0xx_hal.h"
#include "ssd1306.h"
#include "fonts.h"
#include "southbound_generic.h"
#include "stdio.h"
#include "Definitions.h"

#include "string.h"



/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

IWDG_HandleTypeDef hiwdg;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart5;

RTC_HandleTypeDef hrtc;
RTC_DateTypeDef structDateRTC;
RTC_TimeTypeDef structTimeRTC;
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART5_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#define WIFI_UART_HANDLE huart1
#define RFID_UART_HANDLE huart5
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t WDT_ENABLED=0;
uint8_t i = 0;

uint8_t UartRFID = 0;
uint8_t addressRFID,dataRFID;
unsigned char bufferReception[SIZE_BUFFER_HTTP];
uint8_t  data = 0;
uint16_t BufferReceptionCounter = 0;
unsigned char messageRX[SIZE_BUFFER_HTTP];
uint8_t timeout = 0;
HKStatus HK_Status;

char dataReceived[8];

unsigned char NTPpacket[NTP_PACKET_SIZE];
char *bufferNTP = NULL;

/* USER CODE END 0 */


char *IP_Device = "192.168.1.165";
char *IP_Mask = "255.255.255.0";
char *IP_Gateway = "192.168.1.1";
char *IP_Dns = "192.168.1.1";

char *NTP_Server_Domain = "213.251.52.234";
char *NTP_Server_Port = "123";
char *TCP_Server_Domain = "192.168.1.164";
char *TCP_Server_Port = "8000";
char *IP_Local_Port = "8080";

#pragma data_alignment = 4

int main(void)


{

  /* USER CODE BEGIN 1 */

	//Fill NTPpacket buffer with corrects parameters
	memset(NTPpacket, 0, NTP_PACKET_SIZE);

	NTPpacket[0]= 0b11100011; NTPpacket[1]= 0; NTPpacket[2]= 6; NTPpacket[3]= 0xEC;
	NTPpacket[12]= 49; NTPpacket[13]= 0x4E; NTPpacket[14]= 49; NTPpacket[15]= 52;

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  if (WDT_ENABLED==1)
  {
	  	  MX_IWDG_Init();
		__HAL_IWDG_START(&hiwdg); //no se inicializar watchdog, se deshabilita para debug
		  HAL_IWDG_Refresh(&hiwdg);
  }
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  //MX_SPI1_Init();

  //Initialize SPI control
  //	spiControl.initialize( &hspi1, MX_SPI1_Init );

  TM_MFRC522_Init();
  //MX_USART5_UART_Init();


  /// test NFC
  LCD_Init();
  uint8_t value = 0;
  while (0)
  {

	   //value= TM_MFRC522_ReadRegister(0x37);
	  MFRC522_CS_LOW;
	  value = SPI_transfer_byte(((0x37 << 1) & 0x7E)|0x80);
	  MFRC522_CS_HIGH;

	  int a=0;
	  a=a+1;
  }



  if (1) // To check with real card.
    {

 	   uint8_t CardID[5];
 	   //uint8_t MyID[5]=  { 0x43, 0xdc, 0x52, 0xb6, 0x7b };// My card on my keys
 	   uint8_t MyID[5]=  { 123, 192, 122, 199};// It seems that it is 'Domingo' identifier

 	   TM_MFRC522_Status_t statusRFID;


 	   while (1) /// example with infinite loop
 	   {
 		  /// If any card detected
 		   //TM_MFRC522_ClearBitMask(MFRC522_REG_STATUS2,0x08); // comprobar
 		   if (TM_MFRC522_Check(CardID) == MI_OK)
 		   { // CardID is valid

 		      /// Check if this is my card
 			   char here=1;
 			   unsigned char sectorKey[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
 			   uint8_t ok= TM_MFRC522_SelectTag(CardID);
 			   uint8_t j=63;
 			   //while (j<64)
 			   {

 				   statusRFID = TM_MFRC522_Auth(0x60, j, sectorKey, CardID);
 				   if (statusRFID==MI_OK)
 				   {
 					   TM_MFRC522_Read(j, dataReceived);
 					  TM_MFRC522_ClearBitMask(MFRC522_REG_STATUS2, 0x08);
 				    }


 			   }


 			   //if (statusRFID==MI_OK)
 			   //{

 				 //  statusRFID=TM_MFRC522_Read(62, dataReceived);
 				 //  TM_MFRC522_ClearBitMask(MFRC522_REG_STATUS2, 0x08);

 			   //}



 		   }

 		   else
 		   {
 			   // Card not detected.

 		   }


 	   }




    }


   /* USER CODE BEGIN 2 */





     /*Initialize, Set LCD Display config  and show status message*/
     LCD_Init();


     /*Read Context parameters from FLASH*/
     //MIC_Flash_Memory_Read((const uint8_t *) &Context, sizeof(Context));
    // HK_Status = HK_Set_Config(0, &huart1, 2, 100, 500, messageRX);
   // HAL_Delay(2000);
    // HK_Status = HK_Connect(0, &huart1, 2, 100, 500, messageRX);
     //HAL_Delay(2000);
     //HK_Status = HK_Get_Config(0, &huart1, 2, 100, 500, messageRX);
     //if(HK_Status == 0)
    // LCD_Write_mifare_info(4);
     /*NTP Synchronization: Read time from server and set into uC RTC*/
     //TCP_IP_Connect();
     //TCP_IP_Get_Data()
     //MIC_Set_RTC();

     /*Update Context: Read Flash Memory and update context if necessary */
     //MIC_Flash_Memory_Read();
     //MIC_Flash_Memory_Write();
    // HK_Set_Connection_Settings();

     //LCD_Write_mifare_info();
    /* HAL_GPIO_WritePin(ES0_GPIO_Port, ES0_Pin, GPIO_PIN_RESET);
    HAL_Delay(2500);
    HAL_GPIO_WritePin(ES0_GPIO_Port, ES0_Pin, GPIO_PIN_SET);
     HAL_Delay(2000);*/
     MIC_Set_Digital_Output_status(2,0);
       			HAL_Delay(2500);
       			MIC_Set_Digital_Output_status(2,1);
       			HAL_Delay(1000);
     sendingATCommands(&huart1, 100, 500,14, (uint8_t *)"at+SAtMode0=0\r\n", messageRX);
    /* sendingATCommands(&huart1, 100, 500,10, (uint8_t *)"at+Save=1\r\n", messageRX);
     sendingATCommands(&huart1, 100, 500,11, (uint8_t *)"at+Apply=1\r\n", messageRX);
     sendingATCommands(&huart1, 100, 500,0, (uint8_t *)"at+Reboot=1\r\n", messageRX);*/
     CleanBufferReception();
     //HAL_Delay(4000);

     //sendingATCommands(&huart1, 100, 500,14, (uint8_t *)"at+SAtMode0=?\r\n", messageRX);
     //LCD_Write_mifare_info(4);
    // MIC_UART_Send_Data(&huart5,(uint8_t*),12,100);

     /*MIC_Set_Digital_Output_status(2,0);
     			HAL_Delay(2500);
     			MIC_Set_Digital_Output_status(2,1);
     			HAL_Delay(1000);*/
     //MIC_UART_Send_Data(&huart1,(uint8_t*)"GET / HTTP/1.1\r\nHost: 192.168.1.164\r\n\r\n",39,100);
    //sendingATCommands(&huart1,100, 500, 100,(uint8_t*)"GET /index.htm HTTP/1.1\r\nHost: 192.168.1.164\r\n\r\n",messageRX);

     if (0) /// Testing getting data. This code part should be included when some RFID card is detected
     {

		CleanBufferReception();
		//while(1)
		while(BufferReceptionCounter != 96)
		{
			HAL_Delay(500);
			MIC_UART_Get_Data(&huart1, &data, 1);

			//MIC_UART_Send_Data(&huart1,(uint8_t*)"GET /index.htm HTTP/1.1\r\nHost: 192.168.1.164\r\n\r\n",48,100);
			//MIC_UART_Send_Data(&huart1,(uint8_t*)"GET /es/use.html HTTP/1.1\r\nHost: pool.ntp.org\r\n\r\n",49,100);
			//MIC_UART_Send_Data(&huart1,(uint8_t*)&packet,48,100);
			MIC_UART_Send_Data(&huart1,(uint8_t*)&NTPpacket,48,100);
			HAL_Delay(500);
			//while (BufferReceptionCounter < 50)
			//		;
			if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);
			if (BufferReceptionCounter>0)
			{
				HAL_Delay(4000);

				Get_NTP_Time(bufferReception);
				HAL_Delay(100);
				//CleanBufferReception();

			}
			if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);
		}
	}


      /* USER CODE END 2 */

     /* Infinite loop */
     /* USER CODE BEGIN WHILE */
     while (1)
    {
     /* USER CODE END WHILE */
   	  if (WDT_ENABLED == 1)	HAL_IWDG_Refresh(&hiwdg);
   	  /*Waiting for UART Interrupt*/
   	  //MIC_UART_Send_Data(&huart1,(uint8_t*)"GET / HTTP/1.1\r\nHost: 192.168.1.164\r\n\r\n",36,100);
   	  //MIC_Get_RTC (&hrtc, &structTimeRTC, &structDateRTC, RTC_FORMAT_BIN);
   	  //strcpy(bufferNTP, structTimeRTC.Hours);
   	  LCD_Write_mifare_info(4);
   	  // if (flag_interrupt == 1)
   	  //{
   	  	  /*Reading MIFARE card: block 3, sector 16, key FFFFFFFFFFFF*/
   	  	  //RFID_Read_Memory_Block();

   	  	  // if(RFID_OK)
   		  //{
   	  	  	  /*Beep & Update LCD*/
   	  	  	  //Blink_LED_Status(Reading);
   	  	  	  //Buzzer_Control();

   	  	  	  /*Create the HTTP message, connect to HTTP server and send HTTP msg*/
   	  	  	  //TCP_IP_Connect();
   	  	  	  //TCP_Send_Data();

   	  	  	  /*Wait for server response until timeout*/
   	  	  	  // if(response_OK)
   	  	  	  //{
   	  	  	  	  /*Beep & Update LCD*/
   	  	  	  	  //Blink_LED_Status(Reading);
   	  	  	  	  //Buzzer_Control();
   	  	  	  // else (response_NOK)
   	  	  	  // {
   				  	  /*Beep & Update LCD*/
   	  	  	  	  	  //Blink_LED_Status(Reading);
   	  	  	  	  	  //Buzzer_Control();
   	  	  	  // }
   	  	  //}
   	  	  //else (RFID_NOK)
   	  	  //{
   	  	  	  	  /*Beep & Update LCD*/
   				  //Blink_LED_Status(Reading);
   				  //Buzzer_Control();
   	  	  //}
   	  //}
   	  // else (flag_interrupt != 1)
   	  //{
   	  	  	  /*Update LCD Display*/
   		  	  //LCD_Write_mifare_info();
   	  //}

   	  //LED_STATUS pin test
   	  //HAL_UART_Transmit(&huart5,(const char*)"Testing RM08S module\r\n",22,100);
   	 //HAL_UART_Transmit(&huart1,(uint8_t*)"at+Netmode=1\r",13,100);
   	 // HAL_UART_Transmit(&huart1,(uint8_t*)"at+Save=1\r\n",11,100);
   	  //HAL_UART_Transmit(&huart1,(uint8_t*)"at+Apply=1\r\n",12,100);
   	 // HAL_UART_Transmit(&huart1,(uint8_t*)"at+Reboot=1\r\n",13,100);




     /* USER CODE BEGIN 3 */

     }
     /* USER CODE END 3 */

   }

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  //PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  //PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  //PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
 if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00200C2C;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* IWDG init function */
static void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM6 init function */
static void MX_TIM6_Init(void)
{

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 33474;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 25000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM7 init function */
static void MX_TIM7_Init(void)
{

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 33474;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 25000;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  //hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; //100khz
  //hspi1.Init.BaudRatePrescaler =SPI_BAUDRATEPRESCALER_32;
  //hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;  ///1600khz
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;







  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}


/* USART5 init function */
static void MX_USART5_UART_Init(void)
{

  huart5.Instance = USART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  //HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
  //HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
  /*Configure GPIO pin : CSS_Pin */
    GPIO_InitStruct.Pin = CSS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH ;
    HAL_GPIO_Init(CSS_GPIO_Port, &GPIO_InitStruct);



  /* configure output pins MIFARE RFID output */
  GPIO_InitStruct.Pin =  GPIO_PIN_3|GPIO_PIN_5 ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin MIFARE RFID input */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin | ES0_Pin | ES1_Pin ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIOX13_Pin */
    //GPIO_InitStruct.Pin = LED_STATUS_Pin;
    //GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    //GPIO_InitStruct.Pull = GPIO_NOPULL;
    //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    //HAL_GPIO_Init(LED_STATUS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : GPIOX13_Pin */
	GPIO_InitStruct.Pin = PORST_Pin | PERST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* RTC init function */
void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  RTC_AlarmTypeDef sAlarm;

    /**Initialize RTC and set the Time and Date
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  //hrtc.Init.AsynchPrediv = 127;
 // hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if(HAL_RTC_Init(&hrtc) != HAL_OK)
     {
	  _Error_Handler(__FILE__, __LINE__);
     }

  //HAL_RTC_Init(&hrtc);

   /*sTime.Hours = 0x11;
   sTime.Minutes = 0x0;
   sTime.Seconds = 0x0;*/
   sTime.TimeFormat = RTC_HOURFORMAT12_AM;
   sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
   sTime.StoreOperation = RTC_STOREOPERATION_RESET;
   //HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
   if(HAL_RTC_SetTime(&hrtc,&sTime, RTC_FORMAT_BCD) != HAL_OK)
   {
	   _Error_Handler(__FILE__, __LINE__);
   }

   /*sDate.WeekDay = RTC_WEEKDAY_MONDAY;
   sDate.Month = RTC_MONTH_JANUARY;
   sDate.Date = 0x1;
   sDate.Year = 0x0;*/
   if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
      {
	   _Error_Handler(__FILE__, __LINE__);
      }
   //HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

     /**Enable the Alarm A
     */
   sAlarm.AlarmTime.Hours = 0x0;
   sAlarm.AlarmTime.Minutes = 0x0;
   sAlarm.AlarmTime.Seconds = 0x0;
   sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
   sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
   sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
   sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
   sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
   sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
   sAlarm.AlarmDateWeekDay = 0x1;
   sAlarm.Alarm = RTC_ALARM_A;

   if(HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
        {
	   _Error_Handler(__FILE__, __LINE__);
        }
   //HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BIN);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if (huart->Instance==WIFI_UART_HANDLE.Instance)
 {

	  //USART_ClearITPendingBit(USARTx, USART_IT_TC);
	  //	USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
	  //UartRFID =1;

	 bufferReception[BufferReceptionCounter]=data;
	 BufferReceptionCounter=((BufferReceptionCounter+1)%SIZE_BUFFER_HTTP);
	HAL_UART_Receive_IT(huart,&data,1);

	 //packet[BufferReceptionCounter]=data;
	// 	 BufferReceptionCounter=((BufferReceptionCounter+1)%48);
	 //	 HAL_UART_Receive_IT(huart,&data,1);
  }

}
/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart1)
{
  //Disable_All_INT();

  if (huart1->Instance==WIFI_UART_HANDLE->Instance)
  {
	  UartRFID =1;
  }
  //Enable_All_INT();

}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->ErrorCode == HAL_UART_ERROR_ORE)
	{
	   //remove the error condition
		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}



#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
