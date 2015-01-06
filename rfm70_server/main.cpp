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
int fdGP;

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
	// for (int i = 0; i < sizeof(buf); i++)
	// printf("0x%02x, ", buf[i]);
	// printf("\n");
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
int main(int argc, char** argv)
{
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
	// rfm70DisableChip();
	// rfm70PowerDown();
	// rfm70PrintStatus();
	
	// rfm70WriteRegisterValue(RFM70_CONFIG, 0x00);
	uint8_t val = 0;
	// rfm70ReadRegisterValue(RFM70_CONFIG, &val);
	val |= (1 << 3);
	// val |= (1 << 5);
	// val |= (1 << 4);
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
	
	int fdirq = open("/dev/uio0", O_RDONLY);
	
	uint32_t lastTick = 0;
	uint32_t lastCounter = 0;
	uint8_t status;
	for (;;)
	{
		uint32_t t1 = getTicks();
		
		char c[4];
		fd_set set;
		FD_ZERO(&set); /* clear the set */
		FD_SET(fdirq, &set); /* add our file descriptor to the set */
		
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		int rv = select(fdirq + 1, &set, NULL, NULL, &timeout);
		if (rv == -1)
			perror("select"); /* an error accured */
		// else if (rv == 0)
			// printf("timeout\r\n"); /* a timeout occured */
		else if (rv > 0)
		{
			// printf("intr!\r\n");
			read(fdirq, c, 4);
		}
			
		// usleep(100000);
		rfm70ReadStatus(&status);
		// printf("test 0x%02x\r\n", status);
		// rfm70ClearStatus();
		if (status & RFM70_STATUS_RX_DR)
		{
			rfm70ClearStatus();
			rfm70ReadRxPayload((uint8_t*)&data, 12);
			
			rfm70WriteRegisterValue(RFM70_STATUS, status);
			rfm70SPISendCommand(RFM70_FLUSH_RX, 0, 0);
			
			uint32_t diff = t1 - lastTick;
			lastTick = t1;
			uint32_t cntDiff = data.counter - lastCounter;
			lastCounter = data.counter;
			
			float power = 3600 * 1000 / (float)diff / 2 * cntDiff;
			
			char stars[] = "++++++++++++++++++++++++++";
			if (cntDiff < 10)
				stars[cntDiff] = 0;
				
			printf("counter: %u Vcc: %.3f V power: %.2f W diff: %.2f s %s\n", data.counter, data.vdd / 1000.0f, power, diff / 1000.0f, stars);
		}
	}
}
