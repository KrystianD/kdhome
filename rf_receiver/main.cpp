#include <cstdlib>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include <settings.h>
#include <RFM70.h>

#include <spi.h>
SPI spi;
int fdGP;

struct
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
	
	uint8_t val;
	rfm70ReadRegisterValue(RFM70_RF_SETUP, &val);
	val |= (1 << 3);
	rfm70WriteRegisterValue(RFM70_RF_SETUP, val);
	rfm70WriteRegisterValue(RFM70_SETUP_RETR, 0xff);
	rfm70WriteRegisterValue(RFM70_RF_CH, 10);
	rfm70WriteRegisterValue(RFM70_RX_PW_P0, 12);
	
	rfm70SetTxAddress("\xe7\xe7\xe7\xe7\xe8", 5);
	rfm70SetRxAddress(0, "\xe7\xe7\xe7\xe7\xe7", 5);
	
	rfm70SwitchToRxMode();
	rfm70PowerUp();
	rfm70PrintStatus();
	rfm70EnableChip();
	
	uint32_t lastTick = 0;
	uint32_t lastCounter = 0;
	uint8_t status;
	for (;;)
	{
		uint32_t t1 = getTicks();
		rfm70ReadStatus(&status);
		if (status & RFM70_STATUS_RX_DR)
		{
			rfm70ReadRxPayload((uint8_t*)&data, 12);
			printf("%u %u %u \n", data.counter, data.time, data.vdd);
			
			rfm70WriteRegisterValue(RFM70_STATUS, status);
			rfm70SPISendCommand(RFM70_FLUSH_RX, 0, 0);
			
			
			uint32_t diff = t1 - lastTick;
			lastTick = t1;
			uint32_t cntDiff = data.counter - lastCounter;
			lastCounter = data.counter;
			
			float power = 3600 * 1000 / (float)diff / 2 * cntDiff;
			
			printf("%.2f\r\n", power);
		}
		// rfm70PrintStatus();
		// printf("\n\n%llu\n\n", t1);
		// usleep(1000000);
	}
	
	// uint32_t t1 = getTicks();
	// uint8_t dd[32];
	// memset(dd, 0, 32);
	// for (;;)
	// {
	// char c = mygetch();
	// if (c == 'q')
	// {
	// rfm70PrintStatus();
	// }
	// if (c == 's')
	// {
// sendmore:
	// printf("\n%d\n", time(0));
	// printf("Flushing TX...\n");
	// rfm70WriteRegisterValue(RFM70_SETUP_RETR, 0xff);
	// rfm70WriteRegisterValue(RFM70_RF_CH, 60);
	// rfm70SPISendCommand(RFM70_FLUSH_TX, 0, 0);
	// printf("Sending payload...\n");
	// rfm70WriteTxPayload(dd, 32);
	// usleep(10 * 1000);
	// // rfm70PrintStatus ();
// more:
	// val = rfm70ReadRegisterValue(RFM70_STATUS);
	// // printf("status: %s\n", bin(val));
	// if (val & (1 << 4)) // MAX_RT
	// {
	// printf("MAX_RT!!!\n");
	// // rfm70PrintStatus ();
	// // rfm70WriteRegisterValue (RFM70_SETUP_RETR, 0xff);
	// // rfm70WriteRegisterValue (RFM70_RF_CH, 60);
	// rfm70WriteRegisterValue(RFM70_STATUS, val);
	// // rfm70PrintStatus ();
	// // rfm70WriteRegisterValue (RFM70_RF_CH, 60);
	// goto more;
	// // continue;
	// }
	// if (!(val & (1 << 5))) // TX_DS
	// {
	// printf("still sending!!!\n");
	// usleep(1000000);
	// goto more;
	// }
	// rfm70WriteRegisterValue(RFM70_STATUS, val);
	// (*((uint32_t*)dd))++;
	// }
	// usleep(10000);
	// }
	// rfm70EnableChip ();
	// RFM70_disableChip ();
	
	// usleep(100 * 1000);
	// rfm70PrintStatus();
	// uint32_t t2 = getTicks();
	// printf("%u\n", t2 - t1);
	
	
	// // // libusb_close (dev);
	
	// libusb_exit (0);
}
