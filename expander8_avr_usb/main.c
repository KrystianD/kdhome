#include <public.h>

#define USB_PUBLIC static
#include "usbdrv.c"

// #define PRINTF_USB_BUFFER_LENGTH 200
// #define PRINTF_USB_PACKET_LENGTH 20
// #include <printf_usb.h>

#include "advinputmanager.h"

// #include "../usb_expander.h"

uint8_t buffer[1 + ADVIM_PATTERNMAX];
volatile uint16_t bufferIdx, totalIdx;
volatile uint16_t ledTimer = 0;
volatile usbRequest_t grq;
volatile uint8_t timerMs = 0;
volatile uint16_t bytesRemaining;

uint8_t getInputStates ()
{
	uint8_t i;
	uint8_t val = 0;
	for (i = 0; i < 8; i++)
		if (advimIsHigh (i))
			val |= (1 << i);
	return val;
}

#include "buffer.h"
#include "providers.h"
char gdata[300];
usbMsgLen_t usbFunctionSetup (uchar setupData[8])
{
	usbRequest_t *rq = (usbRequest_t*)setupData;
	grq = *rq;
	switch (rq->bRequest)
	{
	case 0:
		usbMsgPtr = gdata;
		return bytesRemaining;
	case 1:
		bufferIdx = 0;
		totalIdx = 0;
		return USB_NO_MSG;
	}
	// case UEXP_REQ_GETSTATUS:
		// buffer[0] = getInputStates ();
		// usbMsgPtr = buffer;
		// return 1;
	// case UEXP_REQ_SETINPUTSETTINGS:
		// bufferIdx = 0;
		// totalIdx = 0;
		// return USB_NO_MSG;
	// case UEXP_REQ_GETCLEARPATTERN:
		// buffer[0] = ADVIM_patternIdx;
		// memcpy (buffer + 1, ADVIM_pattern, ADVIM_PATTERNMAX); 
		// return sizeof (buffer);
	// case UEXP_REG_SETSWITCH:
		// if (rq->wValue.bytes[0] & 0x01) IO_LOW(out1); else IO_HIGH(out1);
		// if (rq->wValue.bytes[0] & 0x02) IO_LOW(out2); else IO_HIGH(out2);
		// if (rq->wValue.bytes[0] & 0x04) IO_LOW(out3); else IO_HIGH(out3);
		// if (rq->wValue.bytes[0] & 0x08) IO_LOW(out4); else IO_HIGH(out4);
		// if (rq->wValue.bytes[0] & 0x10) IO_LOW(out5); else IO_HIGH(out5);
		// if (rq->wValue.bytes[0] & 0x20) IO_LOW(out6); else IO_HIGH(out6);
		// if (rq->wValue.bytes[0] & 0x40) IO_LOW(out7); else IO_HIGH(out7);
		// if (rq->wValue.bytes[0] & 0x80) IO_LOW(out8); else IO_HIGH(out8);
		// IO_HIGH(led); ledTimer = 100;
		// return 0;
	// case 255:
		// cli ();
		// wdt_enable (WDTO_15MS);
		// for (;;);
	// }
#ifdef __PRINTF_USB__
	usbMsgLen_t res;
	if ((res = PRINTF_USB_usbFunctionSetup (setupData)) != 0) return res;
#endif
	return 0;
}
uint8_t usbFunctionRead (uint8_t *data, uint8_t len) // IN
{
	// uchar i;
	// if(len > bytesRemaining)
		// len = bytesRemaining;
	// bytesRemaining -= len;
	// for(i = 0; i < len; i++)
		// data[i] = gdata[bufferIdx++];
	// return len;
}
uint8_t usbFunctionWrite (uint8_t *data, uint8_t len) // OUT
{
	uint8_t i;
	if (grq.bRequest != (uchar)1)
		return 0;

	for (i = 0; i < len; i++)
	{
		gdata[bufferIdx++] = *data++;
	}
	if (bufferIdx == grq.wLength.word)
	{
		IO_HIGH(led); ledTimer = 5000;
		TByteBuffer b;
		b.data = gdata;
		b.pos = 0;
		b.len = grq.wLength.word;

		uint16_t packetId;
		if (BYTEBUFFER_FETCH(&b, packetId)) return;

		provProcess(&b);
		return 1;
	}
	else
	{
		return 0;
	}
	// return totalIdx == grq.wLength.word ? 1 : 0;
}

SIGNAL(TIMER0_OVF_vect)
{
	timerMs++;
}

