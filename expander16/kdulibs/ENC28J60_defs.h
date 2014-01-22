#ifndef __ENC28J60_DEFS_H__
#define __ENC28J60_DEFS_H__

#define CMD_READ_CONTROL_REG		(0x00 << 5)
#define CMD_READ_BUFFER_MEMORY	0x3a
#define CMD_WRITE_CONTROL_REG		(0x02 << 5)
#define CMD_WRITE_BUFFER_MEMORY	0x7a
#define CMD_BIT_FIELD_SET				(0x04 << 5)
#define CMD_BIT_FIELD_CLEAR			(0x05 << 5)
#define CMD_SYSTEM_COMMAND			0xff //((0x07 << 5) | 0x1f)

#define ECON1_BANK_MASK		0x03
#define BANK0							0x00
#define BANK1							0x01
#define BANK2							0x02
#define BANK3							0x03

#define BANK0_REG		(BANK0 << 5)
#define BANK1_REG		(BANK1 << 5)
#define BANK2_REG		(BANK2 << 5)
#define BANK3_REG		(BANK3 << 5)
#define DUMMY_REG		0x80

// BANK0
#define ERDPTL		(0x00 | BANK0_REG) // Read Pointer Low Byte
#define ERDPTH		(0x01 | BANK0_REG) // Read Pointer High Byte
#define EWRPTL		(0x02 | BANK0_REG) // Write Pointer Low Byte
#define EWRPTH		(0x03 | BANK0_REG) // Write Pointer High Byte
#define ETXSTL		(0x04 | BANK0_REG) // TX Start Low Byte
#define ETXSTH		(0x05 | BANK0_REG) // TX Start High Byte
#define ETXNDL		(0x06 | BANK0_REG) // TX End Low Byte
#define ETXNDH		(0x07 | BANK0_REG) // TX End High Byte
#define ERXSTL		(0x08 | BANK0_REG) // RX Start Low Byte
#define ERXSTH		(0x09 | BANK0_REG) // RX Start High Byte
#define ERXNDL		(0x0a | BANK0_REG) // RX End Low Byte
#define ERXNDH		(0x0b | BANK0_REG) // RX End High Byte
#define ERXRDPTL	(0x0c | BANK0_REG) // RX RD Pointer Low Byte
#define ERXRDPTH	(0x0d | BANK0_REG) // RX RD Pointer High Byte
#define ERXWRPTL	(0x0e | BANK0_REG) // RX WR Pointer Low Byte
#define ERXWRPTH	(0x0f | BANK0_REG) // RX WR Pointer High Byte
#define EDMASTL		(0x10 | BANK0_REG) // DMA Start Low Byte
#define EDMASTH		(0x11 | BANK0_REG) // DMA Start High Byte
#define EDMANDL		(0x12 | BANK0_REG) // DMA End Low Byte
#define EDMANDH		(0x13 | BANK0_REG) // DMA End High Byte
#define EDMADSTL	(0x14 | BANK0_REG) // DMA Destination Low Byte
#define EDMADSTH	(0x15 | BANK0_REG) // DMA Destination High Byte
#define EDMACSL		(0x16 | BANK0_REG) // DMA Checksum Low Byte
#define EDMACSH		(0x17 | BANK0_REG) // DMA Checksum High Byte

