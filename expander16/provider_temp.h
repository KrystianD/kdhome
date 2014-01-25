#ifndef __PROVIDER_INPUT_H__
#define __PROVIDER_INPUT_H__

#include "buffer.h"

void provInputReset();
void provInputProcess(TByteBuffer* data);
void provInputSendState();

// Notifies input provider about input change
void provInputSetState(int num, int value);
// Sends current state to server
void provInputSendState();

// [callback] Reset state based on current state (pin values)
void provInputResetState();

#endif
