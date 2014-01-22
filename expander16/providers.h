#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#include "utils.h"
#include "providers_settings.h"

void provInit();
void provProcess(TByteBuffer* data);

// -------------------- Output provider --------------------

// [callback] Function called when output update is requested
extern void provOutput_setOutputCallback(int num, int enable);
// [callback] Function called after all outputs have been set
extern void provOutput_update();

// -------------------- Input provider --------------------
// Notifies input provider about input change
void provInput_setInput(int num, int value);
// Sends current state to server
void provInput_sendState();

// [callback] Reset state based on current state (pin values)
void provInput_resetState();

// -------------------- IR Provider --------------------
// Notifies ir provider about new IR code
void provIR_newCode(uint32_t code);

#endif
