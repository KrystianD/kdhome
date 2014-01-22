#include "advinputmanager.h"

#include <stdio.h>

static void tmpPrint ()
{
	u8 i;
#ifdef ADVIM_DEBUG
#ifdef ADVIM_PATTERNENABLED
	if (ADVIM_patternIdx & ADVIM_PATTERNIDX_ACTIVEHIGH)
		ADVIM_PRINTF ("pat (first high): ");
	else if (ADVIM_patternIdx & ADVIM_PATTERNIDX_ACTIVELOW)
		ADVIM_PRINTF ("pat (first low): ");

	for (i = 0; i < (ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK); i++)
	{
		ADVIM_PRINTF ("%u ", ADVIM_pattern[i] * 10);
	}
	ADVIM_PRINTF ("\r\n");
#endif
#endif
}

void advimInit ()
{
	ADVIM_patternLastChange = 0;
	ADVIM_patternIdx = 0;

	#if ADVIM_INPUTSCOUNT > 0
	ADVIM_inputs[0].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT0_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[0], ADVIM_INPUT0_ADDR);
	ADVIM_inputs[0].debounceTime = ADVIM_INPUT0_DEBOUNCETIME;
	ADVIM_inputs[0].patternTimeout = ADVIM_INPUT0_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 1
	ADVIM_inputs[1].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT1_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[1], ADVIM_INPUT1_ADDR);
	ADVIM_inputs[1].debounceTime = ADVIM_INPUT1_DEBOUNCETIME;
	ADVIM_inputs[1].patternTimeout = ADVIM_INPUT1_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 2
	ADVIM_inputs[2].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT2_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[2], ADVIM_INPUT2_ADDR);
	ADVIM_inputs[2].debounceTime = ADVIM_INPUT2_DEBOUNCETIME;
	ADVIM_inputs[2].patternTimeout = ADVIM_INPUT2_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 3
	ADVIM_inputs[3].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT3_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[3], ADVIM_INPUT3_ADDR);
	ADVIM_inputs[3].debounceTime = ADVIM_INPUT3_DEBOUNCETIME;
	ADVIM_inputs[3].patternTimeout = ADVIM_INPUT3_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 4
	ADVIM_inputs[4].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT4_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[4], ADVIM_INPUT4_ADDR);
	ADVIM_inputs[4].debounceTime = ADVIM_INPUT4_DEBOUNCETIME;
	ADVIM_inputs[4].patternTimeout = ADVIM_INPUT4_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 5
	ADVIM_inputs[5].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT5_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[5], ADVIM_INPUT5_ADDR);
	ADVIM_inputs[5].debounceTime = ADVIM_INPUT5_DEBOUNCETIME;
	ADVIM_inputs[5].patternTimeout = ADVIM_INPUT5_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 6
	ADVIM_inputs[6].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT6_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[6], ADVIM_INPUT6_ADDR);
	ADVIM_inputs[6].debounceTime = ADVIM_INPUT6_DEBOUNCETIME;
	ADVIM_inputs[6].patternTimeout = ADVIM_INPUT6_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 7
	ADVIM_inputs[7].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT7_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[7], ADVIM_INPUT7_ADDR);
	ADVIM_inputs[7].debounceTime = ADVIM_INPUT7_DEBOUNCETIME;
	ADVIM_inputs[7].patternTimeout = ADVIM_INPUT7_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 8
	ADVIM_inputs[8].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT8_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[8], ADVIM_INPUT8_ADDR);
	ADVIM_inputs[8].debounceTime = ADVIM_INPUT8_DEBOUNCETIME;
	ADVIM_inputs[8].patternTimeout = ADVIM_INPUT8_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 9
	ADVIM_inputs[9].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[9], ADVIM_INPUT9_ADDR);
	ADVIM_inputs[9].debounceTime = ADVIM_INPUT9_DEBOUNCETIME;
	ADVIM_inputs[9].patternTimeout = ADVIM_INPUT9_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 10
	ADVIM_inputs[10].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[10], ADVIM_INPUT10_ADDR);
	ADVIM_inputs[10].debounceTime = ADVIM_INPUT10_DEBOUNCETIME;
	ADVIM_inputs[10].patternTimeout = ADVIM_INPUT10_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 11
	ADVIM_inputs[11].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[11], ADVIM_INPUT11_ADDR);
	ADVIM_inputs[11].debounceTime = ADVIM_INPUT11_DEBOUNCETIME;
	ADVIM_inputs[11].patternTimeout = ADVIM_INPUT11_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 12
	ADVIM_inputs[12].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[12], ADVIM_INPUT12_ADDR);
	ADVIM_inputs[12].debounceTime = ADVIM_INPUT12_DEBOUNCETIME;
	ADVIM_inputs[12].patternTimeout = ADVIM_INPUT12_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 13
	ADVIM_inputs[13].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[13], ADVIM_INPUT13_ADDR);
	ADVIM_inputs[13].debounceTime = ADVIM_INPUT13_DEBOUNCETIME;
	ADVIM_inputs[13].patternTimeout = ADVIM_INPUT13_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 14
	ADVIM_inputs[14].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[14], ADVIM_INPUT14_ADDR);
	ADVIM_inputs[14].debounceTime = ADVIM_INPUT14_DEBOUNCETIME;
	ADVIM_inputs[14].patternTimeout = ADVIM_INPUT14_PATTERNTIMEOUT;
	#endif
	#if ADVIM_INPUTSCOUNT > 15
	ADVIM_inputs[15].state_flags = ADVIM_STATE_UNDETERMINED | ADVIM_INPUT9_FLAGS;
	ADVIM_SETPORTADDR (ADVIM_inputs[15], ADVIM_INPUT15_ADDR);
	ADVIM_inputs[15].debounceTime = ADVIM_INPUT15_DEBOUNCETIME;
	ADVIM_inputs[15].patternTimeout = ADVIM_INPUT15_PATTERNTIMEOUT;
	#endif
}
void advimProcess (uint16_t ticks)
{
	uint8_t i;
	for (i = 0; i < ADVIM_INPUTSCOUNT; i++)
	{
		struct TInputData *input = &ADVIM_inputs[i];
		uint8_t flags = ADVIM_GETFLAGS(input->state_flags);
		uint8_t state = ADVIM_getPinState (&input->addr);
		
				//ADVIM_PRINTF ("[%u] %u- %u\r\n", ticks, i, state);

		if (state)
		{
			switch (ADVIM_GETSTATE(input->state_flags))
			{
			case ADVIM_STATE_UNDETERMINED:
				ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_HIGH);
#ifdef ADVIM_DEBUG
				ADVIM_PRINTF ("[%4u] [%u] high\r\n", ticks, i);
#endif
				break;
			case ADVIM_STATE_LOW_TO_HIGH_TODEBOUNCE:
				if (ticks - input->changeTime >= input->debounceTime)
				{
					ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_HIGH);

#ifdef ADVIM_PATTERNENABLED
					if (flags & ADVIM_FLAGS_ENABLEPATTERN)
					{
						ADVIM_patternInput = i;
						if (ADVIM_patternIdx & ADVIM_PATTERNIDX_ACTIVEMASK)
						{
							if ((ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK) < ADVIM_PATTERNMAX)
							{	
								ADVIM_pattern[ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK] = (ticks - ADVIM_patternLastChange) / 10;
								ADVIM_patternIdx = (ADVIM_patternIdx & ADVIM_PATTERNIDX_ACTIVEMASK) |
									((ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK) + 1);
							}
						}
						else
						{
							ADVIM_patternIdx = ADVIM_PATTERNIDX_ACTIVEHIGH;
						}
						ADVIM_patternLastChange = ticks;
						tmpPrint ();
					}
#endif

					onInputHigh (i);
#ifdef ADVIM_DEBUG
					ADVIM_PRINTF ("[%4u] [%u] high\r\n", ticks, i);
#endif
				}
				break;
			case ADVIM_STATE_LOW:
				ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_LOW_TO_HIGH_TODEBOUNCE);
				input->changeTime = ticks;
#ifdef ADVIM_DEBUG
				ADVIM_PRINTF ("[%4u] [%u] low_to_high\r\n", ticks, i);
#endif
				break;
			case ADVIM_STATE_HIGH_TO_LOW_TODEBOUNCE:
				//if (ticks - input->changeTime >= input->debounceTime)
				{
					ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_HIGH);
#ifdef ADVIM_DEBUG
					ADVIM_PRINTF ("[%4u] [%u] high (debounce failed)\r\n", ticks, i);
#endif
				}
				break;
			}
		}
		else
		{
			switch (ADVIM_GETSTATE(input->state_flags))
			{
			case ADVIM_STATE_UNDETERMINED:
				ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_LOW);
#ifdef ADVIM_DEBUG
				ADVIM_PRINTF ("[%4u] [%u] low\r\n", ticks, i);
#endif
				break;
			case ADVIM_STATE_LOW_TO_HIGH_TODEBOUNCE:
				// if (ticks - input->changeTime >= input->debounceTime)
				{
					ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_LOW);
#ifdef ADVIM_DEBUG
					ADVIM_PRINTF ("[%4u] [%u] low (debounce failed)\r\n", ticks, i);
#endif
				}
				break;
			case ADVIM_STATE_HIGH:
				ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_HIGH_TO_LOW_TODEBOUNCE);
				input->changeTime = ticks;
