#ifndef __PROVIDER_TEMP_H__
#define __PROVIDER_TEMP_H__

#include "buffer.h"

void provTempReset();
void provTempProcess(TByteBuffer* data);
void provTempTmr();

void provTempSetValueIntFrac(int num, int16_t integral, uint16_t frac);
void provTempSetValueFloat(int num, float value);
void provTempSetError(int num);

#endif
