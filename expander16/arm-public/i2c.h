#ifndef __I2C_H__
#define __I2C_H__

#include <public.h>

#define I2C_SUCCESS     0
#define I2C_ERROR       1
#define I2C_ERROR_START 2
#define I2C_ERROR_ADDR  3

void i2cInit ();
void i2cDeinit ();

uint8_t i2cStart (uint8_t addr);
// uint8_t i2cStop ();
uint8_t i2cWrite (uint8_t data);
uint8_t i2cRead (uint8_t* data);
void i2cSetACK ();
void i2cSetNACK ();
void i2cSetStop ();
uint8_t i2cWaitUntilStop ();

uint8_t i2cWriteData (uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t cnt);
uint8_t i2cReadData (uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t cnt);
uint8_t i2cWriteDataNoReg (uint8_t addr, uint8_t *buffer, uint8_t cnt);
uint8_t i2cReadDataNoReg (uint8_t addr, uint8_t *buffer, uint8_t cnt);

#endif
