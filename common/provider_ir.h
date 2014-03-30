#ifndef __PROVIDER_IR_H__
#define __PROVIDER_IR_H__

#include "buffer.h"

void provIRReset();
void provIRProcess(TByteBuffer* data);
void provIRTmr();

// Notifies ir provider about new IR code
void provIRNewCode(uint32_t code);

#endif
