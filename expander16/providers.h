#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#include "utils.h"
#include "providers_settings.h"
#include "buffer.h"
#include "provider_output.h"
#include "provider_input.h"

void provInit();
void provProcess(TByteBuffer* data);

#endif
