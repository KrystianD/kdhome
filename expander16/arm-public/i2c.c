#include "i2c.h"

#include <hardware.h>

// typedef struct
// {
	// I2C_TypeDef* i2c;
	// GPIO_TypeDef* port;
	// int pin;
// } I2Cobj;

void i2cInit ()//I2Cobj* obj, I2C_TypeDef* i2c, GPIO_TypeDef* port, int pin)
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	IO_OPEN_DRAIN(SCL);
	IO_OPEN_DRAIN(SDA);
	IO_HIGH(SCL);
	IO_HIGH(SDA);

	while(IO_IS_LOW(SDA))
	{
		IO_LOW(SCL);
		_delay_us(10);
		IO_HIGH(SCL);
		_delay_us(10);
	}

	// _delay_ms (100);

	IO_ALT_OPEN_DRAIN(SCL);
	IO_ALT_OPEN_DRAIN(SDA);

	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
	_delay_us(1);
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;
	_delay_us(1);
	// obj->i2c = i2c;
	// obj->port = port;
	// obj->pin = pin;



	// for(;;)
	// {
		// _delay_ms(500);
		// IO_TOGGLE(SDA);
		// IO_TOGGLE(SCL);
	// }

	I2C_OBJ->CR1 = 8;
	I2C_OBJ->CCR = 0x28;
	I2C_OBJ->TRISE = 0x09;
	I2C_OBJ->CR1 |= I2C_CR1_PE;

	// for(;;)
	// {
			// myprintf("%02x %02x %02x\r\n", I2C_OBJ->CR1, I2C_OBJ->SR1, I2C_OBJ->SR2);
		// // _delay_ms(500);
		// // IO_TOGGLE(SDA);
		// // IO_TOGGLE(SCL);
	// }
}
void i2cDeinit ()
{
	I2C_OBJ->CR1 = 0;

	IO_OPEN_DRAIN(SCL);
	IO_OPEN_DRAIN(SDA);
	IO_HIGH(SCL);
	IO_HIGH(SDA);

	int i;
	for (i = 0; i < 10; i++)
	{
		IO_TOGGLE(SCL);
		_delay_us (10);
	}
	for (i = 0; i < 10; i++)
	{
		IO_TOGGLE(SDA);
		_delay_us (10);
	}
	for (i = 0; i < 10; i++)
	{
		IO_TOGGLE(SCL);
		_delay_us (10);
	}
	for (i = 0; i < 10; i++)
	{
		IO_TOGGLE(SDA);
		_delay_us (10);
	}
	IO_HIGH(SCL);
	IO_HIGH(SDA);
}

// #define I2CD(x) myprintf(x"\r\n");

#ifndef I2CD
#define I2CD(x)
#endif

uint8_t i2cStart (uint8_t addr)
{
	uint32_t t;

	I2C_OBJ->CR1 |= I2C_CR1_START;
	t = ticks;
	while (!(I2C_OBJ->SR1 & I2C_SR1_SB))
	{
		I2CD("s1");
		if (ticks - t > 10)
		{
			myprintf("%02x %02x %02x\r\n", I2C_OBJ->CR1, I2C_OBJ->SR1, I2C_OBJ->SR2);
			return I2C_ERROR_START;
		}
	}

	I2C_OBJ->DR = addr;
	t = ticks;
	while (!(I2C_OBJ->SR1 & I2C_SR1_ADDR))
	{
		I2CD("s2");
		if (ticks - t > 10)
			return I2C_ERROR_ADDR;
	}
	uint8_t q = I2C_OBJ->SR2;
	return I2C_SUCCESS;
}
uint8_t i2cWrite (uint8_t data)
{
	I2C_OBJ->DR = data;
	uint32_t t = ticks;
	while (!(I2C_OBJ->SR1 & I2C_SR1_TXE))
	{
		I2CD("w1");
		if (ticks - t > 10)
			return I2C_ERROR;
	}
	return I2C_SUCCESS;
}
uint8_t i2cRead (uint8_t* data)
{
	uint32_t t = ticks;
	while (!(I2C_OBJ->SR1 & I2C_SR1_RXNE))
	{
		I2CD("r1");
		if (ticks - t > 10)
			return I2C_ERROR;
	}
	*data = I2C_OBJ->DR;
	return I2C_SUCCESS;
}
inline void i2cSetACK ()
{
	I2C_OBJ->CR1 |= I2C_CR1_ACK;
}
inline void i2cSetNACK ()
{
	I2C_OBJ->CR1 &= ~I2C_CR1_ACK;
}
inline void i2cSetStop ()
{
	I2C_OBJ->CR1 |= I2C_CR1_STOP;
}
inline uint8_t i2cWaitUntilStop ()
{
	while ((I2C_OBJ->SR2 & I2C_SR2_MSL)) I2CD("u1");
}

