/*
 * Definitions.h
 *
 *  Created on: 5 sept. 2017
 *      Author: FLG
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define MAX_LEN 16

//#define SOFTWARE_EMULATED    /// uncomment this for use same pin for SPI in emulated way
//#define TIM_MAJERLE		/// Uncomment this for use library from Tim Majerle, but it does not work
							/// for selecting tag and authentication.
#define CLOCK_26MHZ_APB113MHZ //Main clock to 26MHz, APB1 13MHZ for driving hardware SPI


#define PIN_spiClock GPIO_PIN_3
#define PORT_spiClock GPIOB
#define PIN_spiTX    GPIO_PIN_5
#define PIN_spiRX    GPIO_PIN_4
#define PORT_spiTX GPIOB
#define PORT_spiRX GPIOB

#define TIMEOUT_TX 		100 		//Timeout for UART transmission data
#define TIMEOUT_RX 		500			//Timeout for UART reception data
#define RETRIES 		2			//Retries number for transmit AT commands

#define TIMEDELAY_RESET 10000		//Delay for WIFI device reset

#define NTP_PACKET_SIZE 48
#define NTP_SEVENTYYEARS 2208988800UL
#define SIZE_BUFFER_RECEPTION  4000

#define NTP_BUFFERRECEPTION_SIZE 48
#define NTP_TIME_SIZE 20
#define HTTP_ARRAY_SIZE 800
#define HTTP_METHOD_GET "GET"
//#define HTTP_METHOD_GET_SIZE 3
#define HTTP_METHOD_POST "POST"
#define HTTP_HEADER_DESTINATION "/registro/index.php/?solicitud="
//#define HTTP_HEADER_DESTINATION_SIZE 37
#define HTTP_HEADER_PROTOCOL "HTTP/1.1"
//#define HTTP_HEADER_PROTOCOL_SIZE 8
#define HTTP_HOST "Host: "
//#define HTTP_HOST_SIZE 6
#define CARRIAGE_RETURN "\r"
#define NEWLINE "\n"

#define MIN_BUFFERRECEPTION_SIZE 	436
#define MAX_BUFFERRECEPTION_SIZE	588
#define TIMING_TIMEOUT_UART 5// cada 10 segundos máximo

#define BLOCKTRAIL_RFID	63
#define BLOCKREAD_RFID	62

#define RTC_CLOCK_SOURCE_LSI
//#define RTC_CLOCK_SOURCE_LSE  //Se usa el externo 32768Hz.
//#define RTC_CLOCK_SOURCE_HSE  //Se usa el externo 32768Hz.

#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0x137
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif

#ifdef RTC_CLOCK_SOURCE_HSE
#define RTC_ASYNCH_PREDIV  100
#define RTC_SYNCH_PREDIV   8125
#endif

#endif /* DEFINITIONS_H_ */
