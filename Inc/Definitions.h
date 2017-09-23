/*
 * Definitions.h
 *
 *  Created on: 5 sept. 2017
 *      Author: FLG
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_



#define NTP_PACKET_SIZE 48
#define NTP_SEVENTYYEARS 2208988800UL
#define SIZE_BUFFER_RECEPTION  4000

#define GET_MSG_SIZE 250
#define NTP_TIME_SIZE 20
#define HTTP_ARRAY_SIZE 400
#define HTTP_METHOD_GET "GET"
//#define HTTP_METHOD_GET_SIZE 3
#define HTTP_METHOD_POST "POST"
#define HTTP_HEADER_DESTINATION "/registro/index.php/simple?solicitud="
//#define HTTP_HEADER_DESTINATION_SIZE 37
#define HTTP_HEADER_PROTOCOL "HTTP/1.1"
//#define HTTP_HEADER_PROTOCOL_SIZE 8
#define HTTP_HOST "Host: "
//#define HTTP_HOST_SIZE 6
#define CARRIAGE_RETURN "\r"
#define NEWLINE "\n"




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
