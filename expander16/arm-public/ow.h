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

#endif
