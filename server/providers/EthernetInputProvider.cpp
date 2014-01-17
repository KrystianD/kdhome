#include "EthernetInputProvider.h"

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
	buffer.fetch(cmd);

	switch(cmd)
	{
	case 0x00:
		{
			uint8_t cnt;
			buffer.fetch(cnt);
			for (int i = 0; i < cnt; i++)
			{
				uint8_t low, high;
				buffer.fetch(low);
				buffer.fetch(high);

				TInput &inp = m_inputs[i];

				// printf("%d - %d %d\n", i, low, high);
				while (inp.low != low || inp.high != high)
				{
					if (inp.low != low)
					{
						inp.low++;
						if (m_hasFirstData && m_listener) m_listener->onInputChanged(this, i, 0);
					}
					if (inp.high != high)
					{
						inp.high++;
						if (m_hasFirstData && m_listener) m_listener->onInputChanged(this, i, 1);
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
	// if (getTicks() - m_lastUpdateTime >= 2000)
		// update();
}

void EthernetInputProvider::update()
{
	// ByteBuffer data;
	// prepareCommand(data, 0x00);

	// uint8_t cnt = getInputsAmount();
	// data.append(cnt);
	// uint8_t b = 0;
	// int idx = 0;
	// for (int i = 0; i < cnt; i++)
	// {
		// if (getInputState(i))
			// b |= (1 << (7 - idx));

		// idx++;
		// if (idx == 8)
		// {
			// idx = 0;
			// data.append(b);
			// b = 0;
		// }
	// }
	// if (idx)
		// data.append(b);

	// printf("out: 0x%02x\n", (unsigned int)b);

	// sendData(data);
	// m_lastUpdateTime = getTicks();
}
void EthernetInputProvider::prepareCommand(ByteBuffer& buffer, uint8_t command)
{
	m_device->prepareBuffer(buffer);
	buffer.append(getType());
	buffer.append(command);
}
