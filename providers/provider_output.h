#ifndef __PROVIDER_OUTPUT_H__
#define __PROVIDER_OUTPUT_H__

#include <stdint.h>
#include "kdhome.h"

void provOutputReset();
void provOutputRegister();
void provOutputProcess(const void* data, int len);
void provOutputTmr();

// [callback] Function called when output update is requested
extern void provOutputSetOutput(int num, int enable);
// [callback] Function called after all outputs have been set
extern void provOutputUpdate();

#endif
