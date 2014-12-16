#include "provider_input.h"

#include "providers.h"
#include <kdhome.h>
#include "providers_settings.h"

#include <string.h>

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

uint8_t prov_inputLow[INPUTS_COUNT], prov_inputHigh[INPUTS_COUNT];

void provInputReset()
{
	memset(&prov_inputLow, 0, sizeof(prov_inputLow));
	memset(&prov_inputHigh, 0, sizeof(prov_inputHigh));
	provInputResetState();
}
void provInputRegister()
{
	PROVIDER_DEBUG("REGISTER INPUT\r\n");
	TProvInputRegisterPacket p;
	provPrepareHeader((TProvHeader*)&p);
	p.header.type = PROVIDER_TYPE_INPUT;
	p.header.cmd = INPUT_CMD_REGISTER;
	p.cnt = INPUTS_COUNT;
	provSendPacket(&p, sizeof(p));
}
void provInputProcess(const void* data, int len)
{
	TSrvHeader *header = (TSrvHeader*)data;

	switch (header->cmd)
	{
	default:
	case INPUT_NOTF_NEWSTATE:
		break;
	case INPUT_REQ_SENDSTATE:
		PROVIDER_DEBUG("INPUT REQUESET!!!\r\n");
		provInputSendState();
		break;
	}
}
void provInputTmr()
{
}
void provInputSetState(int num, int value)
{
	if (value)
		prov_inputHigh[num]++;
	else
		prov_inputLow[num]++;
		
	// int i; for (i=0;i<16;i++)
	// myprintf("inp %d - %d:%d\r\n", i, prov_inputLow[i], prov_inputHigh[i]);
}
void provInputSendState()
{
	TProvInputStatePacket p;
	provPrepareHeader((TProvHeader*)&p);
	
	p.header.type = PROVIDER_TYPE_INPUT;
	p.header.cmd = INPUT_NOTF_NEWSTATE;
	p.cnt = INPUTS_COUNT;
	
	int i;
	for (i = 0; i < INPUTS_COUNT; i++)
	{
		p.inputs[i].low = prov_inputLow[i];
		p.inputs[i].high = prov_inputHigh[i];
	}
	
	provSendPacket(&p, sizeof(p));
}
