#include "provider_output.h"
#include "providers_settings.h"

#include <kdhome.h>
#include <myprintf.h>

void provOutputReset()
{
}
void provOutputProcess(const void* data, int len)
{
	TSrvHeader *header = (TSrvHeader*)data;
	
	switch (header->cmd)
	{
	case OUTPUT_CMD_SET_OUTPUTS:
	{
		TSrvOutputSetOutputsPacket *p = (TSrvOutputSetOutputsPacket*)data;
		int i;
		for (i = 0; i < p->cnt; i++)
		{
			int en = p->outputs & (1 << i);
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
