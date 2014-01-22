#include <public.h>
#include <myprintf.h>
#include <ow.h>

#include "temp.h"

enum ETempStatus tempStatus;
int8_t tempCurrent;
uint8_t tempCurrentFraq;
uint32_t tempLastRead;

uint32_t tempNextReadTime;

void tempSetupNextRead ()
{
	uint8_t res;
	
	OW_UART_set9600 ();
	res = OW_UART_resetPulse ();
	if (res == 0)
	{
		tempStatus = TEMP_NOSENSOR;
		return;
	}
	OW_UART_set115200 ();
	OW_UART_writeByte (0xcc);
	OW_UART_writeByte (0x4e);
	OW_UART_writeByte (0x00);
	OW_UART_writeByte (0x00);
	OW_UART_writeByte (0x1f | 0b01100000);
	
	OW_UART_set9600 ();
	res = OW_UART_resetPulse ();
	if (res == 0)
	{
		tempStatus = TEMP_NOSENSOR;
		return;
	}
	OW_UART_set115200 ();
	OW_UART_writeByte (0xcc);
	OW_UART_writeByte (0x44);
	
	tempStatus = TEMP_SENSORWAITFORREAD;
	tempNextReadTime = ticks + 1000;
}
void tempReadTemperature ()
{
	uint8_t i, res;
	
	OW_UART_set9600 ();
	res = OW_UART_resetPulse ();
	if (res == 0)
	{
		tempStatus = TEMP_NOSENSOR;
		return;
	}
	OW_UART_set115200 ();
	OW_UART_writeByte (0xcc);
	OW_UART_writeByte (0xbe);
	
	uint8_t r[8];
	uint8_t crc = 0;
	for (i = 0; i < 8; i++)
	{
		r[i] = OW_UART_readByte ();
		crc = OW_crc8_update (crc, r[i]);
	}
	uint8_t origcrc = OW_UART_readByte ();

	if (crc == origcrc)
	{
		tempCurrent = (r[0] >> 4) | ((r[1] & 0x0f) << 4);
		tempCurrentFraq = r[0] & 0x0f;
		tempLastRead = ticks;
		myprintf ("[temp] read %d fraq: %d\r\n", tempCurrent, tempCurrentFraq);
	}
	else
	{
		myprintf ("[temp] Invalid crc (%d != %d)\r\n", crc, origcrc);
	}
	
	tempSetupNextRead ();
}

void tempInit ()
{
	tempStatus = TEMP_NOSENSOR;
}
void tempProcess ()
{
	uint8_t res;
	
	switch (tempStatus)
	{
		case TEMP_NOSENSOR:
			//myprintf ("TEMP_NOSENSOR\r\n");
			
			OW_UART_set9600 ();
			res = OW_UART_resetPulse ();
			if (res == 1)
			{
				tempSetupNextRead ();
			}
			break;
			
		case TEMP_SENSORWAITFORREAD:
			//myprintf ("TEMP_SENSORWAITFORREAD\r\n");
			
			if (ticks >= tempNextReadTime)
			{
				tempReadTemperature ();
			}
			break;
	}
}