uint8_t i2cWriteData (uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t cnt)
{
	if (i2cStart (addr | 0)) return I2C_ERROR;
	i2cSetACK ();
	if (i2cWrite (reg))      return I2C_ERROR;
	while (cnt--)
	{
		if (i2cWrite (*buffer++)) return I2C_ERROR;
	}
	i2cSetStop ();
	if (i2cWaitUntilStop ()) return I2C_ERROR;
	return I2C_SUCCESS;
}
uint8_t i2cReadData (uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t cnt)
{
	if (i2cStart (addr | 0))   return I2C_ERROR;
	if (i2cWrite (reg))        return I2C_ERROR;

	if (cnt == 1)
	{
		if (i2cStart (addr | 1)) return I2C_ERROR;
		i2cSetNACK ();
		i2cSetStop ();
		if (i2cRead (buffer))    return I2C_ERROR;
		if (i2cWaitUntilStop ()) return I2C_ERROR;
		return I2C_SUCCESS;
	}
	else
	{
		if (i2cStart (addr | 1)) return I2C_ERROR;
		i2cSetACK ();
		for (;;)
		{
			if (cnt == 2)
			{
				if (i2cRead (buffer))    return I2C_ERROR;
				i2cSetNACK ();
				i2cSetStop ();
				buffer++;
				if (i2cRead (buffer))    return I2C_ERROR;
				if (i2cWaitUntilStop ()) return I2C_ERROR;
				return I2C_SUCCESS;
			}
			else
			{
				if (i2cRead (buffer))    return I2C_ERROR;
				buffer++;
				cnt--;
			}
		}
	}
}
uint8_t i2cWriteDataNoReg (uint8_t addr, uint8_t *buffer, uint8_t cnt)
{
	if (i2cStart (addr | 0)) return I2C_ERROR;
	i2cSetACK ();
	while (cnt--)
	{
		if (i2cWrite (*buffer++)) return I2C_ERROR;
	}
	i2cSetStop ();
	if (i2cWaitUntilStop ()) return I2C_ERROR;
	return I2C_SUCCESS;
}
uint8_t i2cReadDataNoReg (uint8_t addr, uint8_t *buffer, uint8_t cnt)
{
	if (cnt == 1)
	{
		if (i2cStart (addr | 1)) return I2C_ERROR;
		i2cSetNACK ();
		i2cSetStop ();
		if (i2cRead (buffer))    return I2C_ERROR;
		if (i2cWaitUntilStop ()) return I2C_ERROR;
		return I2C_SUCCESS;
	}
	else
	{
		if (i2cStart (addr | 1)) return I2C_ERROR;
		i2cSetACK ();
		for (;;)
		{
			if (cnt == 2)
			{
				if (i2cRead (buffer))    return I2C_ERROR;
				i2cSetNACK ();
				i2cSetStop ();
				buffer++;
				if (i2cRead (buffer))    return I2C_ERROR;
				if (i2cWaitUntilStop ()) return I2C_ERROR;
				return I2C_SUCCESS;
			}
			else
			{
				if (i2cRead (buffer))    return I2C_ERROR;
				buffer++;
				cnt--;
			}
		}
	}
}
