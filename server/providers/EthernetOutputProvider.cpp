#include "EthernetOutputProvider.h"

#include "kutils.h"

EthernetOutputProvider::EthernetOutputProvider(EthernetDevice* device, int amount)
	: m_device(device)
{
	for (int i = 0; i < amount; i++)
		m_outputs.push_back(0);
	m_lastUpdateTime = 0;
}

void EthernetOutputProvider::processData(ByteBuffer& buffer)
{
}
void EthernetOutputProvider::process()
{
	// setOutputState(0, (getTicks() % 1000) < 500);

	if (getTicks() - m_lastUpdateTime >= 2000)
		update();
}

void EthernetOutputProvider::setOutputState(int num, bool on)
{
	if (on == m_outputs[num])
		return;
	m_outputs[num] = on;
	update();
}
void EthernetOutputProvider::toggleOutputState(int num)
{
	m_outputs[num] = !m_outputs[num];
	update();
}

void EthernetOutputProvider::update()
{
	ByteBuffer data;
	prepareCommand(data, 0x00);

	uint8_t cnt = getAmount();
	data.append(cnt);
	uint8_t b = 0;
	int idx = 0;
	for (int i = 0; i < cnt; i++)
	{
		if (getOutputState(i))
			b |= (1 << (7 - idx));

		idx++;
		if (idx == 8)
		{
			idx = 0;
			data.append(b);
			b = 0;
		}
	}
	if (idx)
		data.append(b);

	// printf("out: 0x%02x\n", (unsigned int)b);

	sendData(data);
	m_lastUpdateTime = getTicks();
}
void EthernetOutputProvider::prepareCommand(ByteBuffer& buffer, uint8_t command)
{
	m_device->prepareBuffer(buffer);
	buffer.append(getType());
	buffer.append(command);
}
