#ifndef __PROVIDER_OUTPUT_H__
#define __PROVIDER_OUTPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void provOutputReset();
void provOutputRegister();
void provOutputProcess(const void* data, int len);
void provOutputTmr();

// [callback] Function called when output update is requested
extern void provOutputSetOutput(int num, int enable);
// [callback] Function called after all outputs have been set
extern void provOutputUpdate();

#ifdef __cplusplus
}
#endif

#endif
