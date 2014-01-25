#include "providers.h"
#include <public.h>
#include "settings.h"

#include "ethernet.h"

#include <kdhome.h>
#include "providers_settings.h"

// PRIVATE

// ------------------------- IMPLEMENTATION ------------------------------

// PUBLIC
void provInit()
{
	// provOutputReset();
	// provInputReset();
	// prov_irReset();
}
void provProcess(TByteBuffer* data)
{
	uint16_t type;
	if (BYTEBUFFER_FETCH(data,type)) return;
	// myprintf("type: 0x%04x\r\n", type);

	switch (type) {
	case PROVIDER_TYPE_CONTROL:
		{
			uint8_t cmd;
			if (BYTEBUFFER_FETCH(data,cmd)) return;
			if (cmd == CONTROL_CMD_REGISTER)
			{
				if (BYTEBUFFER_FETCH(data,ethSessKey)) return;
				myprintf("New sesskey: 0x%04x\r\n", ethSessKey);

				ethPacketId = 1;
				// reset providers
				provOutputReset();
				provInputReset();
				provIRReset();
				provTempReset();
			}
		}
		break;
	case PROVIDER_TYPE_OUTPUT:
		provOutputProcess(data);
		break;
	case PROVIDER_TYPE_INPUT:
		provInputProcess(data);
		break;
	case PROVIDER_TYPE_IR:
		provIRProcess(data);
		break;
	case PROVIDER_TYPE_TEMP:
		provTempProcess(data);
		break;
	}
}
void provTmr()
{
	provOutputTmr();
	provInputTmr();
	provIRTmr();
	provTempTmr();
}
