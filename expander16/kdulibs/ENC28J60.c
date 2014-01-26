#include "ENC28J60.h"
#include <myprintf.h>
#include <delay.h>

#define HIGH(x)(((x) >> 8) & 0xff)
#define LOW(x)(((x) >> 0) & 0xff)

uint8_t enc28j60_bank = 0xff;

#include <color_codes.h>
#define ERROR COLOR_RED " --------------- ERROR%d --------------- " COLOR_OFF

void enc28j60SelectBank(uint8_t bank)
{
	uint8_t b;

	if (enc28j60_bank != bank)
	{
		// myprintf("sel bank %d\r\n", bank);
		enc28j60_enableChip();
		enc28j60_rw(CMD_READ_CONTROL_REG | ECON1);
		b = enc28j60_rw(0xff);
		enc28j60_disableChip();

		b = (b & ~ECON1_BANK_MASK) | bank;

		enc28j60_enableChip();
		enc28j60_rw(CMD_WRITE_CONTROL_REG | ECON1);
		enc28j60_rw(b);
		enc28j60_disableChip();

		enc28j60_bank = bank;
	}
}
uint8_t enc28j60SelectBankByAddr(uint8_t addr)
{
	enc28j60SelectBank((addr >> 5) & 0x03);
	return addr & 0x1f;
}
uint8_t enc28j60ReadControl(uint8_t addr)
{
	uint8_t _addr = enc28j60SelectBankByAddr(addr);
	uint8_t v;
	
	enc28j60_enableChip();
	enc28j60_rw(CMD_READ_CONTROL_REG | _addr);
	if (addr & DUMMY_REG)
		enc28j60_rw(0xff);
	v = enc28j60_rw(0xff);
	enc28j60_disableChip();
	
	return v;
}
uint16_t enc28j60ReadControlWord(uint8_t addr)
{
	return 
		((uint16_t)enc28j60ReadControl(addr)) |
		((uint16_t)enc28j60ReadControl(addr + 1) << 8);
}
void enc28j60WriteControl(uint8_t addr, uint8_t val)
{
	uint8_t _addr = enc28j60SelectBankByAddr(addr);
	
	enc28j60_enableChip();
	enc28j60_rw(CMD_WRITE_CONTROL_REG | _addr);
	enc28j60_rw(val);
	enc28j60_disableChip();
}
void enc28j60WriteControlCheck(uint8_t addr, uint8_t val)
{
	int tries = 1;
try:
	enc28j60WriteControl(addr, val);
	uint8_t newval = enc28j60ReadControl(addr);

	if (val != newval)
	{
		myprintf(ERROR " CTRL\r\n", tries);
		tries++;
		if (tries == 3)
		{
			myprintf("unable\r\n");
			for (;;);
		}
		_delay_ms(1);
		goto try;
	}
}
void enc28j60WriteControlWord(uint8_t addr, uint16_t val)
{
	enc28j60WriteControl(addr, LOW(val));
	enc28j60WriteControl(addr + 1, HIGH(val));
}
void enc28j60WriteControlWordCheck(uint8_t addr, uint16_t val)
{
	int tries = 1;
try:
	enc28j60WriteControlWord(addr, val);
	uint16_t newval = enc28j60ReadControlWord(addr);

	if (val != newval)
	{
		myprintf(ERROR " CTRL WORD %d %d\r\n", tries, val, newval);
		tries++;
		if (tries == 3)
		{
			myprintf("unable\r\n");
			for (;;);
		}
		_delay_ms(1);
		goto try;
	}
}
uint16_t enc28j60ReadPhyWord(uint8_t addr)
{
	uint16_t val;
	uint8_t b;

	enc28j60WriteControl(MIREGADR, addr);

	b = MICMD_MIIRD;
	enc28j60WriteControl(MICMD, b);

	do
	{
		_delay_us(2);
		b = enc28j60ReadControl(MISTAT);
	} while (b & MISTAT_BUSY);
	
	enc28j60WriteControl(MICMD, 0);
	
	val = enc28j60ReadControl(MIRDL);
	val |=(uint16_t)enc28j60ReadControl(MIRDH) << 8;
	return val;
}
void enc28j60WritePhyWord(uint8_t addr, uint16_t val)
{
	uint8_t b;
	
	enc28j60WriteControl(MIREGADR, addr);
	enc28j60WriteControl(MIWRL, LOW(val));	
	enc28j60WriteControl(MIWRH, HIGH(val));
		
	b = 0;
	do
	{
		_delay_us(2);
		b = enc28j60ReadControl(MISTAT);
	} while (b & MISTAT_BUSY);
}
void enc28j60WritePhyWordCheck(uint8_t addr, uint16_t val)
{
	int tries = 1;
try:
	enc28j60WritePhyWord(addr, val);
	uint16_t newval = enc28j60ReadPhyWord(addr);

	if (val != newval)
	{
		myprintf(ERROR " PHY\r\n", tries);
		tries++;
		if (tries == 3)
		{
			myprintf("unable\r\n");
			for (;;);
		}
		_delay_ms(1);
		goto try;
	}
}
void enc28j60SetBits(uint8_t addr, uint8_t mask)
{
	uint8_t _addr = enc28j60SelectBankByAddr(addr);

	enc28j60_enableChip();
	enc28j60_rw(CMD_BIT_FIELD_SET | _addr);
	enc28j60_rw(mask);
	enc28j60_disableChip();
}
void enc28j60ClearBits(uint8_t addr, uint8_t mask)
{
	uint8_t _addr = enc28j60SelectBankByAddr(addr);
	
	enc28j60_enableChip();
	enc28j60_rw(CMD_BIT_FIELD_CLEAR | _addr);
	enc28j60_rw(mask);
	enc28j60_disableChip();
}
void enc28j60Reset()
{
	enc28j60_enableChip();
	enc28j60_rw(CMD_SYSTEM_COMMAND);
	_delay_ms(10);
	enc28j60_disableChip();
}

