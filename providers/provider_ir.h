#ifndef __PROVIDER_IR_H__
#define __PROVIDER_IR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void provIRReset();
void provIRRegister();
void provIRProcess(const void* data, int len);
void provIRTmr();

// Notifies ir provider about new IR code
void provIRNewCode(uint32_t code);

#ifdef __cplusplus
}
#endif

#endif
