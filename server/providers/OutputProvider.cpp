#include "OutputProvider.h"

#include "kdutils.h"

OutputProvider::OutputProvider(EthernetDevice* device, int amount)
	: m_device(device), m_hasInitialStates(false)
{
	for (int i = 0; i < amount; i++)
		m_outputs.push_back(0);
	m_lastUpdateTime = 0;
}

void OutputProvider::processData(const void* buffer, int len)
{
}
void OutputProvider::process()
{
	if (!m_hasInitialStates)
	{
		m_listener->onInitialStatesRequest(this);
		m_hasInitialStates = true;
	}
	if (getTicks() - m_lastUpdateTime >= 2000)
		update();
}

void OutputProvider::setOutputState(int num, bool on)
{
	if (on == m_outputs[num])
		return;
	m_outputs[num] = on;
	update();
}
void OutputProvider::toggleOutputState(int num)
{
	m_outputs[num] = !m_outputs[num];
	update();
}

void OutputProvider::update()
{
	// TSrvOutputPacket p;
	// ByteBuffer data;
	// prepareCommand(data, 0x00);
	
	TSrvOutputSetOutputsPacket p;
	preparePacket((TSrvHeader*)&p, OUTPUT_CMD_SET_OUTPUTS);
	
	p.cnt = getAmount();
	p.outputs = 0;
	
	for (int i = 0; i < p.cnt; i++)
	{
		if (getOutputState(i))
			p.outputs |= (1 << i);
	}
	logInfo(str(Format("Sending output provider packet {} {}") << (uint64_t)this << p.outputs));
	// printf("out: 0x%02x\n", (unsigned int)b);
	
	sendData(&p, sizeof(p));
	m_lastUpdateTime = getTicks();
}
void OutputProvider::preparePacket(TSrvHeader* packet, uint8_t command)
{
	m_device->preparePacket(packet);
	packet->type = getType();
	packet->cmd = command;
}

string OutputProvider::getOutputID(int num)
{
	return str(Format("{}-{}") << getDevice()->getName() << num);
}

void OutputProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[{} - output] {}") << m_device->getName() << msg);
}
