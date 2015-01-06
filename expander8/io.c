#include "io.h"
#include <settings.h>
#include <hardware.h>
#include <advinputmanager.h>
#include <myprintf.h>

#include "providers.h"

void ioInit()
{
	provOutputUpdate();

	advimInit();
	advimProcess(ticks);
	provInit();
}
void ioProcess()
{
	advimProcess(ticks);
}

void onInputLow(uint8_t idx)
{
	myprintf("low %d\r\n", idx);
// #ifdef ETHERNET
	provInputSetState(idx, 0);
	provInputSendState();
// #endif
}
void onInputHigh(uint8_t idx)
{
	myprintf("high %d\r\n", idx);
// #ifdef ETHERNET
	provInputSetState(idx, 1);
	provInputSendState();
// #endif
}

// Output provider callbacks
void provOutputSetOutput(int num, int enable)
{
	switch(num)
	{
	case 0: if(enable) IO_HIGH(OUT1); else IO_LOW(OUT1); break;
	case 1: if(enable) IO_HIGH(OUT2); else IO_LOW(OUT2); break;
	case 2: if(enable) IO_HIGH(OUT3); else IO_LOW(OUT3); break;
	case 3: if(enable) IO_HIGH(OUT4); else IO_LOW(OUT4); break;
	case 4: if(enable) IO_HIGH(OUT5); else IO_LOW(OUT5); break;
	case 5: if(enable) IO_HIGH(OUT6); else IO_LOW(OUT6); break;
	case 6: if(enable) IO_HIGH(OUT7); else IO_LOW(OUT7); break;
	case 7: if(enable) IO_HIGH(OUT8); else IO_LOW(OUT8); break;
	}
}
void provOutputUpdate()
{
}

// Input provider callbacks
void provInputResetState()
{
	int i;

	// settings initial state
	provInputSetState(0, IO_IS_HIGH(IN1) ? 1 : 0);
	provInputSetState(1, IO_IS_HIGH(IN2) ? 1 : 0);
	provInputSetState(2, IO_IS_HIGH(IN3) ? 1 : 0);
	provInputSetState(3, IO_IS_HIGH(IN4) ? 1 : 0);
	provInputSetState(4, IO_IS_HIGH(IN5) ? 1 : 0);
	provInputSetState(5, IO_IS_HIGH(IN6) ? 1 : 0);
	provInputSetState(6, IO_IS_HIGH(IN7) ? 1 : 0);
	provInputSetState(7, IO_IS_HIGH(IN8) ? 1 : 0);
}
