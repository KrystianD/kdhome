#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include <stdint.h>

#include "utils.h"

uint16_t ethNextPacketPtr;
uint16_t ethSessKey;
uint16_t ethPacketId;

void ethInit();
void ethProcess();

int ethPrepareBuffer(TByteBuffer* buffer, uint16_t len);
void ethFreeBuffer(TByteBuffer* buffer);
void ethSendPacket(TByteBuffer* buffer);

#endif