// BANK1
#define EHT0			(0x00 | BANK1_REG) // Hash Table Byte 0
#define EHT1			(0x01 | BANK1_REG) // Hash Table Byte 1
#define EHT2			(0x02 | BANK1_REG) // Hash Table Byte 2
#define EHT3			(0x03 | BANK1_REG) // Hash Table Byte 3
#define EHT4			(0x04 | BANK1_REG) // Hash Table Byte 4
#define EHT5			(0x05 | BANK1_REG) // Hash Table Byte 5
#define EHT6			(0x06 | BANK1_REG) // Hash Table Byte 6
#define EHT7			(0x07 | BANK1_REG) // Hash Table Byte 7
#define EPMM0			(0x08 | BANK1_REG) // Pattern Match Mask Byte 0
#define EPMM1			(0x09 | BANK1_REG) // Pattern Match Mask Byte 1
#define EPMM2			(0x0a | BANK1_REG) // Pattern Match Mask Byte 2
#define EPMM3			(0x0b | BANK1_REG) // Pattern Match Mask Byte 3
#define EPMM4			(0x0c | BANK1_REG) // Pattern Match Mask Byte 4
#define EPMM5			(0x0d | BANK1_REG) // Pattern Match Mask Byte 5
#define EPMM6			(0x0e | BANK1_REG) // Pattern Match Mask Byte 6
#define EPMM7			(0x0f | BANK1_REG) // Pattern Match Mask Byte 7
#define EPMCSL		(0x10 | BANK1_REG) // Pattern Match Checksum Low Byte
#define EPMCSH		(0x11 | BANK1_REG) // Pattern Match Checksum High Byte
#define EPMOL			(0x14 | BANK1_REG) // Pattern Match Offset Low Byte
#define EPMOH			(0x15 | BANK1_REG) // Pattern Match Offset High Byte
#define EWOLIE		(0x16 | BANK1_REG)
#define EWOLIR		(0x17 | BANK1_REG)
#define ERXFCON		(0x18 | BANK1_REG)
#define EPKTCNT		(0x19 | BANK1_REG) // Ethernet Packet Count

// BANK2
#define MACON1		(0x00 | BANK2_REG | DUMMY_REG)
#define MACON2		(0x01 | BANK2_REG | DUMMY_REG)
#define MACON3		(0x02 | BANK2_REG | DUMMY_REG)
#define MACON4		(0x03 | BANK2_REG | DUMMY_REG)
#define MABBIPG		(0x04 | BANK2_REG | DUMMY_REG) // Back-to-Back Inter-Packet Gap
#define MAIPGL		(0x06 | BANK2_REG | DUMMY_REG) // Non-Back-to-Back Inter-Packet Gap Low Byte
#define MAIPGH		(0x07 | BANK2_REG | DUMMY_REG) // Non-Back-to-Back Inter-Packet Gap High Byte
#define MACLCON1	(0x08 | BANK2_REG | DUMMY_REG) // Retransmission Maximum
#define MACLCON2	(0x09 | BANK2_REG | DUMMY_REG) // Collision Window
#define MAMXFLL		(0x0a | BANK2_REG | DUMMY_REG) // Maximum Frame Length Low Byte
#define MAMXFLH		(0x0b | BANK2_REG | DUMMY_REG) // Maximum Frame Length High Byte
#define MAPHSUP		(0x0d | BANK2_REG | DUMMY_REG)
#define MICON			(0x11 | BANK2_REG | DUMMY_REG)
#define MICMD			(0x12 | BANK2_REG | DUMMY_REG)
#define MIREGADR	(0x14 | BANK2_REG | DUMMY_REG) // MII Register Address
#define MIWRL			(0x16 | BANK2_REG | DUMMY_REG) // MII Write Data Low Byte
#define MIWRH			(0x17 | BANK2_REG | DUMMY_REG) // MII Write Data High Byte
#define MIRDL			(0x18 | BANK2_REG | DUMMY_REG) // MII Read Data Low Byte
#define MIRDH			(0x19 | BANK2_REG | DUMMY_REG) // MII Read Data High Byte

// BANK3
#define MAADR1		(0x00 | BANK3_REG | DUMMY_REG) // MAC Address Byte 1
#define MAADR0		(0x01 | BANK3_REG | DUMMY_REG) // MAC Address Byte 0
#define MAADR3		(0x02 | BANK3_REG | DUMMY_REG) // MAC Address Byte 3
#define MAADR2		(0x03 | BANK3_REG | DUMMY_REG) // MAC Address Byte 2
#define MAADR5		(0x04 | BANK3_REG | DUMMY_REG) // MAC Address Byte 5
#define MAADR4		(0x05 | BANK3_REG | DUMMY_REG) // MAC Address Byte 4
#define EBSTSD		(0x06 | BANK3_REG) // Built-in Self-Test Fill Seed
#define EBSTCON		(0x07 | BANK3_REG)
#define EBSTCSL		(0x08 | BANK3_REG) // Built-in Self-Test Checksum Low Byte
#define EBSTCSH		(0x09 | BANK3_REG) // Built-in Self-Test Checksum High Byte
#define MISTAT		(0x0a | BANK3_REG | DUMMY_REG)