void enc28j60ReadBuffer(uint8_t* data, uint16_t len)
{
	uint16_t i;
	enc28j60_enableChip();
	enc28j60_rw(CMD_READ_BUFFER_MEMORY);
	for (i = 0; i < len; i++)
		data[i] = enc28j60_rw(0xff);
	enc28j60_disableChip();
}
void enc28j60WriteBuffer(const uint8_t* data, uint16_t len)
{
	uint16_t i;
	enc28j60_enableChip();
	enc28j60_rw(CMD_WRITE_BUFFER_MEMORY);
	for (i = 0; i < len; i++)
		enc28j60_rw(data[i]);
	enc28j60_disableChip();
}
void enc28j60WriteBufferByte(uint8_t data)
{
	enc28j60_enableChip();
	enc28j60_rw(CMD_WRITE_BUFFER_MEMORY);
	enc28j60_rw(data);
	enc28j60_disableChip();
}
void enc28j60WriteBufferWordBE(uint16_t data)
{
	enc28j60_enableChip();
	enc28j60_rw(CMD_WRITE_BUFFER_MEMORY);
	enc28j60_rw(HIGH(data));
	enc28j60_rw(LOW(data));
	enc28j60_disableChip();
}

void enc28j60WaitForTransmitReady()
{
	uint8_t b = enc28j60ReadControl(ECON1);
	while (b & ECON1_TXRTS)
	{
		_delay_us(10);
		b = enc28j60ReadControl(ECON1);
	}
}

// utils
uint8_t enc28j60GetLinkStatus()
{
	uint16_t bb = enc28j60ReadPhyWord(PHSTAT2);
	return(bb & PHSTAT2_LSTAT) > 0;
}
void enc28j60SendPacket()
{
	// enc28j60SetBits(ECON1, ECON1_TXRTS);
	// enc28j60WaitForTransmitReady();

	// enc28j60ClearBits(ECON1, ECON1_TXRTS);

	// uint8_t b;
	// b = enc28j60ReadControl(ESTAT);

	// if (b & ESTAT_TXABRT)
	// {
		// myprintf("
	// }
}
void enc28j60SetFullDuplex(uint8_t enabled)
{
	uint8_t b;

	if (enabled)
	{
		b = enc28j60ReadControl(MACON3);
		b |= MACON3_FULDPX;
		enc28j60WriteControl(MACON3, b);

		b = enc28j60ReadPhyWord(PHCON1);
		b |= PHCON1_PDPXMD;
		enc28j60WritePhyWord(PHCON1, b);
	}
	else
	{
		b = enc28j60ReadControl(MACON3);
		b &= ~MACON3_FULDPX;
		enc28j60WriteControl(MACON3, b);

		b = enc28j60ReadPhyWord(PHCON1);
		b &= ~PHCON1_PDPXMD;
		enc28j60WritePhyWord(PHCON1, b);
	}
}

