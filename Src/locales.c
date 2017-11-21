/*
 * locales.c
 *
 *  Created on: 13 sept. 2017
 *      Author: FLG
 */
#define TEST
//#define MIFARE_US

#ifdef TEST

	char *IP_Device = "192.168.1.166";
	char *IP_Mask = "255.255.255.0";
	char *IP_Gateway = "192.168.1.1";
	char *IP_Dns = "192.168.1.1";

	char *NTP_Server_Domain = "213.251.52.234";
	char *NTP_Server_Port = "123";
	char *TCP_Server_Domain = "192.168.1.52";
	char *TCP_Server_Port = "1880";
	char *IP_Local_Port = "8080";

#endif

#ifdef MIFARE_US

	char *IP_Device = "10.5.6.187";
	char *IP_Mask = "255.255.0.0";
	char *IP_Gateway = "10.5.0.1";
	char *IP_Dns = "150.214.186.69";

	char *NTP_Server_Domain = "150.214.230.64";
	char *NTP_Server_Port = "123";
	char *TCP_Server_Domain = "150.214.230.64";
	char *TCP_Server_Port = "80";
	char *IP_Local_Port = "8080";

#endif