#define EREVID		(0x12 | BANK3_REG) // Ethernet Revision ID
#define ECOCON		(0x15 | BANK3_REG)
#define EFLOCON		(0x17 | BANK3_REG)
#define EPAUSL		(0x18 | BANK3_REG) // Pause Timer Value Low Byte
#define EPAUSH		(0x19 | BANK3_REG) // Pause Timer Value High Byte

// Bez banku
#define EIE			0x1b
#define EIR			0x1c
#define ESTAT		0x1d
#define ECON2		0x1e
#define ECON1		0x1f

// PHY
#define PHCON1	0x00
#define PHSTAT1	0x01
#define PHID1		0x02 // PHY Identifier
#define PHID2 	0x03 // PHY Identifier, PHY P/N, PHY Revision
#define PHCON2	0x10
#define PHSTAT2 0x11
#define PHIE		0x12
#define PHIR		0x13
#define PHLCON	0x14

// Dostęp do bitów rejestrów
#define ESTAT_INT       0x80
#define ESTAT_BUFER     0x40
#define ESTAT_LATECOL   0x10
#define ESTAT_RXBUSY    0x04
#define ESTAT_TXABRT    0x02
#define ESTAT_CLKRDY    0x01

#define ECON1_TXRST			(1l << 7)
#define ECON1_RXRST			(1l << 6)
#define ECON1_DMAST			(1l << 5)
#define ECON1_CSUMEN		(1l << 4)
#define ECON1_TXRTS			(1l << 3)
#define ECON1_RXEN			(1l << 2)
#define ECON1_BSEL1			(1l << 1)
#define ECON1_BSEL0			(1l << 0)

#define ECON2_AUTOINC		(1l << 7)
#define ECON2_PKTDEC		(1l << 6)
#define ECON2_PWRSV			(1l << 5)
#define ECON2_VRPS			(1l << 3)

#define ECOCON_COCON0   (1l << 0)
#define ECOCON_COCON1   (1l << 1)
#define ECOCON_COCON2   (1l << 2)

#define EIR_PKTIF				(1l << 6)
#define EIR_DMAIF				(1l << 5)
#define EIR_LINKIF			(1l << 4)
#define EIR_TXIF				(1l << 3)
#define EIR_WOLIF				(1l << 2)
#define EIR_TXERIF			(1l << 1)
#define EIR_RXERIF			(1l << 0)

#define ERXFCON_UCEN    0x80
#define ERXFCON_ANDOR   0x40
#define ERXFCON_CRCEN   0x20
#define ERXFCON_PMEN    0x10
#define ERXFCON_MPEN    0x08
#define ERXFCON_HTEN    0x04
#define ERXFCON_MCEN    0x02
#define ERXFCON_BCEN    0x01

#define MACON1_LOOPBK		(1l << 4)
#define MACON1_TXPAUS		(1l << 3)
#define MACON1_RXPAUS		(1l << 2)
#define MACON1_PASSALL	(1l << 1)
#define MACON1_MARXEN		(1l << 0)

#define MACON2_MARST		(1l << 7)
#define MACON2_RNDRST		(1l << 6)
#define MACON2_MARXRST	(1l << 3)
#define MACON2_RFUNRST	(1l << 2)
#define MACON2_MATXRST	(1l << 1)
#define MACON2_TFUNRST	(1l << 0)

#define MACON3_PADCFG2	(1l << 7)
#define MACON3_PADCFG1	(1l << 6)
#define MACON3_PACDFG0	(1l << 5)
#define MACON3_TXCRCEN	(1l << 4)
#define MACON3_PHDRLEN	(1l << 3)
#define MACON3_HFRMEN		(1l << 2)
#define MACON3_FRMLNEN	(1l << 1)
#define MACON3_FULDPX		(1l << 0)

#define MICMD_MIISCAN		(1l << 1)
#define MICMD_MIIRD			(1l << 0)

#define MISTAT_NVALID		(1l << 2)
#define MISTAT_SCAN			(1l << 1)
#define MISTAT_BUSY			(1l << 0)

