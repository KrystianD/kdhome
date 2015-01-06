#include <public.h>
#include <hardware.h>
#include <settings.h>
#include <delay.h>
#include <myprintf.h>
#include <ow.h>

#include "esp.h"
#include "../expander8/io.h"
#include "../expander8/temp.h"
#include "../expander8/ir.h"
#include "providers.h"

volatile uint32_t ticks = 0;

void cliPutChar(char c)
{
	myputchar(c);
}

void myputchar(int c)
{
	USART1->DR = c;
	while (!(USART1->SR & USART_SR_TC));
}

void main()
{
	SCB->VTOR = 0x08000000;
	
	RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_ADC1EN;
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;
	
	IO_ALT_PUSH_PULL(UART_TX);
	USART1->BRR = USART_BRR(115200);
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	ENABLE_INTERRUPT(USART1_IRQn);
	
	
	// Enable systick
	SysTick->LOAD = SysTick->VAL = (F_CPU / 1000) / 8;
	SysTick->CTRL = /*SysTick_CTRL_CLKSOURCE |*/ SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
	
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
	
	_delay_init();
	_delay_ms(1000);
	volatile int i = 0;
	
	IO_INPUT(IN1);
	IO_INPUT(IN2);
	IO_INPUT(IN3);
	IO_INPUT(IN4);
	IO_INPUT(IN5);
	IO_INPUT(IN6);
	IO_INPUT(IN7);
	IO_INPUT(IN8);
	
	IO_PUSH_PULL(OUT1);
	IO_PUSH_PULL(OUT2);
	IO_PUSH_PULL(OUT3);
	IO_PUSH_PULL(OUT4);
	IO_PUSH_PULL(OUT5);
	IO_PUSH_PULL(OUT6);
	IO_PUSH_PULL(OUT7);
	IO_PUSH_PULL(OUT8);
	
	provInit();
	OW_UART_init();
	// tempInit();
	provTempSetRealSensorsCount(0);
	ioInit();
	irInit();
	espInit();
	
	IO_PUSH_PULL(GPIOB, 15);
	for (;;)
	{
		ioProcess();
		espProcess();
		provTmr();
		// i++;
		// _delay_ms (100);
		
		// myprintf ("AD\r\n");
	}
}

void cli_wifi(const char* args)
{
	// espSendData("ASD", 3);
}
#include <cli.h>
cli_cmd_t cliCommands[] =
{
	{ .cmd = "wifi", .handler = cli_wifi },
	{ 0, 0 }
};
void USART1_Handler()
{
	if (USART1->SR & USART_SR_RXNE)
	{
		uint8_t d = USART1->DR;
		if (d == 0x7f)
		{
			while (!(USART1->SR & USART_SR_RXNE));
			uint8_t d = USART1->DR;
			if (d == 0x7f)
			{
				*((unsigned long*)0x0E000ED0C) = 0x05FA0004;
				while (1);
			}
		}
		
		// espSendPacket(3, "AD\r\n", 4);
		// cliProcessChar(d);
		while (!(USART2->SR & USART_SR_TXE));
		USART2->DR = USART1->DR;
	}
}
void SysTick_Handler()
{
	ticks++;
}
void _errorloop()
{
	while (1)
	{
		// IO_TOGGLE(led);
		_delay_ms(100);
	}
}