void onInputHigh (uint8_t idx)
{
	char d[] = { 0 };
	// while (!usbInterruptIsReady ()) usbPoll ();
	usbSetInterrupt (d, 2);
	IO_HIGH(led); ledTimer = 100;
}
void onInputLow (uint8_t idx)
{
	char d[] = { 0 };
	// while (!usbInterruptIsReady ()) usbPoll ();
	usbSetInterrupt (d, 2);
	IO_HIGH(led); ledTimer = 100;
}

int main ()
{
	u8 i;

	wdt_enable (WDTO_2S);

	IO_PUSH_PULL(led);

	usbInit ();
	usbDeviceDisconnect ();
	for (i = 0; i < 10; i++) { IO_TOGGLE(led); _delay_ms (50); }
	usbDeviceConnect ();

#ifdef __PRINTF_USB__
	PRINTF_USB_init ();
#endif

	IO_PUSH_PULL(out1); IO_LOW(out1);
	IO_PUSH_PULL(out2); IO_LOW(out2);
	IO_PUSH_PULL(out3); IO_LOW(out3);
	IO_PUSH_PULL(out4); IO_LOW(out4);
	IO_PUSH_PULL(out5); IO_LOW(out5);
	IO_PUSH_PULL(out6); IO_LOW(out6);
	IO_PUSH_PULL(out7); IO_LOW(out7);
	IO_PUSH_PULL(out8); IO_LOW(out8);

	IO_INPUT(in1);
	IO_INPUT(in2);
	IO_INPUT(in3);
	IO_INPUT(in4);
	IO_INPUT(in5);
	IO_INPUT(in6);
	IO_INPUT(in7);
	IO_INPUT(in8);

	TCCR0 = _BV(CS01) | _BV(CS00);
	TIMSK = _BV(TOIE0);

	advimInit ();

	sei ();

	uint16_t timer = 0;
	uint8_t lastTimerMs = 0;
	for (;;)
	{
		uint8_t diff = timerMs - lastTimerMs;
		lastTimerMs = timerMs;
		timer += diff;

		advimProcess (timer);
		usbPoll ();
		wdt_reset ();

		if (!ledTimer)
			IO_LOW(led);
		else
			ledTimer--;

		static uint32_t lastCheck = 0;

		if (timer - lastCheck >= 1000)
		{
			lastCheck = timer;

			TByteBuffer b;
			if (provPrepareBuffer(&b, 2))
			{
				uint16_t type = 0x0000;
				BYTEBUFFER_APPEND(&b, type);
				provSendPacket(&b);
				provFreeBuffer(&b);
			}
		}
	}
}

void provInputResetState()
{
	// int i;

	// // settings initial state
	// for (i = 0; i < INPUTS_COUNT; i++)
	// {
	// if (io_getInp(i))
	// provInputSetState(i, 1);
	// else
	// provInputSetState(i, 0);
	// }
}
void provOutputSetOutput(int num, int enable)
{
	// if (enable)
	// ioOutputs |= io_getOutMask(num);
	// else
	// ioOutputs &= ~io_getOutMask(num);
}
void provOutputUpdate()
{
	// i2cWriteDataNoReg(PCF_ADDR | PCF_OUT0, (uint8_t*)&ioOutputs, 1);
	// i2cWriteDataNoReg(PCF_ADDR | PCF_OUT1, (uint8_t*)&ioOutputs + 1, 1);
}

int provPrepareBuffer(TByteBuffer* buffer, uint16_t len)
{
	// buffer->p = pbuf_alloc(PBUF_RAW, len + 2 + 2, PBUF_POOL);
	// // buffer->p = pbuf_alloc(PBUF_TRANSPORT, len + 2 + 2, PBUF_POOL);
	// if (!buffer->p)
	// return 0;
	buffer->data = gdata;
	buffer->len = sizeof(gdata);
	buffer->pos = 0;

	BYTEBUFFER_APPEND(buffer, prov_packetId);
	BYTEBUFFER_APPEND(buffer, prov_sessKey);

	prov_packetId++;

	return 1;
}
void provFreeBuffer(TByteBuffer* buffer)
{
	// pbuf_free(buffer->p);
	// buffer->p = 0;
}
void provSendPacket(TByteBuffer* buffer)
{
	bufferIdx = 0;
	bytesRemaining = BYTEBUFFER_LEN(buffer);
	bytesRemaining = buffer->pos;

	char d[] = {1};
	usbSetInterrupt (d, 1);
	// bufferPrint(buffer);
	// udp_send(eth_udppcb, buffer->p);
}
