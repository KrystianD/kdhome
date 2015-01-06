#include "provider_temp.h"

#include "providers.h"
#include <kdhome.h>
#include "providers_settings.h"

#include <string.h>

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

#if TEMP_SENSORS_COUNT > 16
#error Maximum number of temperature sensors is 16
#endif

#pragma pack(1)
struct
{
	union
	{
		struct
		{
			int16_t integral;
			uint16_t frac;
		} spl;
		float value;
	} value;
} prov_tempData[TEMP_SENSORS_COUNT];
#pragma pack()

uint16_t prov_tempErrors;
uint8_t prov_tempRealSensorsCount = 0;

void provTemp_sendData();

void provTempReset()
{
	memset(&prov_tempData, 0, sizeof(prov_tempData));
	prov_tempErrors = 0;
}
void provTempRegister()
{
	PROVIDER_DEBUG("REGISTER TEMP\r\n");
	TProvTempRegisterPacket p;
	provPrepareHeader((TProvHeader*)&p);
	p.header.type = PROVIDER_TYPE_TEMP;
	p.header.cmd = TEMP_CMD_REGISTER;
	p.cnt = prov_tempRealSensorsCount;
	provSendPacket(&p, sizeof(p));
}
void provTempProcess(const void* data, int len)
{
}
void provTempTmr()
{
	static uint32_t lastSendTime = 0;
	if (prov_tempRealSensorsCount > 0 && getTicks() - lastSendTime >= 1000 / prov_tempRealSensorsCount)
	{
		lastSendTime = getTicks();
		
		provTemp_sendData();
	}
}

void provTemp_sendData()
{
	static const uint8_t modes[] = TEMP_SENSORS_MODES;
	static uint8_t num = 0;
	TProvTempValuePacket p;
	provPrepareHeader((TProvHeader*)&p);
	
	p.header.type = PROVIDER_TYPE_TEMP;
	p.header.cmd = TEMP_NOTF_TEMP;
	p.cnt = prov_tempRealSensorsCount;
	p.num = num;
	p.flags = modes[num];
	p.value.value = prov_tempData[num].value.value;
	
	provSendPacket(&p, sizeof(p));
	
	num++;
	if (num >= prov_tempRealSensorsCount)
		num = 0;
}

void provTempSetRealSensorsCount(int cnt)
{
	if (cnt > TEMP_SENSORS_COUNT)
		cnt = TEMP_SENSORS_COUNT;
	prov_tempRealSensorsCount = cnt;
}
void provTempSetValueIntFrac(int num, int16_t integral, uint16_t frac)
{
	prov_tempData[num].value.spl.integral = integral;
	prov_tempData[num].value.spl.frac = frac;
	prov_tempErrors &= ~(1 << num);
}
void provTempSetValueFloat(int num, float value)
{
	prov_tempData[num].value.value = value;
	prov_tempErrors &= ~(1 << num);
}
void provTempSetError(int num)
{
	prov_tempErrors |= 1 << num;
}
