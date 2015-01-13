#include "CounterProvider.h"

#include "common.h"
#include "kdutils.h"
#include <unistd.h>

#include <kdhome.h>

CounterProvider::CounterProvider(EthernetDevice* device, int amount)
	: m_device(device), m_listener(0), m_lastUpdateTime(0)
{
	for (int i = 0; i < amount; i++)
		m_counters.push_back(0);
}

void CounterProvider::init()
{
	m_hasFirstData = 0;
	for (int i = 0; i < getAmount(); i++)
		m_counters.push_back(0);
		
	TSrvHeader p;
	preparePacket((TSrvHeader*)&p, COUNTER_REQ_SENDSTATE);
	sendData(&p, sizeof(p));
}
void CounterProvider::processData(const void* buffer, int len)
{
	TProvHeader *header = (TProvHeader*)buffer;
	
	switch (header->cmd)
	{
	case COUNTER_NOTF_NEWSTATE:
	{
		TProvCounterStatePacket *p = (TProvCounterStatePacket*)buffer;
		for (int i = 0; i < p->cnt; i++)
		{
			uint32_t val = p->counters[i];
			
			if (val != m_counters[i])
			{
				m_counters[i] = val;
				
				if (m_hasFirstData)
				{
					if (m_listener) m_listener->onCounterChanged(this, getInputID(i), m_counters[i]);
				}
			}
			// uint8_t low, high;
			// low = p->inputs[i].low;
			// high = p->inputs[i].high;
			
			// TInput &inp = m_inputs[i];
			
			// // printf("%d - %d %d\n", i, low, high);
			// while (inp.low != low || inp.high != high)
			// {
			// if (inp.low != low)
			// {
			// inp.low++;
			// if (m_hasFirstData)
			// {
			// if (m_listener) m_listener->onInputChanged(this, getInputID(i), 0);
			// }
			// }
			// if (inp.high != high)
			// {
			// inp.high++;
			// if (m_hasFirstData)
			// {
			// if (m_listener) m_listener->onInputChanged(this, getInputID(i), 1);
			// }
			// }
			// }
			// printf("test: %d - %d %d\n", i, inp.low, inp.high);
		}
		m_hasFirstData = true;
	}
	break;
	}
}
void CounterProvider::process()
{
}

void CounterProvider::update()
{
}
void CounterProvider::preparePacket(TSrvHeader* packet, uint8_t command)
{
	m_device->preparePacket(packet);
	packet->type = getType();
	packet->cmd = command;
}

string CounterProvider::getInputID(int num)
{
	return str(Format("{}-{}") << getDevice()->getName() << num);
}

void CounterProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[counter] {}") << msg);
}
