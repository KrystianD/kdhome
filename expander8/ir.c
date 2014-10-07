#include "ir.h"
#include <public.h>
#include <hardware.h>
#include <myprintf.h>

#include "3rd/IRremote.h"
#include "providers.h"

#define FALLING_SPACE 1
#define RISING_MARK   0

volatile uint32_t ir_lastChangeTime;
volatile int ir_state = RISING_MARK;

void irInit()
{
	IO_INPUT_PP(IR);
	IO_HIGH(IR);
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP;
	
	TIM2->PSC = F_CPU / 1000000;
	TIM2->ARR = 32000;
	TIM2->EGR = TIM_EGR_UG;
	TIM2->CNT = 0;
	
	TIM2->CCMR2 = TIM_CCMR2_CC4S_0;
	TIM2->CCER = TIM_CCER_CC4P | TIM_CCER_CC4E;
	TIM2->DIER = TIM_DIER_CC4IE;
	
	ENABLE_INTERRUPT(TIM2_IRQn);
	ir_lastChangeTime = 0;
	irparams.rawlen = 0;
	TIM2->CR1 = TIM_CR1_CEN;
	
	irparams.rcvstate = STATE_STOP;
}
void irProcess()
{
	// myprintf("%6d %d\r\n", TIM2->CCR4, IO_IS_HIGH(IR) ?1:0);
	if (ticks - ir_lastChangeTime > 5 && ir_state == FALLING_SPACE)
	{
		if (irparams.rawlen > 0)
		{
			struct decode_results res;
			int r = decode(&res);
			if (r && res.decode_type != -1)
			{
				myprintf("decode: %d 0x%08x %d\r\n", r, res.value, res.decode_type);
#ifdef ETHERNET
				provIRNewCode(res.value);
#endif
			}
			// int i; for(i=0;i<idx;i++) myprintf("%4d, ", data[i]); myprintf("\r\n");
		}
		ir_lastChangeTime = 0;
		irparams.rawlen = 0;
		ir_state = FALLING_SPACE;
		TIM2->CCER |= TIM_CCER_CC4P;
	}
}

void TIM2_Handler()
{
	TIM2->CNT = 0;
	TIM2->SR &= ~TIM_SR_CC4IF;
	
	uint16_t cnt = TIM2->CCR4;
	irparams.rawbuf[irparams.rawlen] = cnt;
	irparams.rawlen++;
	if (irparams.rawlen == sizeof(irparams.rawbuf))
	{
		myprintf("ir buf er\r\n");
		irparams.rawlen = 0;
	}
	if (ir_state == FALLING_SPACE)
	{
		TIM2->CCER &= ~TIM_CCER_CC4P;
		ir_state = RISING_MARK;
	}
	else
	{
		TIM2->CCER |= TIM_CCER_CC4P;
		ir_state = FALLING_SPACE;
	}
	ir_lastChangeTime = ticks;
}
