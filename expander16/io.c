#include "io.h"
#include <settings.h>
#include <hardware.h>
#include <i2c.h>
#include <advinputmanager.h>

#include "providers.h"

uint16_t ioInputs = 0, ioOutputs = 0;

uint8_t io_readPCF(uint8_t addr);
int io_getInp(int idx);
uint16_t io_getOutMask(int idx);

void ioInit()
{
	i2cInit();
	IO_INPUT(INT);

	ioOutputs = 0;
	provOutputUpdate();

	ioInputs = io_readPCF(PCF_IN0);
	ioInputs |= io_readPCF(PCF_IN1) << 8;

	advimInit();
	advimProcess(ticks);
	provInit();
}
void ioProcess()
{
	if (IO_IS_LOW(INT))
	{
		ioInputs = io_readPCF(PCF_IN0);
		ioInputs |= io_readPCF(PCF_IN1) << 8;

		// int i; for (i = 0; i < 16; i++)
			// myprintf("%d ", !!io_getInp(i));
		// myprintf("\n%s\r\n", dec2bin16(ioInputs));
	}
	advimProcess(ticks);
}

void onInputLow(uint8_t idx)
{
#ifdef ETHERNET
	provInputSetState(idx, 0);
	provInputSendState();
#endif
}
void onInputHigh(uint8_t idx)
{
#ifdef ETHERNET
	provInputSetState(idx, 1);
	provInputSendState();
#endif
}

uint8_t ADVIM_getPinState(struct TInputAddr* addr)
{
	return !!io_getInp(addr->num);
}

// Output provider callbacks
void provOutputSetOutput(int num, int enable)
{
	if (enable)
		ioOutputs |= io_getOutMask(num);
	else
		ioOutputs &= ~io_getOutMask(num);
}
void provOutputUpdate()
{
	i2cWriteDataNoReg(PCF_ADDR | PCF_OUT0, (uint8_t*)&ioOutputs, 1);
	i2cWriteDataNoReg(PCF_ADDR | PCF_OUT1, (uint8_t*)&ioOutputs + 1, 1);
}

// Input provider callbacks
void provInputResetState()
{
	int i;

	// settings initial state
	for (i = 0; i < INPUTS_COUNT; i++)
	{
		if (io_getInp(i))
			provInputSetState(i, 1);
		else
			provInputSetState(i, 0);
	}
}

uint8_t io_readPCF(uint8_t addr)
{
	char d[1];
	i2cReadDataNoReg(PCF_ADDR | addr, d, 1);
	return d[0];
}

int io_getInp(int idx)
{
	switch (idx)
	{
	case 0:  return ioInputs & (1ul << 3);
	case 1:  return ioInputs & (1ul << 2);
	case 2:  return ioInputs & (1ul << 1);
	case 3:  return ioInputs & (1ul << 0);
	case 4:  return ioInputs & (1ul << 4);
	case 5:  return ioInputs & (1ul << 5);
	case 6:  return ioInputs & (1ul << 6);
	case 7:  return ioInputs & (1ul << 7);
	case 8:  return ioInputs & (1ul << 11);
	case 9:  return ioInputs & (1ul << 10);
	case 10: return ioInputs & (1ul << 9);
	case 11: return ioInputs & (1ul << 8);
	case 12: return ioInputs & (1ul << 12);
	case 13: return ioInputs & (1ul << 13);
	case 14: return ioInputs & (1ul << 14);
	case 15: return ioInputs & (1ul << 15);
	}
}
uint16_t io_getOutMask(int idx)
{
	switch (idx)
	{
	case 0:  return 1ul << 3;
	case 1:  return 1ul << 2;
	case 2:  return 1ul << 1;
	case 3:  return 1ul << 0;
	case 4:  return 1ul << 4;
	case 5:  return 1ul << 5;
	case 6:  return 1ul << 6;
	case 7:  return 1ul << 7;
	case 8:  return 1ul << 11;
	case 9:  return 1ul << 10;
	case 10: return 1ul << 9;
	case 11: return 1ul << 8;
	case 12: return 1ul << 12;
	case 13: return 1ul << 13;
	case 14: return 1ul << 14;
	case 15: return 1ul << 15;
	}
}

