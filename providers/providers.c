#include "providers.h"

#include <kdhome.h>
#include "providers_settings.h"

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

// PRIVATE
static uint16_t sessKey = 0;
static uint16_t packetId = 1;

static uint32_t lastPresenceSendTime = 0;

// registration
// #define OUTPUT_NUM 0
// #define INPUT_NUM  1
// #define IR_NUM     2
// #define TEMP_NUM   3
// struct TProviderInfo
// {
	// void (*resetFunc)();
	// void (*registerFunc)();
	// void (*processFunc)(const void* data, int len);
	// void (*tmrFunc)();
	// int enabled;
// } providers[] =
// {
	// // Output
	// {
		// .resetFunc = provOutputReset, .registerFunc = provOutputRegister, .processFunc = provOutputProcess, .tmrFunc = provOutputTmr,
		// .enabled = OUTPUT_PROVIDER_ENABLED
	// },
	// // Input
	// {
		// .resetFunc = provInputReset, .registerFunc = provInputRegister, .processFunc = provInputProcess, .tmrFunc = provInputTmr,
		// .enabled = INPUT_PROVIDER_ENABLED
	// },
	// // IR
	// {
		// .resetFunc = provIRReset, .registerFunc = provIRRegister, .processFunc = provIRProcess, .tmrFunc = provIRTmr,
		// .enabled = IR_PROVIDER_ENABLED
	// },
	// // Temp
	// {
		// .resetFunc = provTempReset, .registerFunc = provTempRegister, .processFunc = provTempProcess, .tmrFunc = provTempTmr,
		// .enabled = TEMP_PROVIDER_ENABLED
	// },
// };
// const int PROVIDERS_COUNT = sizeof(providers) / sizeof(providers[0]);

// ------------------------- IMPLEMENTATION ------------------------------

// PUBLIC
void provInit()
{
	sessKey = 0;
	packetId = 1;
}
void provProcess(const void* data, int len)
{
	int i;

	TSrvHeader *header = (TSrvHeader*)data;
	
	PROVIDER_DEBUG("type: 0x%04x cmd: %d\r\n", header->type, header->cmd);
	
	switch (header->type)
	{
	case PROVIDER_TYPE_CONTROL:
	{
		if (header->cmd == CONTROL_CMD_REGISTER)
		{
			TSrvCmdRegister *p = (TSrvCmdRegister*)data;
			sessKey = p->sessKey;
			PROVIDER_DEBUG("New sesskey: 0x%04x\r\n", sessKey);
			
			packetId = 1;
			// reset providers
			
			// for (i = 0; i < PROVIDERS_COUNT; i++)
			// {
				// if (providers[i].enabled)
				// {
					// providers[i].resetFunc();
				// }
			// }
#ifdef OUTPUT_PROVIDER_ENABLED
			provOutputReset();
			provOutputRegister();
#endif
#ifdef INPUT_PROVIDER_ENABLED
			provInputReset();
			provInputRegister();
#endif
#ifdef IR_PROVIDER_ENABLED
			provIRReset();
			provIRRegister();
#endif
#ifdef TEMP_PROVIDER_ENABLED
			provTempReset();
			provTempRegister();
#endif
#ifdef COUNTER_PROVIDER_ENABLED
			provCounterReset();
			provCounterRegister();
#endif
		}
	}
	break;
	case PROVIDER_TYPE_OUTPUT:
#ifdef OUTPUT_PROVIDER_ENABLED
		// myprintf("PROVIDER_TYPE_OUTPUT\r\n");
		provOutputProcess(data, len);
#endif
		break;
	case PROVIDER_TYPE_INPUT:
#ifdef INPUT_PROVIDER_ENABLED
		// myprintf("PROVIDER_TYPE_INPUT\r\n");
		provInputProcess(data, len);
#endif
		break;
	case PROVIDER_TYPE_IR:
#ifdef IR_PROVIDER_ENABLED
		// myprintf("PROVIDER_TYPE_IR\r\n");
		// provIRProcess(data);
#endif
		break;
	case PROVIDER_TYPE_TEMP:
#ifdef TEMP_PROVIDER_ENABLED
		PROVIDER_DEBUG("PROVIDER_TYPE_TEMP\r\n");
#endif
		break;
	case PROVIDER_TYPE_COUNTER:
#ifdef COUNTER_PROVIDER_ENABLED
		PROVIDER_DEBUG("PROVIDER_TYPE_COUNTER\r\n");
#endif
		break;
	}
}
void provTmr()
{
	if (getTicks() - lastPresenceSendTime >= 1000)
	{
		lastPresenceSendTime = getTicks();
		
		TProvHeader header;
		provPrepareHeader(&header);
		
		header.type = PROVIDER_TYPE_CONTROL;
		header.cmd = 0;
		provSendPacket(&header, sizeof(header));
	}
	
#ifdef OUTPUT_PROVIDER_ENABLED
	provOutputTmr();
#endif
#ifdef INPUT_PROVIDER_ENABLED
	provInputTmr();
#endif
#ifdef IR_PROVIDER_ENABLED
	provIRTmr();
#endif
#ifdef TEMP_PROVIDER_ENABLED
	provTempTmr();
#endif
#ifdef COUNTER_PROVIDER_ENABLED
	provCounterTmr();
#endif
}

void provPrepareHeader(TProvHeader* header)
{
	header->packetId = packetId;
	header->sessKey = sessKey;
	
	packetId++;
}
