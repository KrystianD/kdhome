#include "ethernet.h"
#include <public.h>
#include <hardware.h>
#include <settings.h>
#include <delay.h>
#include <myprintf.h>

#include "lwip/sockets.h"
#include "lwip/init.h"
#include "ipv4/lwip/ip_addr.h"
#include "ipv4/lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/udp.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include <ENC28J60.h>
#include "enc28j60if.h"

#include "providers.h"

extern int dodump;

// consts
const static uint8_t _HW_ADDR[] = { HW_ADDR };
// const static uint8_t _IP_ADDR[] = { IP_ADDR };
// const static uint8_t _IP_MASK[] = { IP_MASK };
// const static uint8_t _IP_GW[] = { IP_GW };
const static uint8_t _IP_SRV[] = { IP_SRV };

// public
uint16_t ethNextPacketPtr;
uint16_t ethSessKey = 0;
uint16_t ethPacketId = 1;

// private
struct udp_pcb *eth_udppcb;
struct netif eth_netif;
uint16_t eth_recvPacketId = 0;

void eth_initRegisters();
void eth_onUDPDataReceived(void* arg, struct udp_pcb* upcb, struct pbuf* p, struct ip_addr* addr, u16_t port);

// public
void enc28j60_enableChip()
{
	IO_LOW(CS);
	_delay_us(1);
}
void enc28j60_disableChip()
{
	_delay_us(1);
	IO_HIGH(CS);
}
uint8_t enc28j60_rw(uint8_t val)
{
	SPI2->DR = val;
	while (!(SPI2->SR & SPI_SR_TXE));
	while (!(SPI2->SR & SPI_SR_RXNE));
	return SPI2->DR;
}

