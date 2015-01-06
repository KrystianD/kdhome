#ifndef __PROVIDER_INPUT_H__
#define __PROVIDER_INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void provInputReset();
void provInputRegister();
void provInputProcess(const void* data, int len);
void provInputTmr();
void provInputSendState();

// Notifies input provider about input change
void provInputSetState(int num, int value);
// Sends current state to server
void provInputSendState();

// [callback] Reset state based on current state (pin values), calls above functions
void provInputResetState();

#ifdef __cplusplus
}
#endif

#endif
