#ifndef __ADVINPUTMANAGER__
#define __ADVINPUTMANAGER__

#include <public.h>
#include "settings.h"
#include "hardware.h"

/*
Example configuration:

#define ADVIM_DEBUG 1
#define ADVIM_PRINTF myprintf
#define ADVIM_INPUTSCOUNT 1
#define ADVIM_PATTERNENABLED
#define ADVIM_PATTERNMAX 20

#define ADVIM_INPUT0_ADDR IN1
#define ADVIM_INPUT0_FLAGS 0
#define ADVIM_INPUT0_DEBOUNCETIME 40
#define ADVIM_INPUT0_PATTERNTIMEOUT 0
*/

// 3 bits
#define ADVIM_STATE_UNDETERMINED           4
#define ADVIM_STATE_HIGH                   0
#define ADVIM_STATE_HIGH_TO_LOW_TODEBOUNCE 1
#define ADVIM_STATE_LOW                    2
#define ADVIM_STATE_LOW_TO_HIGH_TODEBOUNCE 3

#define ADVIM_FLAGS_ENABLEPATTERN          (1 << (0 + 3))

#define ADVIM_GETSTATE(x) ((x) & 0x07)
#define ADVIM_SETSTATE(x,state) ((x) = (x) & ~0x07 | (state))
#define ADVIM_GETFLAGS(x) ((x) & ~0x07)
#define ADVIM_SETFLAGS(x,flags) ((x) = (x) & 0x07 | (flags))

#define ADVIM_PATTERNIDX_ACTIVEHIGH 0x80
#define ADVIM_PATTERNIDX_ACTIVELOW  0x40
#define ADVIM_PATTERNIDX_ACTIVEMASK 0xc0
#define ADVIM_PATTERNIDX_IDXMASK    0x3f

struct TInputData
{
	uint8_t state_flags;
	uint16_t changeTime;
	uint8_t debounceTime;
	struct TInputAddr addr;

	// pattern
	uint16_t patternTimeout;
};

struct TInputData ADVIM_inputs[ADVIM_INPUTSCOUNT];
#ifdef ADVIM_PATTERNENABLED
uint8_t ADVIM_pattern[ADVIM_PATTERNMAX];
#endif
// 7 bit - is pattern recording active (first high)
// 6 bit - is pattern recording active (first low)
// 0-5 bit - pattern idx
uint8_t ADVIM_patternIdx;
uint8_t ADVIM_patternInput;
uint16_t ADVIM_patternLastChange;

void advimInit ();
void advimProcess (uint16_t ticks);

// handlers
extern void onInputLow (uint8_t idx);
extern void onInputHigh (uint8_t idx);

static void advimSetState (uint8_t idx, uint8_t state)
{
	ADVIM_inputs[idx].state_flags = ADVIM_inputs[idx].state_flags & ~0x07 | state;
}
static void advimSetFlags (uint8_t idx, uint8_t flags)
{
	ADVIM_inputs[idx].state_flags = ADVIM_inputs[idx].state_flags & 0x07 | flags;
}
static void advimSetDebounceTime (uint8_t idx, uint8_t time)
{
	ADVIM_inputs[idx].debounceTime = time;
}
static void advimSetPatternTimeout (uint8_t idx, uint16_t timeout)
{
	ADVIM_inputs[idx].patternTimeout = timeout;
}
static uint8_t advimIsLow (uint8_t idx)
{
	switch (ADVIM_GETSTATE(ADVIM_inputs[idx].state_flags))
	{
	case ADVIM_STATE_LOW:
	case ADVIM_STATE_LOW_TO_HIGH_TODEBOUNCE:
		return 1;
	default:
		return 0;
	}
}
static uint8_t advimIsHigh (uint8_t idx)
{
	switch (ADVIM_GETSTATE(ADVIM_inputs[idx].state_flags))
	{
	case ADVIM_STATE_HIGH:
	case ADVIM_STATE_HIGH_TO_LOW_TODEBOUNCE:
		return 1;
	default:
		return 0;
	}
}

#endif
