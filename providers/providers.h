#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#include "utils.h"
#include "providers_settings.h"
#include "buffer.h"
#include "provider_output.h"
#include "provider_input.h"
#include "provider_ir.h"
#include "provider_temp.h"
#include "kdhome.h"

extern volatile uint32_t ticks;

void provInit();
void provProcess(const char* data, int len);
void provTmr();

void provPrepareHeader(TProvHeader* header);

#endif
