#include "EthernetInputProvider.h"

#include "common.h"
#include "kutils.h"
#include <unistd.h>

EthernetInputProvider::EthernetInputProvider(EthernetDevice* device, int amount)
	: m_device(device), m_listener(0), m_lastUpdateTime(0)
{
	for (int i = 0; i < amount; i++)
		m_inputs.push_back(TInput());
}

void EthernetInputProvider::init()
{
	m_hasFirstData = 0;
	for (int i = 0; i < getAmount(); i++)
		m_inputs[i] = TInput();

	ByteBuffer data;
	prepareCommand(data, 0x01);
	sendData(data);
}
void EthernetInputProvider::processData(ByteBuffer& buffer)
{
	uint8_t cmd;
	if (!buffer.fetch(cmd)) return;

	switch(cmd)
	{
	case 0x00:
		{
			uint8_t cnt;
			if (!buffer.fetch(cnt)) return;
			for (int i = 0; i < cnt; i++)
			{
				uint8_t low, high;
				if (!buffer.fetch(low)) return;
				if (!buffer.fetch(high)) return;

				TInput &inp = m_inputs[i];

				// printf("%d - %d %d\n", i, low, high);
				while (inp.low != low || inp.high != high)
				{
					if (inp.low != low)
					{
						inp.low++;
						if (m_hasFirstData)
						{
							if (m_listener) m_listener->onInputChanged(this, i, 0);
						}
					}
					if (inp.high != high)
					{
						inp.high++;
						if (m_hasFirstData)
						{
							if (m_listener) m_listener->onInputChanged(this, i, 1);
						}
					}
				}
				// printf("test: %d - %d %d\n", i, inp.low, inp.high);
			}
			m_hasFirstData = true;
		}
		break;
	}
}
void EthernetInputProvider::process()
{
}

void EthernetInputProvider::update()
{
}
void EthernetInputProvider::prepareCommand(ByteBuffer& buffer, uint8_t command)
{
	m_device->prepareBuffer(buffer);
	buffer.append(getType());
	buffer.append(command);
}

void EthernetInputProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[input] {}") << msg);
}
