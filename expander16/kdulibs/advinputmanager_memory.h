#ifndef __ADVINPUTMANAGER_MEMORY__
#define __ADVINPUTMANAGER_MEMORY__

#include <public.h>

struct TInputAddr
{
	uint8_t num;
};

extern uint8_t ADVIM_getPinState (struct TInputAddr* addr);

#define ADVIM_SETPORTADDR(s,x) { s.addr.num = x; }

#endif
