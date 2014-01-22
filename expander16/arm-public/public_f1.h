#ifndef __PUBLIC_F1_H__
#define __PUBLIC_F1_H__

#include "stm32f10x.h"

#define BITBAND_SYSTEM(addr,bit) (volatile uint32_t*)(0x42000000 + ((uint32_t)(addr) - 0x40000000) * 32 + (bit) * 4)

// IO
#define PORTCR_FROM_PORTPIN(port,pin) *(uint32_t*)((uint32_t)port + 0x04 * (pin / 8))

static inline void IO_SET_CNF_MODE (GPIO_TypeDef* port, int pin, uint32_t cnf, uint32_t mode)
{
	uint32_t v = PORTCR_FROM_PORTPIN(port, pin);
	v &= ~(0b1111 << ((pin % 8) * 4));
	v |= ((cnf << 2) | mode) << ((pin % 8) * 4);
	PORTCR_FROM_PORTPIN(port, pin) = v;
}

static inline void IO_PUSH_PULL (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b00, 0b11);
}
static inline void IO_OPEN_DRAIN (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b01, 0b11);
}
static inline void IO_ALT_PUSH_PULL (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b10, 0b11);
}
static inline void IO_ALT_OPEN_DRAIN (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b11, 0b11);
}
static inline void IO_ANALOG (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b00, 0b00);
}
static inline void IO_INPUT (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b01, 0b00);
}
static inline void IO_INPUT_PP (GPIO_TypeDef* port, int pin)
{
	IO_SET_CNF_MODE (port, pin, 0b10, 0b00);
}

static inline void IO_HIGH (GPIO_TypeDef* port, int pin)
{
	// port->ODR |= (1 << pin);
	
	//volatile uint32_t *addr = (uint32_t*)(0x42000000 + ((uint32_t)&port->ODR - 0x40000000) * 32 + pin * 4);
	//*addr = 1;
	
	port->BSRR = 1 << pin;
}
static inline void IO_LOW (GPIO_TypeDef* port, int pin)
{
	// port->ODR &= ~(1 << pin);
	
	//volatile uint32_t *addr = (uint32_t*)(0x42000000 + ((uint32_t)&port->ODR - 0x40000000) * 32 + pin * 4);
	//*addr = 0;
	
	port->BRR = 1 << pin;
}
static inline void IO_TOGGLE (GPIO_TypeDef* port, int pin)
{
	//port->ODR ^= (1 << pin);

	volatile uint32_t *addr = (volatile uint32_t*)(0x42000000 + ((uint32_t)&port->ODR - 0x40000000) * 32 + pin * 4);
	*addr = *addr ^ 1;
}

static inline uint8_t IO_IS_HIGH (GPIO_TypeDef* port, int pin)
{
	return (port->IDR & (1 << pin)) ? 1 : 0;
	// uint32_t *addr = (uint32_t*)(0x42000000 + ((uint32_t)&port->IDR - 0x40000000) * 32 + pin * 4);
	// return *addr == 0 ? 0 : 1;
}
static inline uint8_t IO_IS_LOW (GPIO_TypeDef* port, int pin)
{
	return (port->IDR & (1 << pin)) == 0;
	// uint32_t *addr = (uint32_t*)(0x42000000 + ((uint32_t)&port->IDR - 0x40000000) * 32 + pin * 4);
	// return *addr == 0 ? 1 : 0;
}

#define SET_EXTICR(irq,port) { \
	AFIO->EXTICR[(irq) / 4] &= ~(0b1111 << (((irq) % 4) * 4)); \
	AFIO->EXTICR[(irq) / 4] |= (port) << (((irq) % 4) * 4); }
#define SET_EXTICRl(irq,port) { \
	AFIO->EXTICR[(irq) / 4] &= ~(0b1111 << (((irq) % 4) * 4)); \
	AFIO->EXTICR[(irq) / 4] |= ((port) - 'A') << (((irq) % 4) * 4); }

#endif