#define	PHCON1_PRST		  (1l << 15)
#define	PHCON1_PLOOPBK	(1l << 14)
#define	PHCON1_PPWRSV	  (1l << 11)
#define	PHCON1_PDPXMD	  (1l << 8)

#define PHCON2_FRCLNK		(1l << 14)
#define PHCON2_TXDIS		(1l << 13)
#define PHCON2_JABBER		(1l << 10)
#define PHCON2_HDLDIS		(1l << 8)

#define PHSTAT1_PFDPX	  (1l << 12)
#define PHSTAT1_PHDPX		(1l << 11)
#define PHSTAT1_LLSTAT	(1l << 2)
#define PHSTAT1_JBSTAT	(1l << 1)

#define PHSTAT2_TXSTAT	(1l << 13)
#define PHSTAT2_RXSTAT	(1l << 12)
#define PHSTAT2_COLSTAT	(1l << 11)
#define PHSTAT2_LSTAT		(1l << 10)
#define PHSTAT2_DPXSTAT	(1l << 9)
#define PHSTAT2_PLRITY	(1l << 4)

#define PHIE_PLNKIE			(1l << 4)
#define PHIE_PGEIE			(1l << 1)

#define PHLCON_LACFG3		(1l << 11)
#define PHLCON_LACFG2		(1l << 10)
#define PHLCON_LACFG1		(1l << 9)
#define PHLCON_LACFG0		(1l << 8)
#define PHLCON_LBCFG3		(1l << 7)
#define PHLCON_LBCFG2		(1l << 6)
#define PHLCON_LBCFG1		(1l << 5)
#define PHLCON_LBCFG0		(1l << 4)
#define PHLCON_LFRQ1		(1l << 3)
#define PHLCON_LFRQ0		(1l << 2)
#define PHLCON_STRCH		(1l << 1)

#define PHIR_PLNKIF     (1l << 4)
#define PHIR_PGIF       (1l << 2)

#define	EIE_INTIE		    (1l << 7)
#define	EIE_PKTIE		    (1l << 6)
#define	EIE_DMAIE		    (1l << 5)
#define	EIE_LINKIE		  (1l << 4)
#define	EIE_TXIE		    (1l << 3)
#define	EIE_TXERIE		  (1l << 1)
#define	EIE_RXERIE		  (1l << 0)

typedef union
{
	uint8_t d[7];
	struct
	{
		uint16_t ByteCount;
		uint8_t  CollisionCount:4;
		uint8_t  CRCError:1;
		uint8_t  LengthCheckError:1;
		uint8_t  LengthOutOfRange:1;
		uint8_t  Done:1;
		uint8_t  Multicast:1;
		uint8_t  Broadcast:1;
		uint8_t  PacketDefer:1;
		uint8_t  ExcessiveDefer:1;
		uint8_t  MaximumCollisions:1;
		uint8_t  LateCollision:1;
		uint8_t  Giant:1;
		uint8_t  Underrun:1;
		uint16_t BytesTransmittedOnWire;
		uint8_t  ControlFrame:1;
		uint8_t  PAUSEControlFrame:1;
		uint8_t  BackpressureApplied:1;
		uint8_t  VLANTaggedFrame:1;
		uint8_t  Zeros:4;
	} bits;
} ENC28J60_TXSTATUS;
typedef union
{
	uint8_t d[6];
	struct
	{
		uint16_t NextPacket;
		uint16_t ByteCount;
		uint8_t  LongEvent:1;
		uint8_t  Reserved:1;
		uint8_t  CarrierEvent:1;
		uint8_t  Reserved2:1;
		uint8_t  CRCError:1;
		uint8_t  LenChkError:1;
		uint8_t  LenOutofRange:1;
		uint8_t  RxOk:1;
		uint8_t  RxMultiCast:1;
		uint8_t  RxBroadCast:1;
		uint8_t  DribbleNibble:1;
		uint8_t  RxCntrlFrame:1;
		uint8_t  RxPauseFrame:1;
		uint8_t  RxUkwnOpcode:1;
		uint8_t  RxVlan:1;
		uint8_t  Zeros:1;
	} bits;
} ENC28J60_RXSTATUS;

#endif
