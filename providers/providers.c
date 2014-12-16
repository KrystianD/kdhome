#include "providers.h"

#include <kdhome.h>
#include "providers_settings.h"

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

// PRIVATE
uint16_t ethNextPacketPtr;
uint16_t ethSessKey = 0;
uint16_t ethPacketId = 1;

// ------------------------- IMPLEMENTATION ------------------------------

// PUBLIC
void provInit()
{
	ethSessKey = 0;
	ethPacketId = 1;
}
void provProcess(const char* data, int len)
{
	TSrvHeader *header = (TSrvHeader*)data;
	
	PROVIDER_DEBUG("type: 0x%04x cmd: %d\r\n", header->type, header->cmd);
	
	switch (header->type)
	{
	case PROVIDER_TYPE_CONTROL:
	{
		if (header->cmd == CONTROL_CMD_REGISTER)
		{
			TSrvCmdRegister *p = (TSrvCmdRegister*)data;
			ethSessKey = p->sessKey;
			PROVIDER_DEBUG("New sesskey: 0x%04x\r\n", ethSessKey);
			
			ethPacketId = 1;
			// reset providers
			provOutputReset();
			provInputReset();
#ifdef ENABLE_IR_PROVIDER
			provIRReset();
#endif
			provTempReset();

			// register providers
			provOutputRegister();
			provInputRegister();
			provIRRegister();
			provTempRegister();
		}
	}
	break;
	case PROVIDER_TYPE_OUTPUT:
		// myprintf("PROVIDER_TYPE_OUTPUT\r\n");
		provOutputProcess(data, len);
		break;
	case PROVIDER_TYPE_INPUT:
		// myprintf("PROVIDER_TYPE_INPUT\r\n");
		provInputProcess(data, len);
		break;
	case PROVIDER_TYPE_IR:
		// myprintf("PROVIDER_TYPE_IR\r\n");
#ifdef ENABLE_IR_PROVIDER
		// provIRProcess(data);
#endif
		break;
	case PROVIDER_TYPE_TEMP:
		PROVIDER_DEBUG("PROVIDER_TYPE_TEMP\r\n");
		// provTempProcess(data);
		break;
	}
}
void provTmr()
{
	provOutputTmr();
	provInputTmr();
#ifdef ENABLE_IR_PROVIDER
	provIRTmr();
#endif
	provTempTmr();
}

void provPrepareHeader(TProvHeader* header)
{
	header->packetId = ethPacketId;
	header->sessKey = ethSessKey;
	
	ethPacketId++;
}
