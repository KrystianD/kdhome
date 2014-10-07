#include "providers.h"
#include <public.h>
#include <myprintf.h>
#include <settings.h>

#include "ethernet.h"

#include <kdhome.h>
#include "providers_settings.h"

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
	// provOutputReset();
	// provInputReset();
	// prov_irReset();
}
// void provProcess(TByteBuffer* data)
void provProcess(const char* data, int len)
{
	TSrvHeader *header = (TSrvHeader*)data;
	
	// uint16_t type;
	// if (BYTEBUFFER_FETCH(data,type)) return;
	// myprintf("type: 0x%04x\r\n", header->type);
	
	switch (header->type)
	{
	case PROVIDER_TYPE_CONTROL:
	{
		// myprintf("PROVIDER_TYPE_CONTROL\r\n");
		// uint8_t cmd;
		// if (BYTEBUFFER_FETCH(data,cmd)) return;
		if (header->cmd == CONTROL_CMD_REGISTER)
		{
			TSrvCmdRegister *p = (TSrvCmdRegister*)data;
			ethSessKey = p->sessKey;
			// if (BYTEBUFFER_FETCH(data,ethSessKey)) return;
			myprintf("New sesskey: 0x%04x\r\n", ethSessKey);
			
			ethPacketId = 1;
			// reset providers
			provOutputReset();
			provInputReset();
#ifdef ENABLE_IR_PROVIDER
			provIRReset();
#endif
			provTempReset();
		}
	}
	break;
	case PROVIDER_TYPE_OUTPUT:
		// myprintf("PROVIDER_TYPE_OUTPUT\r\n");
		provOutputProcess(data, len);
		break;
	case PROVIDER_TYPE_INPUT:
		// myprintf("PROVIDER_TYPE_INPUT\r\n");
		// provInputProcess(data);
		break;
	case PROVIDER_TYPE_IR:
		// myprintf("PROVIDER_TYPE_IR\r\n");
#ifdef ENABLE_IR_PROVIDER
		// provIRProcess(data);
#endif
		break;
	case PROVIDER_TYPE_TEMP:
		myprintf("PROVIDER_TYPE_TEMP\r\n");
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
	// buffer->p = pbuf_alloc(PBUF_RAW, len + 2 + 2, PBUF_POOL);
	// // buffer->p = pbuf_alloc(PBUF_TRANSPORT, len + 2 + 2, PBUF_POOL);
	// if (!buffer->p)
	// return 0;
	// buffer->pos = 0;
	
	header->packetId = ethPacketId;
	header->sessKey = ethSessKey;

	// BYTEBUFFER_APPEND(buffer, ethPacketId);
	// BYTEBUFFER_APPEND(buffer, ethSessKey);
	
	ethPacketId++;
	
	// return 1;
}
