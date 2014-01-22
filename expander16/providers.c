#include "providers.h"
#include <public.h>
#include "settings.h"

#include "ethernet.h"

#include "/home/krystiand/prog/kdhome/server/kdhome.h"
#include "providers_settings.h"

// PRIVATE
// output
void prov_outputReset();
void prov_outputProcess(TByteBuffer* data);
void prov_inputProcess(TByteBuffer* data);
void prov_inputSendState();

// input
uint8_t prov_inputLow[INPUTS_COUNT], prov_inputHigh[INPUTS_COUNT];

void prov_inputReset();

// ir
void prov_irReset();

// ------------------------- IMPLEMENTATION ------------------------------

// PUBLIC
void provInit()
{
	prov_outputReset();
	prov_inputReset();
	prov_irReset();
}
void provProcess(TByteBuffer* data)
{
	uint16_t type;
	BYTEBUFFER_FETCH(data,type);
	// myprintf("type: 0x%04x\r\n", type);

	switch (type) {
	case PROVIDER_TYPE_CONTROL:
		{
			uint8_t cmd;
			BYTEBUFFER_FETCH(data,cmd);
			if (cmd == CONTROL_CMD_REGISTER)
			{
				BYTEBUFFER_FETCH(data,ethSessKey);
				myprintf("New sesskey: 0x%04x\r\n", ethSessKey);

				ethPacketId = 1;
				// reset providers
				prov_outputReset();
				prov_inputReset();
				prov_irReset();
			}
		}
		break;
	case PROVIDER_TYPE_OUTPUT:
		prov_outputProcess(data);
		break;
	case PROVIDER_TYPE_INPUT:
		prov_inputProcess(data);
		break;
	}
}

// output
void prov_outputReset()
{
}
void prov_outputProcess(TByteBuffer* data)
{
	uint8_t cmd;
	BYTEBUFFER_FETCH(data, cmd);
	// myprintf("cmd: 0x%02x\r\n", cmd);

	switch (cmd)
	{
	case OUTPUT_CMD_SET_OUTPUTS:
		{
			uint8_t cnt;
			BYTEBUFFER_FETCH(data, cnt);
			// myprintf("cnt: %d\r\n", cnt);

			int i;
			uint8_t b;
			BYTEBUFFER_FETCH(data, b);
			int idx = 0;
			for (i = 0; i < cnt; i++)
			{
				int en = b & (1 << (7 - idx));

				idx++;
				if (idx == 8)
				{
					idx = 0;
					BYTEBUFFER_FETCH(data, b);
				}

				provOutput_setOutputCallback(i, en);
			}
			provOutput_update();
		}
		break;
	}
}

// input
void prov_inputReset()
{
	memset(&prov_inputLow, 0, sizeof(prov_inputLow));
	memset(&prov_inputHigh, 0, sizeof(prov_inputHigh));
	provInput_resetState();
}
void prov_inputProcess(TByteBuffer* data)
{
	uint8_t cmd;
	BYTEBUFFER_FETCH(data, cmd);
	// myprintf("cmd: 0x%02x\r\n", cmd);

	switch (cmd)
	{
	default:
	case INPUT_NOTF_NEWSTATE:
		break;
	case INPUT_REQ_SENDSTATE:
		myprintf("INPUT REQUESET!!!\r\n");
		provInput_sendState();
		break;
	}
}
void provInput_setInput(int num, int value)
{
	if (value)
		prov_inputHigh[num]++;
	else
		prov_inputLow[num]++;

	// int i; for (i=0;i<16;i++)
		// myprintf("inp %d - %d:%d\r\n", i, prov_inputLow[i], prov_inputHigh[i]);
}
void provInput_sendState()
{
	TByteBuffer b;
	if (!ethPrepareBuffer(&b, 2 + 1 + 1 + INPUTS_COUNT * (1 + 1)))
		return;
	uint16_t type = PROVIDER_TYPE_INPUT;
	BYTEBUFFER_APPEND(&b, type);

	uint8_t cmd = INPUT_NOTF_NEWSTATE;
	BYTEBUFFER_APPEND(&b, cmd);

	uint8_t cnt = ADVIM_INPUTSCOUNT;
	BYTEBUFFER_APPEND(&b, cnt);

	int i;
	for (i = 0; i < INPUTS_COUNT; i++)
	{
		uint8_t low = prov_inputLow[i];
		uint8_t high = prov_inputHigh[i];
		BYTEBUFFER_APPEND(&b, low);
		BYTEBUFFER_APPEND(&b, high);
	}

	ethSendPacket(&b);

	ethFreeBuffer(&b);
}

// ir
void prov_irReset()
{
}
void provIRSendCode(uint32_t code)
{
	TByteBuffer b;
	if (!ethPrepareBuffer(&b, 2 + 1 + sizeof(code)))
		return;
	uint16_t type = PROVIDER_TYPE_IR;
	BYTEBUFFER_APPEND(&b, type);

	uint8_t cmd = IR_NOTF_NEWCODE;
	BYTEBUFFER_APPEND(&b, cmd);

	BYTEBUFFER_APPEND(&b, code);

	ethSendPacket(&b);

	ethFreeBuffer(&b);
}
