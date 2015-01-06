#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "espconn.h"

#include "../esp_kdapi.h"

uint32_t ticks = 0;

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static void loop(os_event_t *events);

//Main code function
static void ICACHE_FLASH_ATTR
loop(os_event_t *events)
{
	uart_process();
	wifiProcess();
	commProcess();

	os_delay_us(10000);
	ticks += 10;

	system_os_post(user_procTaskPrio, 0, 0);
}

//Init function
void ICACHE_FLASH_ATTR
user_init()
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_delay_us(100000);

	wifiInit();
	
	//Start os task
	system_os_task(loop, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
	
	system_os_post(user_procTaskPrio, 0, 0);
}

void commProcessCommand(unsigned char cmd, const unsigned char* buffer, int len)
{
	// char text[100];
	// os_sprintf(text, "cmd 0x%02x len: %d\r\n", cmd, len);
	// uart0_sendStr(text);

	switch(cmd)
	{
	case ESP_CMD_READ_STATUS:
		wifiSendStatus();
		break;
	case ESP_CMD_READ_FRAME:
		wifiSendFrame();
		break;
	case ESP_CMD_SEND_FRAME:
		wifiTransmitFrame(buffer, len);
		break;
	}
}
