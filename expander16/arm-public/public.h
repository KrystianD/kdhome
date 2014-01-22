#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// common

typedef uint32_t u32;
typedef int32_t  s32;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint8_t  u8;
typedef int8_t   s8;

extern volatile uint32_t ticks;

void _errorloop () __attribute__ ((noreturn));

#define SWAP16(x) (((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))

// Interrupts
#define ENABLE_INTERRUPT(x) NVIC->ISER[(x) / 32] = 1 << ((x) % 32)
#define DISABLE_INTERRUPT(x) NVIC->ICER[(x) / 32] = 1 << ((x) % 32)

// USART
#define USART_BRR(x) (((F_CPU/(16*(x))) << 4) | \
	(int)((((float)F_CPU/(float)(16*(x))) - (int)((float)F_CPU/(float)(16*(x)))) * 16.0f + 0.5f))
#define USART_BRR_FCPU(fcpu,x) (((fcpu/(16*(x))) << 4) | \
	(int)((((float)fcpu/(float)(16*(x))) - (int)((float)fcpu/(float)(16*(x)))) * 16.0f + 0.5f))

#define M_PI 3.141592f
#define M_SQ2_2 0.70710678f

static float d2r (float v) { return v * M_PI / 180.0f; }
static float r2d (float v) { return v * 180.0f / M_PI; }

static void st_delay_ms (uint32_t ms)
{
	uint32_t end = ticks + ms;
	while (ticks < end);
}

#define AF0 0
#define AF1 1
#define AF2 2
#define AF3 3
#define AF4 4
#define AF5 5
#define AF6 6
#define AF7 7
#define AF8 8
#define AF9 9
#define AF10 10
#define AF11 11
#define AF12 12
#define AF13 13
#define AF14 14
#define AF15 15

// specific
#if defined(STM32F10X_MD_VL) || defined(STM32F10X_MD)
#include <public_f1.h>
#elif defined(STM32L1XX_MD)
#include <public_l1.h>
#elif defined(STM32F0XX_MD) || defined(STM32F0XX_LD)
#include <public_f0.h>
#elif defined(STM32F40_41xxx)
#include <public_f4.h>
#else
#error NO_PART
#endif

#endif
