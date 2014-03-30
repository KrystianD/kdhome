#ifndef __PROVIDERS_H__
#define __PROVIDERS_H__

#define myprintf(x,...)

// #include "utils.h"
#include "providers_settings.h"
#include "buffer.h"
#include "provider_output.h"
#include "provider_input.h"
#include "provider_ir.h"
#include "provider_temp.h"

extern volatile uint32_t ticks;
extern uint16_t prov_sessKey;
extern uint16_t prov_packetId;

int provPrepareBuffer(TByteBuffer* buffer, uint16_t len);
void provFreeBuffer(TByteBuffer* buffer);
void provSendPacket(TByteBuffer* buffer);

void provInit();
void provProcess(TByteBuffer* data);
void provTmr();

#endif
