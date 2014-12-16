#include "provider_ir.h"

#include "providers.h"
#include <kdhome.h>
#include "providers_settings.h"

#include <string.h>

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

void provIRReset()
{
}
void provIRRegister()
{
	PROVIDER_DEBUG("REGISTER IR\r\n");
	TProvIRRegisterPacket p;
	provPrepareHeader((TProvHeader*)&p);
	p.header.type = PROVIDER_TYPE_IR;
	p.header.cmd = IR_CMD_REGISTER;
	provSendPacket(&p, sizeof(p));
}
void provIRProcess(const void* data, int len)
{
}
void provIRTmr()
{
}
void provIRNewCode(uint32_t code)
{
	TProvIRCodePacket p;
	provPrepareHeader((TProvHeader*)&p);
	
	p.header.type = PROVIDER_TYPE_IR;
	p.header.cmd = IR_NOTF_NEWCODE;
	p.code = code;
	
	provSendPacket(&p, sizeof(p));
}
