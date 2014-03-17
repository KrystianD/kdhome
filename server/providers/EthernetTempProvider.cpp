#include "EthernetTempProvider.h"

#include "common.h"
#include "kutils.h"
#include <unistd.h>

EthernetTempProvider::EthernetTempProvider(Device* device, int amount)
	: m_device(device)//, m_listener(0)//, m_lastUpdateTime(0)
{
	for (int i = 0; i < amount; i++)
		m_sensors.push_back(TSensor());
}

void EthernetTempProvider::init()
{
	// m_hasFirstData = 0;
	for (int i = 0; i < getAmount(); i++)
		m_sensors[i] = TSensor();

	// ByteBuffer data;
	// prepareCommand(data, 0x01);
	// sendData(data);
}
void EthernetTempProvider::processData(ByteBuffer& buffer)
{
	uint8_t cmd;
	if (!buffer.fetch(cmd)) return;

	switch(cmd)
	{
	case TEMP_NOTF_TEMP:
		{
			uint8_t cnt;
			if (!buffer.fetch(cnt)) return;

			uint16_t flags;
			if (!buffer.fetch(flags)) return;

			uint16_t errors;
			if (!buffer.fetch(errors)) return;

			for (int i = 0; i < cnt; i++)
			{
				TSensor &sensor = m_sensors[i];
				if (flags & (1 << i)) // float
				{
					float value;
					if (!buffer.fetch(value)) return;

					sensor.value = value;
				}
				else // int frac
				{
					int16_t integral;
					uint16_t frac;
					if (!buffer.fetch(integral)) return;
					if (!buffer.fetch(frac)) return;

					sensor.value = (float)(((int32_t)integral << 16) | frac) / (float)0x10000;
				}

				sensor.error = (errors & (1 << i)) ? 1 : 0;

				m_lastDataTime = getTicks();
			}
		}
		break;
	}
}
void EthernetTempProvider::process()
{
}

void EthernetTempProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[temp] {}") << msg);
}
