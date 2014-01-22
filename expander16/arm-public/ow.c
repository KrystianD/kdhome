
#include <ow.h>

void OW_UART_set9600 ()
{
	IO_ALT_OPEN_DRAIN(OW);
	IO_HIGH(OW);
	OW_USART->BRR = USART_BRR(9600);
	OW_USART->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	OW_USART->CR3 = USART_CR3_HDSEL;
	uint8_t v = USART2->DR;
}
void OW_UART_set115200 ()
{
	IO_ALT_OPEN_DRAIN(OW);
	IO_HIGH(OW);
	OW_USART->BRR = USART_BRR(115200);
	OW_USART->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	OW_USART->CR3 = USART_CR3_HDSEL;
	uint8_t v = USART2->DR;
}
uint8_t OW_UART_resetPulse ()
{
	uint8_t t = 0;
	OW_USART->DR = 0xf0;
	while (!(OW_USART->SR & USART_SR_RXNE))
	{
		t++;
		if (t == 100)
			return 2;
		_delay_us (200);
	}
	uint8_t v = OW_USART->DR;
	return v != 0xf0 ? 1 : 0;
}
uint8_t OW_UART_readBit ()
{
	uint8_t t = 0;
	OW_USART->DR = 0xff;
	while (!(OW_USART->SR & USART_SR_RXNE))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us (2);
	}
	uint8_t v = OW_USART->DR;
	return v == 0xff ? 1 : 0;
}
uint8_t OW_UART_writeBit (uint8_t b)
{
	uint8_t t = 0;
	OW_USART->DR = b ? 0xff : 0x00;
	while (!(OW_USART->SR & USART_SR_RXNE))
	{
		t++;
		if (t == 100)
			return 0;
		_delay_us (2);
	}
	uint8_t v = OW_USART->DR;
}
void OW_UART_writeByte (uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OW_UART_writeBit (byte & 0x01);
		byte >>= 1;
	}
}
uint8_t OW_UART_readByte ()
{
	uint8_t i, v = 0;
	for (i = 0; i < 8; i++)
	{
		v >>= 1;
		if (OW_UART_readBit ())
			v |= 0x80;
	}
	return v;
}
uint8_t OW_crc8_update (uint8_t crc, char d)
{
	uint8_t j;

	crc ^= d;
	for (j = 0; j < 8; j++)
	{
		if (crc & 0x1)
		{
			crc >>= 1;
			crc ^= 0x8c;
		}
		else
		{
			crc >>= 1;
		}
	}
	return crc;
}