#ifdef ENC28J60_DEBUG
#include <string.h>
#include "color_codes.h"

static const char* dec2bin(uint8_t num)
{
	static char txt[9] = { 0 };
	if (num & (1ul << 0)) txt[7] = '1';
	if (num & (1ul << 1)) txt[6] = '1';
	if (num & (1ul << 2)) txt[5] = '1';
	if (num & (1ul << 3)) txt[5] = '1';
	if (num & (1ul << 4)) txt[3] = '1';
	if (num & (1ul << 5)) txt[2] = '1';
	if (num & (1ul << 6)) txt[1] = '1';
	if (num & (1ul << 7)) txt[0] = '1';
	return txt;
}
static const char* dec2bin16(uint16_t num)
{
	static char txt[16 + 1] = { 0 };
	if (num & (1ul << 0)) txt[15] = '1';
	if (num & (1ul << 1)) txt[14] = '1';
	if (num & (1ul << 2)) txt[13] = '1';
	if (num & (1ul << 3)) txt[12] = '1';
	if (num & (1ul << 4)) txt[11] = '1';
	if (num & (1ul << 5)) txt[10] = '1';
	if (num & (1ul << 6)) txt[9] = '1';
	if (num & (1ul << 7)) txt[8] = '1';
	if (num & (1ul << 8)) txt[7] = '1';
	if (num & (1ul << 9)) txt[6] = '1';
	if (num & (1ul << 10)) txt[5] = '1';
	if (num & (1ul << 11)) txt[5] = '1';
	if (num & (1ul << 12)) txt[3] = '1';
	if (num & (1ul << 13)) txt[2] = '1';
	if (num & (1ul << 14)) txt[1] = '1';
	if (num & (1ul << 15)) txt[0] = '1';
	return txt;
}
void enc28j60_printRegister8(const char* txt, uint8_t addr)
{
	uint8_t b = enc28j60ReadControl(addr);
	myprintf("%-15s: hex: 0x%02x     dec: %6d   bin: %s\r\n", txt, b, b, dec2bin(b));
}
void enc28j60_printRegister16(const char* txt, uint8_t addr)
{
	uint8_t b;
	uint16_t val = enc28j60ReadControlWord(addr);
	myprintf("%-15s: hex: 0x%04x   dec: %6d   bin: %s\r\n", txt, val, val, dec2bin16(val));
}
void enc28j60PrintMemory()
{
#define INSIDE(x)(((x) >= (begin)) && ((x) <= (end)))
	uint16_t i;
	uint16_t readPointer = enc28j60ReadControlWord(ERDPTL);
	uint16_t rxStart = enc28j60ReadControlWord(ERXSTL);
	uint16_t rxEnd = enc28j60ReadControlWord(ERXNDL);
	uint16_t txStart = enc28j60ReadControlWord(ETXSTL);
	uint16_t txEnd = enc28j60ReadControlWord(ETXNDL);
	
	uint16_t rxWritePointer = enc28j60ReadControlWord(ERXWRPTL);
	uint16_t rxReadPointer = enc28j60ReadControlWord(ERXRDPTL);
	
	enc28j60_printRegister16("rd pt", ERDPTL);
	enc28j60_printRegister16("rx st", ERXSTL);
	enc28j60_printRegister16("rx end", ERXNDL);
	enc28j60_printRegister16("rx rd  ptr", ERXRDPTL);
	enc28j60_printRegister16("rx wr  ptr", ERXWRPTL);
	
	myprintf("RXST(r),RXND(R): ");
	for (i = 0; i < 0x1ff0; i += 128)
	{
		uint16_t begin = i;
		uint16_t end = i + 127;
		char c = '-';
		if (INSIDE(rxStart)) c = 'r';
		if (INSIDE(rxEnd)) c = 'R';
		myputchar(c);
	}
	myprintf("\r\n");
	myprintf("TXSTt          : ");
	for (i = 0; i < 0x1ff0; i += 128)
	{
		uint16_t begin = i;
		uint16_t end = i + 127;
		char c = '-';
		if (INSIDE(txStart)) c = 't';
		myputchar(c);
	}
	myprintf("\r\n");
	myprintf("RXRDPT$,RXWRPT%%: ");
	for (i = 0; i < 0x1ff0; i += 128)
	{
		uint16_t begin = i;
		uint16_t end = i + 127;
		char c = '-';
		if (INSIDE(rxWritePointer)) c = '%';
		if (INSIDE(rxReadPointer)) c = '$';
		myputchar(c);
	}
	myprintf("\r\n");

	myprintf("RDPT|          : ");
	for (i = 0; i < 0x1ff0; i += 128)
	{
		uint16_t begin = i;
		uint16_t end = i + 127;
		char c = '-';
		if (INSIDE(readPointer)) c = '|';
		myputchar(c);
	}
	myprintf("\r\n");
}
void enc28j60PrintRXStatus(ENC28J60_RXSTATUS* status)
{
	myprintf("byte count: %d, ", status->bits.ByteCount);
	if (status->bits.Zeros) myprintf("Zero,(31)");
	if (status->bits.RxVlan) myprintf("Receive VLAN Type Detected, ");
	if (status->bits.RxUkwnOpcode) myprintf("Receive Unknown Opcode, ");
	if (status->bits.RxPauseFrame) myprintf("Receive Pause Control Frame, ");
	if (status->bits.RxCntrlFrame) myprintf("Receive Control Frame, ");
	if (status->bits.DribbleNibble) myprintf("Dribble Nibble, ");
	if (status->bits.RxBroadCast) myprintf("Receive Broadcast Packet, ");
	if (status->bits.RxMultiCast) myprintf("Receive Multicast Packet, ");
	if (status->bits.RxOk) myprintf("Received Ok, ");
	if (status->bits.LenOutofRange) myprintf("Length Out of Range, ");
	if (status->bits.LenChkError) myprintf("Length Check Error, ");
	if (status->bits.CRCError) myprintf("CRC Error, ");
	if (status->bits.Reserved2) myprintf("Reserved(19), ");
	if (status->bits.CarrierEvent) myprintf("Carrier Event Previously Seen, ");
	if (status->bits.Reserved) myprintf("Reserved(17), ");
	if (status->bits.LongEvent) myprintf("Long Event/Drop Event, ");
	myprintf("\r\n");
}
void enc28j60PrintTXStatus(ENC28J60_TXSTATUS* status)
{
	// uint32_t high = *((uint32_t*)(data + 4));
	// uint32_t low = *((uint32_t*)(data + 0));
	// uint16_t byteCount = low & 0x0000ffff;
	// uint16_t collisionCount = (low & 0x000f0000) >> 16;
	// uint16_t bytesOnWire = high & 0x0000ffff;
	// uint16_t zero = high & 0x00f00000;
	myprintf("byte count: %d, ", status->bits.ByteCount);
	myprintf("collision count: %d, ", status->bits.CollisionCount);
	myprintf("bytes on wire: %d, ", status->bits.BytesTransmittedOnWire);
	myprintf("zero: 0x%x, ", status->bits.Zeros);

	if (status->bits.VLANTaggedFrame) myprintf("Transmit VLAN Tagged Frame, ");
	if (status->bits.BackpressureApplied) myprintf("Backpressure Applied, ");
	if (status->bits.PAUSEControlFrame) myprintf("Transmit Pause Control Frame, ");
	if (status->bits.ControlFrame) myprintf("Transmit Control Frame, ");

	if (status->bits.Underrun) myprintf("Transmit Underrun, ");
	if (status->bits.Giant) myprintf("Transmit Giant, ");
	if (status->bits.LateCollision) myprintf("Transmit Late Collision, ");
	if (status->bits.MaximumCollisions) myprintf("Transmit Excessive Collision, ");
	if (status->bits.ExcessiveDefer) myprintf("Transmit Excessive Defer, ");
	if (status->bits.PacketDefer) myprintf("Transmit Packet Defer, ");
	if (status->bits.Broadcast) myprintf("Transmit Broadcast, ");
	if (status->bits.Multicast) myprintf("Transmit Multicast, ");
	if (status->bits.Done) myprintf("Transmit Done, ");
	if (status->bits.LengthOutOfRange) myprintf("Transmit Length Out of Range, ");
	if (status->bits.LengthCheckError) myprintf("Transmit Length Check Error, ");
	if (status->bits.CRCError) myprintf("Transmit CRC Error, ");
	myprintf("\r\n");
}
void enc28j60PrintESTAT(uint8_t status)
{
	if (status & ESTAT_INT) myprintf("ESTAT_INT,");
	if (status & ESTAT_BUFER) myprintf("ESTAT_BUFER,");
	if (status & ESTAT_LATECOL) myprintf("ESTAT_LATECOL,");
	if (status & ESTAT_RXBUSY) myprintf("ESTAT_RXBUSY,");
	if (status & ESTAT_TXABRT) myprintf("ESTAT_TXABRT,");
	if (status & ESTAT_CLKRDY) myprintf("ESTAT_CLKRDY,");
	myprintf("\r\n");
}
struct
{
	uint8_t reg;
	const char* const data[8];
} registers8[] = 
{
	{ ESTAT, { "INT", "BUFER", "", "LATECOL", "", "RXBUSY", "TXABRT", "CLKRDY"} },
	{ ECON1, { "TXRST", "RXRST", "DMAST", "CSUMEN", "TXRTS", "RXEN", "BSEL1", "BSEL0" } },
	{ ECON2, { "AUTOINC", "PKTDEC", "PWRSV", "", "VRPS", "", "", "" } },
	{ EIE, { "INTIE", "PKTIE", "DMAIE", "LINKIE", "TXIE", "", "TXERIE", "RXERIE" } },
	{ EIR, { "", "PKTIF", "DMAIF", "LINKIF", "TXIF", "", "RXERIF", "RXERIF" } },
	{ 0, 0 }
};
struct
{
	uint8_t reg;
	const char* const data[16];
} registers16[] = 
{
	{ PHSTAT1, { "", "", "", "PFDPX", "PHDPX", "", "", "",
							 "", "", "", "", "", "LLSTAT", "JVSTAT", "" } },
	{ PHSTAT2, { "", "", "TXSTAT", "RXSTAT", "COLSTAT", "LSTAT", "DPXSTAT", "",
							 "" "", "PLRITY", "", "", "", "", "" } },
	{ 0, 0 }
};
void enc28j60DumpRegister(const char* name, uint8_t reg, char t)
{
	int i, j;
	uint8_t b;
	b = enc28j60ReadControl(reg);
	if (t == 'x')
		myprintf(COLOR_CYAN "%-7s" COLOR_OFF " 0x%02x %s", name, b, dec2bin(b));
	else if (t == 'd')
		myprintf(COLOR_CYAN "%-7s" COLOR_OFF " %d", name, b);
	for (i = 0; registers8[i].reg; i++)
	{
		if (registers8[i].reg == reg)
		{
			int has = 0;
			for (j = 0; j < 8; j++)
			{
				if (b & (1 << (7 - j)))
				{
					if (has) myputchar(',');
					else myputchar(':');
					myputchar(' ');
					myprintf("%s", registers8[i].data[j]);
					has = 1;
				}
			}
		}
	}
	myprintf("\r\n");
}
void enc28j60DumpRegisterPHY16(const char* name, uint8_t reg, char t)
{
	int i, j;
	uint16_t b;
	b = enc28j60ReadPhyWord(reg);
	if (t == 'x')
		myprintf(COLOR_CYAN "%-7s" COLOR_OFF " 0x%04x %s", name, b, dec2bin16(b));
	else if (t == 'd')
		myprintf(COLOR_CYAN "%-7s" COLOR_OFF " %d", name, b);
	for (i = 0; registers16[i].reg; i++)
	{
		if (registers16[i].reg == reg)
		{
			int has = 0;
			for (j = 0; j < 16; j++)
			{
				if (b & (1 << (15 - j)))
				{
					if (has) myputchar(',');
					else myputchar(':');
					myputchar(' ');
					myprintf("%s", registers16[i].data[j]);
					has = 1;
				}
			}
		}
	}
	myprintf("\r\n");
}
#define DUMP(x) enc28j60DumpRegister(#x,x,'x')
#define DUMPPHY(x) enc28j60DumpRegisterPHY16(#x,x,'x')
#define DUMPd(x) enc28j60DumpRegister(#x,x,'d')
void enc28j60Dump()
{
	uint8_t b;

	myprintf(COLOR_RED "----- ENC28J60 DUMP -----\r\n" COLOR_OFF);

	DUMP(ESTAT);
	DUMP(ECON1);
	DUMP(ECON2);
	DUMP(EIE);
	DUMP(EIR);
	DUMPd(EPKTCNT);
	DUMPPHY(PHSTAT1);
	DUMPPHY(PHSTAT2);
	// b = enc28j60ReadControl(ECON1);
	// enc28j60_printRegister16("ECON1", ECON1);

	myprintf(COLOR_RED "----- ENC28J60 DUMP END -----\r\n" COLOR_OFF);
}
#endif
