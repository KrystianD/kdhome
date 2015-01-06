/*
 * user_comm.c
 * Copyright (C) 2015 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#include "user_comm.h"

#include "user_main.h"
#include "driver/uart.h"
#include "osapi.h"

#include "user_wifi.h"

#define STATE_NONE         0
#define STATE_READING_LEN1 1
#define STATE_READING_LEN2 2
#define STATE_READING_PAYLOAD 3
#define STATE_READING_CHK  4

static int state = STATE_NONE;

static unsigned char buffer[1024];
static unsigned char chk;
static int len, cnt;
static unsigned long startTime;

void ICACHE_FLASH_ATTR
commProcessChar(unsigned char c)
{
	if (state == STATE_NONE)
	{
		if (c == 'X')
		{
			// uart0_sendStr("x\r\n");
			state = STATE_READING_LEN1;
			len = 0;
			startTime = ticks;
		}
	}
	else if (state == STATE_READING_LEN1)
	{
		// uart0_sendStr("l1\r\n");
		len = c;
		startTime = ticks;
		state = STATE_READING_LEN2;
	}
	else if (state == STATE_READING_LEN2)
	{
		// uart0_sendStr("l2\r\n");
		len |= ((int)c << 8);
		startTime = ticks;
		chk = 0;
		// char text[100];
		// os_sprintf(text, "len: %d\r\n", len);
		// uart0_sendStr(text);
		if (len < sizeof(buffer))
		{
			state = STATE_READING_PAYLOAD;
			cnt = 0;
		}
		else
		{
			state = STATE_NONE;
		}
	}
	else if (state == STATE_READING_PAYLOAD)
	{
		// uart0_sendStr("pa\r\n");
		buffer[cnt++] = c;
		startTime = ticks;
		chk += c;
		if (cnt == len)
		{
			state = STATE_READING_CHK;
		}
	}
	else if (state == STATE_READING_CHK)
	{
		if (chk == c)
		{
			// uart0_sendStr("chk ok\r\n");
			if (len > 0)
				commProcessCommand(buffer[0], buffer + 1, len - 1);
		}
		else
		{
			// uart0_sendStr("chk bad\r\n");
		}
		state = STATE_NONE;
	}
}
void ICACHE_FLASH_ATTR
commProcess()
{
	if (state != STATE_NONE && ticks - startTime >= 200)
	{
		// uart0_sendStr("rst\r\n");
		state = STATE_NONE;
	}
}

void commSendPacket(unsigned char cmd, const unsigned char* buf, int len)
{
	unsigned char tmpBuf[1 + 2 + sizeof(buf)];
	tmpBuf[0] = 'X';
	tmpBuf[1] = (len + 1) & 0xff;
	tmpBuf[2] = ((len + 1) >> 8) & 0xff;
	
	unsigned char chk = cmd;
	int i = 0;
	for (i = 0; i < len; i++)
	{
		chk += buf[i];
	}
	
	uart0_tx_buffer(tmpBuf, 3);
	uart0_tx_buffer(&cmd, 1);
	uart0_tx_buffer(buf, len);
	uart0_tx_buffer(&chk, 1);
}
