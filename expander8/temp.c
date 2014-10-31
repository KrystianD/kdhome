#include <public.h>
#include <myprintf.h>
#include <delay.h>
#include <ow.h>

#include "temp.h"
#include "provider_temp.h"

enum ETempStatus tempStatus;
uint32_t tempLastRead;

uint32_t tempNextReadTime;

uint8_t tempRoms[8][8];
uint8_t tempSensorsCnt, tempSensorIdx;

void tempSetupNextRead()
{
	uint8_t res;
	int i;
	
	myprintf("[temp] convert t %d\r\n", tempSensorIdx);
	
	OW_UART_set9600();
	res = OW_UART_resetPulse();
	if (res == 0)
	{
		tempStatus = TEMP_NOSENSOR;
		return;
	}
	OW_UART_set115200();
	OW_UART_writeByte(OW_MATCH_ROM);
	for (i = 0; i < 8; i++)
		OW_UART_writeByte(tempRoms[tempSensorIdx][i]);
	OW_UART_writeByte(OW_WRITE_SCRATCHPAD);
	OW_UART_writeByte(0x00);
	OW_UART_writeByte(0x00);
	OW_UART_writeByte(0x1f | 0b01100000);
	
	OW_UART_set9600();
	res = OW_UART_resetPulse();
	if (res == 0)
	{
		tempStatus = TEMP_NOSENSOR;
		return;
	}
	OW_UART_set115200();
	OW_UART_writeByte(OW_MATCH_ROM);
	for (i = 0; i < 8; i++)
		OW_UART_writeByte(tempRoms[tempSensorIdx][i]);
	OW_UART_writeByte(OW_CONVERT_T);
	
	tempStatus = TEMP_SENSORWAITFORREAD;
	tempNextReadTime = ticks + 1000;
}
void tempReadTemperature()
{
	uint8_t i, res;
	
	OW_UART_set9600();
	res = OW_UART_resetPulse();
	if (res == 0)
	{
		tempStatus = TEMP_NOSENSOR;
		return;
	}
	OW_UART_set115200();
	OW_UART_writeByte(OW_MATCH_ROM);
	for (i = 0; i < 8; i++)
		OW_UART_writeByte(tempRoms[tempSensorIdx][i]);
	OW_UART_writeByte(OW_READ_SCRATCHPAD);
		
	uint8_t r[8];
	uint8_t crc = 0;
	for (i = 0; i < 8; i++)
	{
		r[i] = OW_UART_readByte();
		crc = OW_crc8_update(crc, r[i]);
	}
	uint8_t origcrc = OW_UART_readByte();
	
	if (crc == origcrc)
	{
		int8_t	tempCurrent = (r[0] >> 4) | ((r[1] & 0x0f) << 4);
		uint8_t tempCurrentFraq = r[0] & 0x0f;
		tempLastRead = ticks;
		myprintf("[temp] read %d fraq: %d\r\n", tempCurrent, tempCurrentFraq);
		provTempSetValueIntFrac(tempSensorIdx, tempCurrent, (uint16_t)tempCurrentFraq << 12);
	}
	else
	{
		myprintf("[temp] Invalid crc (%d != %d)\r\n", crc, origcrc);
	}
	
	
	tempSensorIdx++;
	if (tempSensorIdx == tempSensorsCnt)
		tempSensorIdx = 0;

	tempSetupNextRead();
}

void tempInit()
{
	tempStatus = TEMP_NOSENSOR;
	
	tempSensorsCnt = 0;
	tempSensorIdx = 0;
	
	struct TOWUARTSearchBuffer buf;
	
	OW_UART_romSearchInit(&buf);
	uint8_t res;
	while (tempSensorsCnt < 8)
	{
		res = OW_UART_romSearch(&buf);
		if (res == 0)
		{
			memcpy(&tempRoms[tempSensorsCnt], buf.rom, 8);
			tempSensorsCnt++;
		}
		else if (res == 2)
		{
			memcpy(&tempRoms[tempSensorsCnt], buf.rom, 8);
			tempSensorsCnt++;
			break;
		}
	}
	
	myprintf("[temp] found %d sensors\r\n", tempSensorsCnt);
	int i, j;
	for (i = 0; i < tempSensorsCnt; i++)
	{
		for (j = 0; j < 8; j++)
			myprintf("%02x|", tempRoms[i][j]);
		myprintf("\r\n");
	}

	provTempSetRealSensorsCount(tempSensorsCnt);
}
void tempProcess()
{
	uint8_t res;
	
	switch (tempStatus)
	{
	case TEMP_NOSENSOR:
		//myprintf ("TEMP_NOSENSOR\r\n");
		
		OW_UART_set9600();
		res = OW_UART_resetPulse();
		if (res == 1)
		{
			tempSetupNextRead();
		}
		break;
		
	case TEMP_SENSORWAITFORREAD:
		//myprintf ("TEMP_SENSORWAITFORREAD\r\n");
		
		if (ticks >= tempNextReadTime)
		{
			tempReadTemperature();
		}
		break;
	}
}
