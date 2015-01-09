#include "provider_input.h"

#include "providers.h"
#include <kdhome.h>
#include "providers_settings.h"

#include <string.h>

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

static uint32_t counters[COUNTERS_COUNT];

void provCounterReset()
{
	memset(&counters, 0, sizeof(counters));
	provCounterResetState();
}
void provCounterRegister()
{
	PROVIDER_DEBUG("REGISTER COUNTER\r\n");
	TProvCounterRegisterPacket p;
	provPrepareHeader((TProvHeader*)&p);
	p.header.type = PROVIDER_TYPE_COUNTER;
	p.header.cmd = COUNTER_CMD_REGISTER;
	p.cnt = COUNTERS_COUNT;
	provSendPacket(&p, sizeof(p));
}
void provCounterProcess(const void* data, int len)
{
	TSrvHeader *header = (TSrvHeader*)data;
	
	switch (header->cmd)
	{
	default:
	case COUNTER_NOTF_NEWSTATE:
		break;
	case COUNTER_REQ_SENDSTATE:
		PROVIDER_DEBUG("COUNTER REQUESET!!!\r\n");
		provCounterSendState();
		break;
	}
}
void provCounterTmr()
{
}
void provCounterSet(int num, int value)
{
	counters[num] = value;
}
void provCounterAdd(int num, int value)
{
	counters[num] += value;
}
void provCounterSendState()
{
	TProvCounterStatePacket p;
	provPrepareHeader((TProvHeader*)&p);
	
	p.header.type = PROVIDER_TYPE_COUNTER;
	p.header.cmd = COUNTER_NOTF_NEWSTATE;
	p.cnt = COUNTERS_COUNT;
	
	int i;
	for (i = 0; i < COUNTERS_COUNT; i++)
	{
		p.counters[i] = counters[i];
	}
	
	provSendPacket(&p, sizeof(p));
}
