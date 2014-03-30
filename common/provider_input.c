#include "providers.h"
#include "provider_input.h"
#include "providers_settings.h"
// #include <myprintf.h>

// #include "ethernet.h"
#include <kdhome.h>
#include <string.h>

uint8_t prov_inputLow[INPUTS_COUNT], prov_inputHigh[INPUTS_COUNT];

void provInputReset()
{
	memset(&prov_inputLow, 0, sizeof(prov_inputLow));
	memset(&prov_inputHigh, 0, sizeof(prov_inputHigh));
	provInputResetState();
}
void provInputProcess(TByteBuffer* data)
{
	uint8_t cmd;
	if (BYTEBUFFER_FETCH(data, cmd)) return;
	// myprintf("cmd: 0x%02x\r\n", cmd);

	switch (cmd)
	{
	default:
	case INPUT_NOTF_NEWSTATE:
		break;
	case INPUT_REQ_SENDSTATE:
		myprintf("INPUT REQUESET!!!\r\n");
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
	TByteBuffer b;
	if (!provPrepareBuffer(&b, 2 + 1 + 1 + INPUTS_COUNT * (1 + 1)))
		return;
	uint16_t type = PROVIDER_TYPE_INPUT;
	BYTEBUFFER_APPEND(&b, type);

	uint8_t cmd = INPUT_NOTF_NEWSTATE;
	BYTEBUFFER_APPEND(&b, cmd);

	uint8_t cnt = INPUTS_COUNT;
	BYTEBUFFER_APPEND(&b, cnt);

	int i;
	for (i = 0; i < INPUTS_COUNT; i++)
	{
		uint8_t low = prov_inputLow[i];
		uint8_t high = prov_inputHigh[i];
		BYTEBUFFER_APPEND(&b, low);
		BYTEBUFFER_APPEND(&b, high);
	}

	provSendPacket(&b);

	provFreeBuffer(&b);
}
