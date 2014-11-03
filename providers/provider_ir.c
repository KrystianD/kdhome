#include "provider_ir.h"
#include "providers_settings.h"
#include <myprintf.h>

#include "providers.h"
#include "ethernet.h"
#include <kdhome.h>

void provIRReset()
{
}
void provIRRegister()
{
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
