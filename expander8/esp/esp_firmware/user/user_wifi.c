/*
 * user_wifi.c
 * Copyright (C) 2015 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#include "user_wifi.h"

#include "user_interface.h"
#include "driver/uart.h"
#include "espconn.h"
#include "os_type.h"
#include "osapi.h"

#include "user_comm.h"
#include "../esp_kdapi.h"

struct espconn espConn;
esp_udp espUdp;
struct espconn * pespConn;
int cn = 0;

#define WIFI_STATE_NOT_CONNECTED 0
#define WIFI_STATE_CONNECTING    1
#define WIFI_STATE_CONNECTED     2
static int state = WIFI_STATE_NOT_CONNECTED;

static void createSocket();
static void at_tcpclient_recv(void *arg, char *pdata, unsigned short len);
static void at_tcpclient_sent_cb(void *arg);

void wifiInit()
{
	char ssid[32] = SSID;
	char password[64] = SSID_PASSWORD;
	struct station_config stationConf;
	
	//Set station mode
	wifi_set_opmode(0x1);
	
	//Set ap settings
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 64);
	wifi_station_set_config(&stationConf);
	
	state = WIFI_STATE_CONNECTING;
}

/*enum {
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP
};*/

void wifiProcess()
{
	switch (state)
	{
	case WIFI_STATE_NOT_CONNECTED:
		break;
	case WIFI_STATE_CONNECTING:
		if ((wifi_station_get_connect_status() == STATION_GOT_IP))
		{
			state = WIFI_STATE_CONNECTED;
			createSocket();
		}
		break;
	case WIFI_STATE_CONNECTED:
		break;
	}
}

static void createSocket()
{
	char temp[64];
	//  enum espconn_type linkType;
	int8_t len;
	enum espconn_type linkType = ESPCONN_INVALID;
	uint32_t ip = 0;
	char ipTemp[128] = "192.168.2.2";
	int32_t port = 9999;
	uint8_t linkID;
	
	pespConn = &espConn;//(struct espconn *)mem_zalloc(sizeof(struct espconn));
	if (pespConn == NULL)
	{
		uart0_sendStr("CONNECT FAIL\r\n");
		return;
	}
	pespConn->type = ESPCONN_UDP;
	pespConn->state = ESPCONN_NONE;
	ip = ipaddr_addr(ipTemp);
	
	pespConn->proto.udp = (esp_udp *)&espUdp;//(esp_udp *)mem_zalloc(sizeof(esp_udp));
	pespConn->proto.udp->local_port = port;//espconn_port();
	pespConn->proto.udp->remote_port = port;
	os_memcpy(pespConn->proto.udp->remote_ip, &ip, 4);
	
	os_printf("socket port %d\r\n", pespConn->proto.udp->local_port); ///
	
	espconn_regist_recvcb(pespConn, at_tcpclient_recv);
	espconn_regist_sentcb(pespConn, at_tcpclient_sent_cb);
	
	espconn_create(pespConn);
}

volatile int rxAvail = 0;
volatile int txAvail = 1;
char rxData[1024];

static void ICACHE_FLASH_ATTR
at_tcpclient_recv(void *arg, char *pdata, unsigned short len)
{
	struct espconn *pespconn = (struct espconn *)arg;
	
	if (rxAvail == 0)
	{
		// uart0_sendStr("new data\r\n");
		os_memcpy(rxData, pdata, len);
		rxAvail = len;
	}
	else
	{
		// uart0_sendStr("no space\r\n");
	}
	// char temp[32];
	// temp[0] = 0x7e;
	// temp[1] =
	// os_sprintf(temp, "\r\nDATA,%d:", len);
	// uart0_sendStr(temp);
	// uart0_tx_buffer(pdata, len);
}
static void ICACHE_FLASH_ATTR
at_tcpclient_sent_cb(void *arg)
{
	txAvail = 1;
}

void wifiSendStatus()
{
	unsigned char buf[10];
	buf[0] = state;
	buf[1] = wifi_station_get_connect_status();
	buf[2] = rxAvail > 0 ? 1 : 0;
	buf[3] = txAvail;
	commSendPacket(ESP_CMD_READ_STATUS, buf, 4);
}
void wifiSendFrame()
{
	commSendPacket(ESP_CMD_READ_FRAME, rxData, rxAvail);
	rxAvail = 0;
}
void wifiTransmitFrame(const unsigned char* buffer, int len)
{
	// if (txAvail == 0)
		// return;
	espconn_sent(pespConn, (unsigned char*)buffer, len);
	txAvail = 0;
}
