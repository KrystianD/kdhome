#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

#include "lwip/pbuf.h"

// buffer
typedef struct
{
	struct pbuf* p;
	uint16_t pos;
} TByteBuffer;

void bufferFetch(TByteBuffer* buf, char* val, uint16_t len);
#define BYTEBUFFER_FETCH(buf,val) bufferFetch(buf,(char*)&(val),sizeof(val))

void bufferAppend(TByteBuffer* buf, char* val, uint16_t len);
#define BYTEBUFFER_APPEND(buf,val) bufferAppend(buf,(char*)&(val),sizeof(val))

err_t pbuf_take_offset(struct pbuf *buf, uint16_t offset, const void *dataptr, u16_t len);

// other
static void xtoa (unsigned long val, char *buf, unsigned radix, int is_net);
char * ultoa (unsigned long val, char *buf, int radix);
const char* dec2bin (uint8_t num);
const char* dec2bin16 (uint16_t num);

#endif
