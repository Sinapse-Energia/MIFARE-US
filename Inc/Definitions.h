/*
 * Definitions.h
 *
 *  Created on: 5 sept. 2017
 *      Author: FLG
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define PIN_spiClock GPIO_PIN_3					// Francis
#define PORT_spiClock GPIOB						// Francis
#define PIN_spiTX    GPIO_PIN_5
#define PIN_spiRX    GPIO_PIN_4
#define PORT_spiTX GPIOB
#define PORT_spiRX GPIOB


#define NTP_PACKET_SIZE 48
#define NTP_SEVENTYYEARS 2208988800UL
#define SIZE_BUFFER_HTTP  4000

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
