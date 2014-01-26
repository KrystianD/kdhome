#include <public.h>
#include <hardware.h>
#include <settings.h>
#include <delay.h>
#include <myprintf.h>
#include <ow.h>

#include <ENC28J60.h>

#include "utils.h"
#include "ethernet.h"
#include "ir.h"
#include "io.h"
#include "temp.h"
#include "providers.h"

volatile uint32_t ticks = 0;
int dodump = 0;

void myputchar(int c)
{
	USART1->DR = c;
	while (!(USART1->SR & USART_SR_TC));
}

void main()
{
	SCB->VTOR = 0x08000000;
	
	// set clock source as HSI / 2 *(PLL) 4
	RCC->CFGR |= RCC_CFGR_PLLMULL8 | RCC_CFGR_PLLXTPRE_HSE_Div2 /*| RCC_CFGR_PLLSRC*/;
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));

	// enable peripherals
	RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;

	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 
	
	// configure USART
	IO_ALT_PUSH_PULL(UART_TX);
	USART1->BRR = USART_BRR(230400);
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	ENABLE_INTERRUPT(USART1_IRQn);

	// enable systick
	SysTick->LOAD = SysTick->VAL = (F_CPU / 1000) / 8;
	SysTick->CTRL = /*SysTick_CTRL_CLKSOURCE |*/ SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;

	_delay_init();
	
	IO_PUSH_PULL(LED);
	IO_HIGH(LED);

#ifndef ETHERNET_MODULE
	OW_UART_init();
	tempInit();
	ioInit();
	irInit();
#endif
#ifdef ETHERNET
	ethInit();
#endif


	uint8_t b;
	uint32_t lastCheck = 0;
	for (;;)
	{
		// if (IO_IS_LOW(IN1)) dodump = 1;
		// else dodump = 0;
		// dodump=1;
		
		static int q=0;
		if(ticks-q>=1000)
		{
			myprintf("ASD\r\n");
			q=ticks;
		}

		provTempSetValueIntFrac(0,5, 0x8000);
		provTempSetValueFloat(1, 5.124f);
		provTempSetValueIntFrac(2, ((int16_t)(0xff5 << 4)) >> 4, 0xe << 12);
#ifndef ETHERNET_MODULE
		ioProcess();
	
		tempProcess();

		irProcess();
#endif
		provTmr();
#ifdef ETHERNET
		ethProcess();
#endif
	}
}

void USART1_Handler()
{
	if (USART1->SR & USART_SR_RXNE)
	{
		uint8_t d = USART1->DR;
		if (d == 0x7f)
		{
			*((unsigned long*)0x0E000ED0C) = 0x05FA0004;
			while(1);
		}
	}
}

void SysTick_Handler()
{
	ticks++;
}
void _errorloop()
{
	while(1)
	{
		IO_TOGGLE(LED);
		_delay_ms(100);
	}
}