void ethInit()
{
	// configure SPI
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	IO_PUSH_PULL(CS);
	IO_ALT_PUSH_PULL(SCK);
	IO_ALT_PUSH_PULL(MOSI);
	IO_INPUT(MISO);
	SPI2->CR1 = //SPI_CR1_BR_0 | //SPI_CR1_BR_1 | SPI_CR1_BR_0 | 
							SPI_CR1_MSTR | SPI_CR1_SPE | SPI_CR1_SSM | SPI_CR1_SSI;

	IO_INPUT_PP(INT_ETH);
	IO_HIGH(INT_ETH);

	// configure module
	_delay_ms(100);
	enc28j60Reset();
	_delay_ms(500);

	eth_initRegisters();
	
	struct ip_addr ip_addr;
	struct ip_addr net_mask; 
	struct ip_addr gw_addr;

	// IP4_ADDR(&ip_addr, _IP_ADDR[0], _IP_ADDR[1], _IP_ADDR[2], _IP_ADDR[3]);
	// IP4_ADDR(&net_mask, _IP_MASK[0], _IP_MASK[1], _IP_MASK[2], _IP_MASK[3]);
	// IP4_ADDR(&gw_addr, _IP_GW[0], _IP_GW[1], _IP_GW[2], _IP_GW[3]);

	myprintf("lwip_init\r\n");
	lwip_init();
	myprintf("lwip_init done\r\n");

	eth_netif.hwaddr[0] = _HW_ADDR[0];
	eth_netif.hwaddr[1] = _HW_ADDR[1];
	eth_netif.hwaddr[2] = _HW_ADDR[2];
	eth_netif.hwaddr[3] = _HW_ADDR[3];
	eth_netif.hwaddr[4] = _HW_ADDR[4];
	eth_netif.hwaddr[5] = _HW_ADDR[5];

	// netif_add(&eth_netif, &ip_addr, &net_mask, &gw_addr, 0, enc28j60_if_init, ip_input);
	netif_add(&eth_netif, 0, 0, 0, 0, enc28j60_if_init, ip_input);
	myprintf("netif_add done\r\n");

	netif_set_default(&eth_netif);
	myprintf("netif_set_up done\r\n");

	eth_udppcb = udp_new();
	struct ip_addr addr;
	udp_bind(eth_udppcb, IP_ADDR_ANY, LOCAL_PORT);
	IP4_ADDR(&addr, _IP_SRV[0], _IP_SRV[1], _IP_SRV[2], _IP_SRV[3]);
	udp_connect(eth_udppcb, &addr, SRV_PORT);
	udp_recv(eth_udppcb, &eth_onUDPDataReceived, 0);
}
void eth_initRegisters()
{
	uint8_t b;
	uint16_t bb;
	do {
		b = enc28j60ReadControl(ESTAT);
	} while(b & ~ESTAT_CLKRDY);

	b = enc28j60ReadControl(EREVID);
	myprintf("revid: 0x%02x\r\n", b);

	b = 0; // disable CLKOUT
	enc28j60WriteControlCheck(ECOCON, b);

	ethNextPacketPtr = RXStart;
	enc28j60WriteControlWordCheck(ERXSTL, RXStart);
	enc28j60WriteControlWordCheck(ERXRDPTL, RXStart);
	enc28j60WriteControlWordCheck(ERXNDL, RXEnd);
	enc28j60WriteControlWordCheck(ETXSTL, TXStart);
	
	/*PrintRegister16("rx st  ptr", ERXSTL);
	PrintRegister16("rx end ptr", ERXNDL);
	PrintRegister16("rx rd  ptr", ERXRDPTL);
	PrintRegister16("rx wr  ptr", ERXWRPTL);*/
	
	// b = enc28j60ReadControl(MACON2);
	// b &= ~MACON2_MARST;
	// enc28j60WriteControl(MACON2, b);
	
	b = 0;
	b |= ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN;
	enc28j60WriteControlCheck(ERXFCON, b);

	b = enc28j60ReadControl(MACON2);
	b &= ~MACON2_MARST;
	enc28j60WriteControlCheck(MACON2, b);
	
	b = 0;
	b |= MACON3_PACDFG0;
	b |= MACON3_TXCRCEN;
	b |= MACON3_FRMLNEN;
	enc28j60WriteControlCheck(MACON3, b);
	
	enc28j60WriteControlWordCheck(MAMXFLL, MaxFrameLenRX);
	
	enc28j60WriteControlCheck(MABBIPG, 0x12);
	enc28j60WriteControlCheck(MAIPGL, 0x12);
	enc28j60WriteControlCheck(MAIPGH, 0x0c);
	
	enc28j60WriteControlCheck(MAADR0, _HW_ADDR[5]);
	enc28j60WriteControlCheck(MAADR1, _HW_ADDR[4]);
	enc28j60WriteControlCheck(MAADR2, _HW_ADDR[3]);
	enc28j60WriteControlCheck(MAADR3, _HW_ADDR[2]);
	enc28j60WriteControlCheck(MAADR4, _HW_ADDR[1]);
	enc28j60WriteControlCheck(MAADR5, _HW_ADDR[0]);

	bb = 0;
	enc28j60WritePhyWordCheck(PHCON1, bb);

	bb = 0;
	bb |= PHCON2_HDLDIS;
	enc28j60WritePhyWordCheck(PHCON2, bb);

	// LINKIF
	bb = 0;
	bb |= PHIE_PGEIE;
	bb |= PHIE_PLNKIE;
	enc28j60WritePhyWordCheck(PHIE, bb);

	bb = enc28j60ReadPhyWord(PHLCON);
	bb &= ~(PHLCON_LBCFG3 | PHLCON_LBCFG2 | PHLCON_LBCFG1 |PHLCON_LBCFG0);
	bb |= PHLCON_LBCFG2 | PHLCON_LBCFG1 |PHLCON_LBCFG0;
	myprintf("%x\r\n", bb);
	enc28j60WritePhyWordCheck(PHLCON, bb);

	// Enable Half Duplex
	enc28j60SetFullDuplex(0);

	b = enc28j60ReadControl(EFLOCON);
	myprintf("duplex: %d\r\n", !!(b & 0x04));

	// Enable interrupts
	enc28j60SetBits(EIE, EIE_INTIE | EIE_PKTIE | EIE_LINKIE);

	b = 0;
	b |= MACON1_MARXEN;
	enc28j60WriteControlCheck(MACON1, b);
	enc28j60SetBits(ECON1, ECON1_RXEN);

}
void ethProcess()
{
	static uint32_t lastCheck = 0;

	if (ticks - lastCheck >= 1000) {
		lastCheck = ticks;

		// TProvHeader header;
		// provPrepareHeader(&header);
		
		// header.type = PROVIDER_TYPE_CONTROL;
		// header.cmd = 0;
		// provSendPacket(&header, sizeof(header));

		if (dodump)
			enc28j60Dump();
	}

	static uint32_t lastARPTime = 0;
	if (ticks - lastARPTime >= ARP_TMR_INTERVAL)
	{
		lastARPTime = ticks;
		etharp_tmr();
	}

	static uint32_t lastDHCPTime1 = 0;
	if (ticks - lastDHCPTime1 >= DHCP_COARSE_TIMER_MSECS)
	{
		lastDHCPTime1 = ticks;
		dhcp_coarse_tmr();
	}

	static uint32_t lastDHCPTime2 = 0;
	if (ticks - lastDHCPTime2 >= DHCP_FINE_TIMER_MSECS)
	{
		lastDHCPTime2 = ticks;
		dhcp_fine_tmr();
	}

	if (IO_IS_LOW(INT_ETH))
	{
		uint8_t eir = enc28j60ReadControl(EIR);
		myprintf("eir 0x%02x\r\n", eir);
		if (eir & EIR_LINKIF)
		{
			uint16_t phir = enc28j60ReadPhyWord(PHIR);
			if (phir & PHIR_PLNKIF)
			{
				uint16_t stat = enc28j60ReadPhyWord(PHSTAT2);
				if (stat & PHSTAT2_LSTAT) // link is up
				{
					myprintf("LINK UP\r\n");
					netif_set_up(&eth_netif);
					dhcp_start(&eth_netif);
				}
				else // link is down or was for a period
				{
					myprintf("LINK DOWN\r\n");
					dhcp_stop(&eth_netif);
					netif_set_down(&eth_netif);
				}
			}
			myprintf("phir: 0x%04x\r\n", phir);
		}
		enc28j60_if_input(&eth_netif);
	}
}
void provSendPacket(const void* buffer, int len)
{
	struct pbuf* p;
	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	pbuf_take(p, buffer, len);
	udp_send(eth_udppcb, p);
	pbuf_free(p);
}

// private impl
unsigned long sys_now(void)
{
	return ticks;
}
void eth_onUDPDataReceived(void* arg, struct udp_pcb* upcb, struct pbuf* p, struct ip_addr* addr, u16_t port)
{
	char *srcAddr = ipaddr_ntoa(addr);
	myprintf("src addr: %s %d\r\n", srcAddr, p->tot_len);

	uint16_t packetId;
	uint8_t d[p->tot_len];

	pbuf_copy_partial(p, d, p->tot_len, 0);

	provProcess(d, p->tot_len);

	pbuf_free(p);
}
