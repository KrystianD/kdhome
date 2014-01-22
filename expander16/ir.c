#include "ir.h"
#include <public.h>
#include <hardware.h>

#include "3rd/IRremote.h"
#include "providers.h"

#define FALLING_SPACE 1
#define RISING_MARK   0

volatile uint32_t ir_lastChangeTime;
volatile int ir_state = RISING_MARK;

void irInit()
{
	IO_INPUT_PP(IR); IO_HIGH(IR);
	// TIM3_CH2

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->PSC = F_CPU / 1000000;
	TIM3->ARR = 32000;
	TIM3->EGR = TIM_EGR_UG;
	TIM3->CNT = 0;

	TIM3->CCMR1 = TIM_CCMR1_CC2S_0;
	TIM3->CCER = TIM_CCER_CC2P | TIM_CCER_CC2E;
	TIM3->DIER = TIM_DIER_CC2IE;

	ENABLE_INTERRUPT(TIM3_IRQn);
	ir_lastChangeTime = 0;
	irparams.rawlen = 0;
	TIM3->CR1 = TIM_CR1_CEN;

	irparams.rcvstate = STATE_STOP;
}
void irProcess()
{
	if (ticks - ir_lastChangeTime > 5 && ir_state == FALLING_SPACE)
	{
		if (irparams.rawlen > 0)
		{
			struct decode_results res;
			int r = decode(&res);
			if (r && res.decode_type != -1)
			{
				myprintf("decode: %d %x %d\r\n", r, res.value, res.decode_type);
#ifdef ETHERNET
				provIRSendCode(res.value);
#endif
			}
			// int i; for(i=0;i<idx;i++) myprintf("%4d, ", data[i]); myprintf("\r\n");
		}
		ir_lastChangeTime=0;
		irparams.rawlen = 0;
		ir_state = FALLING_SPACE;
		TIM3->CCER |= TIM_CCER_CC2P;
	}
}

void TIM3_Handler()
{
	TIM3->CNT = 0;
	TIM3->SR &= ~TIM_SR_CC2IF;
	
	uint16_t cnt = TIM3->CCR2;
	irparams.rawbuf[irparams.rawlen++] = cnt;
	if (ir_state == FALLING_SPACE) {
		TIM3->CCER &= ~TIM_CCER_CC2P;
		ir_state = RISING_MARK;
	} else {
		TIM3->CCER |= TIM_CCER_CC2P;
		ir_state = FALLING_SPACE;
	}
	ir_lastChangeTime = ticks;
}
