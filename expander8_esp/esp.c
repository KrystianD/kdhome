/*
 * esp.c
 * Copyright (C) 2014 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#include "esp.h"

#include <public.h>
#include <delay.h>
#include <settings.h>
#include <hardware.h>
#include <myprintf.h>

#include "esp_firmware/esp_kdapi.h"
#include "../providers/providers.h"

static char inBuffer[1024];
// static char line[256];
// static volatile linesReady
static volatile int wrPtr = 0, rdPtr = 0;
// static volatile int ready = 0;

// #define ESP_STATE_NOT_READY     0
// #define ESP_STATE_READY         1
// #define ESP_STATE_CONFIGURING   2
// #define ESP_STATE_CONFIGURING_MODE 3
// #define ESP_STATE_CONNECTING_TO_WIFI    4
// #define ESP_STATE_CONNECTING_TO_SERVER    5
// #define ESP_STATE_NOT_CONNECTED 6
// #define ESP_STATE_FINDING_WIFI  7
// #define ESP_STATE_OK            8

// static int state = ESP_STATE_NOT_READY;

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
static uint32_t stateChangeTime = 0;

void espInit()
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	IO_ALT_PUSH_PULL(ESP_TX);
	IO_INPUT(ESP_RX);
	USART2->BRR = USART_BRR(115200);
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	ENABLE_INTERRUPT(USART2_IRQn);
	
	IO_PUSH_PULL(ESP_RST);
	IO_PUSH_PULL(ESP_CH_PD);
	
	IO_LOW(ESP_RST);
	IO_LOW(ESP_CH_PD);
	_delay_ms(100);
	IO_HIGH(ESP_RST);
	IO_HIGH(ESP_CH_PD);
	_delay_ms(1000);
}

void espProcessCommand(unsigned char cmd, const unsigned char* buf, int len);
void esp_send(const uint8_t* buf, int len)
{
	while (len--)
	{
		while (!(USART2->SR & USART_SR_TXE));
		USART2->DR = *buf++;
	}
}
static void processChar(unsigned char c);
void espProcess()
{
	static uint32_t lastCheck = 0;
	if (ticks - lastCheck >= 20)
	{
		lastCheck = ticks;
		espSendPacket(ESP_CMD_READ_STATUS, 0, 0);
	}
	
	{
		while (rdPtr != wrPtr)
		{
			processChar(inBuffer[rdPtr]);
			__disable_irq();
			rdPtr++;
			rdPtr %= sizeof(inBuffer);
			__enable_irq();
		}
	}
	
	if (state != STATE_NONE && ticks - startTime >= 200)
	{
		state = STATE_NONE;
	}
	
	
	
	static uint32_t lastCheck2 = 0;
	
	if (ticks - lastCheck2 >= 1000)
	{
		lastCheck2 = ticks;
		
		TProvHeader header;
		provPrepareHeader(&header);
		
		header.type = PROVIDER_TYPE_CONTROL;
		header.cmd = 0;
		provSendPacket(&header, sizeof(header));
	}
}

void USART2_Handler()
{
	if (USART2->SR & USART_SR_RXNE)
	{
		uint8_t d = USART2->DR;
		// USART1->DR = d;
		inBuffer[wrPtr++] = d;
		wrPtr %= sizeof(inBuffer);
	}
	if (USART2->SR & USART_SR_ORE)
	{
		uint8_t d = USART2->DR;
	}
}

static void processChar(unsigned char c)
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
			if (len > 0)
				espProcessCommand(buffer[0], buffer + 1, len - 1);
		}
		else
		{
			// uart0_sendStr("chk bad\r\n");
		}
		state = STATE_NONE;
	}
}

void espSendPacket(unsigned char cmd, const unsigned char* buf, int len)
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
	
	esp_send(tmpBuf, 3);
	esp_send(&cmd, 1);
	esp_send(buf, len);
	esp_send(&chk, 1);
}

int txAvail = 1;
void espProcessCommand(unsigned char cmd, const unsigned char* buf, int len)
{
	if (cmd == ESP_CMD_READ_STATUS)
	{
		uint8_t state1 = buf[0];
		uint8_t state2 = buf[1];
		uint8_t rxAvail = buf[2];
		uint8_t _txAvail = buf[3];
		// myprintf("%d,%d,%d\r\n", state1, state2, rxAvail);
		if (rxAvail)
		{
			espSendPacket(ESP_CMD_READ_FRAME, 0, 0);
		}
		if (_txAvail)
		{
			txAvail = 1;
		}
	}
	else if (cmd == ESP_CMD_READ_FRAME)
	{
		myprintf("new data len: %d\r\n", len);
		provProcess(buf, len);
	}
}
void provSendPacket(const void* buffer, int len)
{
	espSendPacket(ESP_CMD_SEND_FRAME, buffer, len);
	txAvail = 0;
}
