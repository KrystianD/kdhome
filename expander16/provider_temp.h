#ifndef __PROVIDER_INPUT_H__
#define __PROVIDER_INPUT_H__

#include "buffer.h"

// uint8_t prov_inputLow[INPUTS_COUNT], prov_inputHigh[INPUTS_COUNT];

void provTempReset();
void provTempProcess(TByteBuffer* data);

void provTempSetValueIntFrac(int num, int16_t integral, uint16_t frac);
void provTempSetValueFloat(int num, float value);
void provTempSetError(int num);

#endif
