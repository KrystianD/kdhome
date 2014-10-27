#include "providers.h"
#include "providers_settings.h"
#include <myprintf.h>

#include "ethernet.h"
#include <kdhome.h>
#include <string.h>

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

void provTemp_sendData();

void provTempReset()
{
	memset(&prov_tempData, 0, sizeof(prov_tempData));
	prov_tempErrors = 0;
}
void provTempProcess(const void* data, int len)
{
	uint8_t cmd;
	// if (BYTEBUFFER_FETCH(data, cmd)) return;

	// switch (cmd)
	// {
	// default:
		// break;
	// }
}
void provTempTmr()
{
	static uint32_t lastSendTime = 0;
	if (ticks - lastSendTime >= 1000 / TEMP_SENSORS_COUNT)
	{
		lastSendTime = ticks;

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
	p.cnt = TEMP_SENSORS_COUNT;
	p.num = num;
	p.flags = modes[num];
	p.value.value = prov_tempData[num].value.value;
	
	provSendPacket(&p, sizeof(p));

	if (num >= TEMP_SENSORS_COUNT)
		num = 0;

	// TByteBuffer b;
	// if (!ethPrepareBuffer(&b, 2 + 1 + 1 + 2 + 2 + TEMP_SENSORS_COUNT * 4))
		// return;
	// uint16_t type = PROVIDER_TYPE_TEMP;
	// BYTEBUFFER_APPEND(&b, type);

	// uint8_t cmd = TEMP_NOTF_TEMP;
	// BYTEBUFFER_APPEND(&b, cmd);

	// uint8_t cnt = TEMP_SENSORS_COUNT;
	// BYTEBUFFER_APPEND(&b, cnt);

	// uint16_t flags = 
		// (TEMP_SENSOR0_MODE << 0) |
		// (TEMP_SENSOR1_MODE << 1) |
		// (TEMP_SENSOR2_MODE << 2) |
		// (TEMP_SENSOR3_MODE << 3) |
		// (TEMP_SENSOR4_MODE << 4) |
		// (TEMP_SENSOR5_MODE << 5) |
		// (TEMP_SENSOR6_MODE << 6) |
		// (TEMP_SENSOR7_MODE << 7) |
		// (TEMP_SENSOR8_MODE << 8) |
		// (TEMP_SENSOR9_MODE << 9) |
		// (TEMP_SENSOR10_MODE << 10) |
		// (TEMP_SENSOR11_MODE << 11) |
		// (TEMP_SENSOR12_MODE << 12) |
		// (TEMP_SENSOR13_MODE << 13) |
		// (TEMP_SENSOR14_MODE << 14) |
		// (TEMP_SENSOR15_MODE << 15);
	// BYTEBUFFER_APPEND(&b, flags);

	// uint16_t errors = prov_tempErrors;
	// BYTEBUFFER_APPEND(&b, errors);

	// int i;
	// for (i = 0; i < TEMP_SENSORS_COUNT; i++)
	// {
		// BYTEBUFFER_APPEND(&b, prov_tempData[i].value.value);
	// }

	// ethSendPacket(&b);

	// ethFreeBuffer(&b);
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