#ifdef ADVIM_DEBUG
				ADVIM_PRINTF ("[%4u] [%u] high_to_low\r\n", ticks, i);
#endif
				break;
			case ADVIM_STATE_HIGH_TO_LOW_TODEBOUNCE:
				if (ticks - input->changeTime >= input->debounceTime)
				{
					ADVIM_SETSTATE(input->state_flags, ADVIM_STATE_LOW);

#ifdef ADVIM_PATTERNENABLED
					if (flags & ADVIM_FLAGS_ENABLEPATTERN)
					{
						ADVIM_patternInput = i;
						if (ADVIM_patternIdx & ADVIM_PATTERNIDX_ACTIVEMASK)
						{
							if ((ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK) < ADVIM_PATTERNMAX)
							{	
								ADVIM_pattern[ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK] = (ticks - ADVIM_patternLastChange) / 10;
								ADVIM_patternIdx = (ADVIM_patternIdx & ADVIM_PATTERNIDX_ACTIVEMASK) |
									((ADVIM_patternIdx & ADVIM_PATTERNIDX_IDXMASK) + 1);
							}
						}
						else
						{
							ADVIM_patternIdx = ADVIM_PATTERNIDX_ACTIVELOW;
						}
						ADVIM_patternLastChange = ticks;
						tmpPrint ();
					}
#endif

					onInputLow (i);
#ifdef ADVIM_DEBUG
					ADVIM_PRINTF ("[%4u] [%u] low\r\n", ticks, i);
#endif
				}
				break;
			}
		}

		if (flags & ADVIM_FLAGS_ENABLEPATTERN && ADVIM_patternInput == i)
		{
			if (ticks - ADVIM_patternLastChange > input->patternTimeout)
			{
				ADVIM_patternIdx = 0;
			}
		}
#ifdef ADVIM_DEBUG
		// printf ("%d\n", state);
#endif
	}
}
