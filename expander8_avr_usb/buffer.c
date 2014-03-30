#include "buffer.h"
#include <string.h>
// #include <myprintf.h>

// #include "utils.h"

int bufferFetch(TByteBuffer* buf, char* val, uint16_t len)
{
	if (buf->pos + len > BYTEBUFFER_LEN(buf))
		return 1;
	memcpy(val, buf->data + buf->pos, len);
	buf->pos += len;
	return 0;
}
int bufferAppend(TByteBuffer* buf, char* val, uint16_t len)
{
	if (buf->pos + len > BYTEBUFFER_LEN(buf))
		return 1;
	// pbuf_take_offset(buf->p, buf->pos, val, len);
	memcpy(buf->data + buf->pos, val, len);
	buf->pos += len;
	return 0;
}
void bufferPrint(TByteBuffer* buf)
{
	// int i;
	// for (i = 0; i < buf->p->tot_len; i++)
		// myprintf("0x%02x, ", pbuf_get_at(buf->p, i));
	// myprintf("\r\n");
}
