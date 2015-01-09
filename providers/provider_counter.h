#ifndef __PROVIDER_COUNTER_H__
#define __PROVIDER_COUNTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void provCounterReset();
void provCounterRegister();
void provCounterProcess(const void* data, int len);
void provCounterTmr();

// 
void provCounterSet(int num, int value);
void provCounterAdd(int num, int value);
// Sends current state to server
void provCounterSendState();

// [callback] Reset state based on current state (pin values), calls above functions
void provCounterResetState();

#ifdef __cplusplus
}
#endif

#endif
