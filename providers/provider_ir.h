#ifndef __PROVIDER_IR_H__
#define __PROVIDER_IR_H__

#include <stdint.h>

void provIRReset();
void provIRProcess(const void* data, int len);
void provIRTmr();

// Notifies ir provider about new IR code
void provIRNewCode(uint32_t code);

#endif
