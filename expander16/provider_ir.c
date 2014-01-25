#include "provider_ir.h"
#include "providers_settings.h"

#include <kdhome.h>

void provIRReset()
{
}
void provIRProcess(TByteBuffer* data)
{
}
void provIRTmr()
{
}
void provIRNewCode(uint32_t code)
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
