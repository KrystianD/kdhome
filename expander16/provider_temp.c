#include "provider_temp.h"
#include "providers_settings.h"

#include <kdhome.h>
#include <string.h>

void provTempReset()
{
	memset(&prov_inputLow, 0, sizeof(prov_inputLow));
	memset(&prov_inputHigh, 0, sizeof(prov_inputHigh));
	provInputResetState();
}
void provTempProcess(TByteBuffer* data)
{
	uint8_t cmd;
	if (BYTEBUFFER_FETCH(data, cmd)) return;
	// myprintf("cmd: 0x%02x\r\n", cmd);

	switch (cmd)
	{
	default:
		break;
	}
}
