#ifndef __PROVIDER_OUTPUT_H__
#define __PROVIDER_OUTPUT_H__

#include "buffer.h"

void provOutputReset();
void provOutputProcess(TByteBuffer* data);

// [callback] Function called when output update is requested
extern void provOutputSetOutput(int num, int enable);
// [callback] Function called after all outputs have been set
extern void provOutputUpdate();

#endif
