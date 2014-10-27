#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include <stdint.h>

#include "utils.h"

uint16_t ethNextPacketPtr;
uint16_t ethSessKey;
uint16_t ethPacketId;

void ethInit();
void ethProcess();

#include "kdhome.h"

void provSendPacket(const void* buffer, int len);

#endif
