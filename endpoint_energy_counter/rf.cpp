/*
 * rf.cpp
 * Copyright (C) 2015 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

// #include "rf.h"

#include <cstdlib>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include <settings.h>
#include <RFM70.h>

#include <spi.h>
SPI spi;
int fdGP = -1;
int fdirq = -1;

uint32_t lastTick = 0;
uint32_t lastCounter = 0;

bool initialized = false;

void rfNewCounterValue(int val);

static struct
{
	uint32_t counter, time, vdd;
} data;

uint8_t rfm70EnableChip()
{
	write(fdGP, "1", 1);
	return 0;
}
uint8_t rfm70DisableChip()
{
	write(fdGP, "0", 1);
	return 0;
}
uint8_t rfm70SPIReadCommand(uint8_t cmd, uint8_t* data, uint8_t len)
{
	return spi.rwData(&cmd, sizeof(cmd), data, len) ? 0 : 1;
}
uint8_t rfm70SPISendCommand(uint8_t cmd, const uint8_t* data, uint8_t len)
{
	uint8_t buf[len + 1];
	buf[0] = cmd;
	memcpy(buf + 1, data, len);
	return spi.rwData(buf, sizeof(buf), 0, 0) ? 0 : 1;
}

uint32_t getTicks()
{
	timeval tv;
	gettimeofday(&tv, 0);
	uint32_t val = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return val;
}

int mygetch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

int rfInit()
{
	if (fdGP == -1)
		close(fdGP);
	if (fdirq == -1)
		close(fdirq);
	if (!spi.open("/dev/spitinyusb.0"))
		return 1;
		
	fdGP = open("/sys/class/gpio/ext/value", O_RDWR);
	if (fdGP == -1)
	{
		printf("unable to open gpio");
		return 1;
	}
	
	if (rfm70Init())
		return 1;
	rfm70EnableChip();
	rfm70EnableFeatures();
	
	uint8_t val = 0;
	val |= (1 << 3);
	rfm70WriteRegisterValue(RFM70_CONFIG, val);
	
	rfm70Set1Mbps();
	rfm70SetCRC2bytes();
	
	rfm70WriteRegisterValue(RFM70_SETUP_RETR, 0xff);
	rfm70WriteRegisterValue(RFM70_RF_CH, 30);
	rfm70WriteRegisterValue(RFM70_RX_PW_P0, 12);
	
	rfm70SetTxAddress("\xe7\xe7\xe7\xe7\xe8", 5);
	rfm70SetRxAddress(0, "\xe7\xe7\xe7\xe7\xe7", 5);
	
	rfm70SwitchToRxMode();
	rfm70PowerUp();
	rfm70EnableChip();
	
	rfm70PrintStatus();
	
	fdirq = open("/dev/uio0", O_RDONLY);
	
	initialized = true;
	return 0;
}

#define ER(x) if(x) { initialized = false; return; }
void rfProcess()
{
	uint8_t status;
	uint32_t t1 = getTicks();
	
	if (!initialized)
	{
		if (!rfInit())
		{
			usleep(1000000);
			return;
		}
	}
	
	char c[4];
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fdirq, &set);
	
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;
	int rv = select(fdirq + 1, &set, NULL, NULL, &timeout);
	if (rv == -1)
	{
		perror("select");
	}
	else if (rv > 0)
	{
		printf("intr!\r\n");
		read(fdirq, c, 4);
	}
	
	// usleep(100000);
	ER(rfm70ReadStatus(&status));
	// printf("test 0x%02x\r\n", status);
	// rfm70ClearStatus();
	if (status & RFM70_STATUS_RX_DR)
	{
		ER(rfm70ClearStatus());
		ER(rfm70ReadRxPayload((uint8_t*)&data, 12));
		
		ER(rfm70WriteRegisterValue(RFM70_STATUS, status));
		ER(rfm70SPISendCommand(RFM70_FLUSH_RX, 0, 0));
		
		uint32_t diff = t1 - lastTick;
		lastTick = t1;
		uint32_t cntDiff = data.counter - lastCounter;
		lastCounter = data.counter;
		
		float power = 3600 * 1000 / (float)diff / 2 * cntDiff;
		
		char stars[] = "++++++++++++++++++++++++++";
		if (cntDiff < 10)
			stars[cntDiff] = 0;
			
		time_t current_time;
		char* c_time_string;
		current_time = time(NULL);
		c_time_string = ctime(&current_time);
		*strchr(c_time_string, '\n') = 0;
		printf("[%s] counter: %u Vcc: %.3f V power: %.2f W diff: %.2f s %s\n",
		       c_time_string, data.counter, data.vdd / 1000.0f, power, diff / 1000.0f, stars);
		rfNewCounterValue(data.counter);
	}
}
