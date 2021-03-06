#include "TempProvider.h"

#include "common.h"
#include "kdutils.h"
#include <unistd.h>

TempProvider::TempProvider(EthernetDevice* device, int amount)
	: m_device(device)//, m_listener(0)//, m_lastUpdateTime(0)
{
	for (int i = 0; i < amount; i++)
		m_sensors.push_back(TSensor());
}

void TempProvider::init()
{
	// m_hasFirstData = 0;
	for (int i = 0; i < getAmount(); i++)
		m_sensors[i] = TSensor();
		
	// ByteBuffer data;
	// prepareCommand(data, 0x01);
	// sendData(data);
}
void TempProvider::processData(const void* buffer, int len)
{
	TProvHeader *header = (TProvHeader*)buffer;
	
	switch (header->cmd)
	{
	case TEMP_NOTF_TEMP:
	{
		TProvTempValuePacket *p = (TProvTempValuePacket*)buffer;
		
		if (p->num >= m_sensors.size())
			break;
			
		TSensor &sensor = m_sensors[p->num];
		if (p->flags & 1) // float
		{
			sensor.value = p->value.value;
		}
		else // int frac
		{
			sensor.value = (float)(((int32_t)p->value.spl.integral << 16) | p->value.spl.frac) / (float)0x10000;
		}
		
		// sensor.error = (errors & (1 << i)) ? 1 : 0;
		
		m_lastDataTime = getTicks();
	}
	break;
	}
}
void TempProvider::process()
{
}

void TempProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[temp] {}") << msg);
}
