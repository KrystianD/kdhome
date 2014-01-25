#include "provider_output.h"
#include "providers_settings.h"

#include <kdhome.h>

void provOutputReset()
{
}
void provOutputProcess(TByteBuffer* data)
{
	uint8_t cmd;
	if (BYTEBUFFER_FETCH(data, cmd)) return;
	// myprintf("cmd: 0x%02x\r\n", cmd);

	switch (cmd)
	{
	case OUTPUT_CMD_SET_OUTPUTS:
		{
			uint8_t cnt;
			if (BYTEBUFFER_FETCH(data, cnt)) return;
			// myprintf("cnt: %d\r\n", cnt);

			int i;
			uint8_t b;
			if (BYTEBUFFER_FETCH(data, b)) return;
			int idx = 0;
			for (i = 0; i < cnt; i++)
			{
				int en = b & (1 << (7 - idx));

				idx++;
				if (idx == 8)
				{
					idx = 0;
					if (BYTEBUFFER_FETCH(data, b)) return;
				}

				provOutputSetOutput(i, en);
			}
			provOutputUpdate();
		}
		break;
	}
}
void provOutputTmr()
{
}
