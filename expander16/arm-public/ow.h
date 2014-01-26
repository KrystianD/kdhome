#ifndef __OW_H__
#define __OW_H__

#include <public.h>
#include "settings.h"
#include "hardware.h"

// USART1, USART2, ...
#ifndef OW_USART
#	error OW_USART must be defined
#endif
// APB1ENR, ...
#ifndef OW_USART_APB
#	error OW_USART_APB must be defined
#endif
// RCC_APB1ENR_USART2EN, ...
#ifndef OW_USART_APBEN
#	error OW_USART_APBEN must be defined
#endif

static inline void OW_UART_init ()
{
	RCC->OW_USART_APB |= OW_USART_APBEN;
}

void OW_UART_set9600 ();
void OW_UART_set115200 ();
uint8_t OW_UART_resetPulse ();
uint8_t OW_UART_readBit ();
uint8_t OW_UART_writeBit (uint8_t b);
void OW_UART_writeByte (uint8_t byte);
uint8_t OW_UART_readByte ();
uint8_t OW_crc8_update (uint8_t crc, char d);

#endif
