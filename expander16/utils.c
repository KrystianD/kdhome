#include "utils.h"

#include <string.h>

// buffer
#ifdef ETHERNET
err_t pbuf_take_offset(struct pbuf *buf, uint16_t offset, const void *dataptr, u16_t len);

void bufferFetch(TByteBuffer* buf, char* val, uint16_t len)
{
	pbuf_copy_partial(buf->p, val, len, buf->pos);
	buf->pos += len;
}
void bufferAppend(TByteBuffer* buf, char* val, uint16_t len)
{
	pbuf_take_offset(buf->p, buf->pos, val, len);
	buf->pos += len;
}
void bufferPrint(TByteBuffer* buf)
{
	uint16_t i;
	for(i = 0; i < buf->p->tot_len; i++)
		myprintf("0x%02x, ", pbuf_get_at(buf->p, i));
	myprintf("\r\n");
}

err_t pbuf_take_offset(struct pbuf *buf, uint16_t offset, const void *dataptr, u16_t len)
{
  struct pbuf *p;
  u16_t buf_copy_len;
  u16_t total_copy_len = len;
  u16_t copied_total = 0;

  LWIP_ERROR("pbuf_take: invalid buf", (buf != NULL), return 0;);
  LWIP_ERROR("pbuf_take: invalid dataptr", (dataptr != NULL), return 0;);

  if ((buf == NULL) || (dataptr == NULL) || (buf->tot_len < len)) {
    return ERR_ARG;
  }

  /* Note some systems use byte copy if dataptr or one of the pbuf payload pointers are unaligned. */
  for(p = buf; total_copy_len != 0; p = p->next) {
		if ((offset != 0) && (offset >= p->len)) {
      /* don't copy from this buffer -> on to the next */
      offset -= p->len;
    } else {
			LWIP_ASSERT("pbuf_take: invalid pbuf", p != NULL);
			buf_copy_len = total_copy_len;
			if (buf_copy_len > p->len - offset) {
				/* this pbuf cannot hold all remaining data */
				buf_copy_len = p->len - offset;
			}
			/* copy the necessary parts of the buffer */
			MEMCPY(p->payload + offset, &((char*)dataptr)[copied_total], buf_copy_len);
			total_copy_len -= buf_copy_len;
			copied_total += buf_copy_len;
			offset = 0;
    }
  }
  LWIP_ASSERT("did not copy all data", total_copy_len == 0 && copied_total == len);
  return ERR_OK;
}
#endif

// other
static void xtoa (unsigned long val, char *buf, unsigned radix, int is_neg)
{
	char *p;                /* pointer to traverse string */
	char *firstdig;         /* pointer to first digit */
	char temp;              /* temp char */
	unsigned digval;        /* value of digit */

	p = buf;

	if (is_neg) {
		/* negative, so output '-' and negate */
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	firstdig = p;           /* save pointer to first digit */

	do {
		digval = (unsigned) (val % radix);
		val /= radix;       /* get next digit */

		/* convert to ascii and store */
		if (digval > 9)
			*p++ = (char) (digval - 10 + 'a');  /* a letter */
		else
			*p++ = (char) (digval + '0');       /* a digit */
	} while (val > 0);

	/* We now have the digit of the number in the buffer, but in reverse
		 order.  Thus we reverse them now. */

	*p-- = '\0';            /* terminate string; p points to last digit */

	do {
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;   /* swap *p and *firstdig */
		--p;
		++firstdig;         /* advance to next two digits */
	} while (firstdig < p); /* repeat until halfway */
}


char * ultoa (unsigned long val, char *buf, int radix)
{
	xtoa(val, buf, radix, 0);
	return buf;
}
const char* dec2bin (uint8_t num)
{
	static char txt[9];
	char str[9];
	strcpy (txt, "00000000");
	ultoa (num, str, 2);
	str[sizeof (str) - 1] = 0;
	strcpy (txt + 8 - strlen (str), str);
	return txt;
}
const char* dec2bin16 (uint16_t num)
{
	static char txt[16 + 1];
	char str[16 + 1];
	strcpy (txt, "0000000000000000");
	ultoa (num, str, 2);
	str[sizeof (str) - 1] = 0;
	strcpy (txt + 16 - strlen (str), str);
	return txt;
}
