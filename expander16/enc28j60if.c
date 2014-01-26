/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright(c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"

#include <public.h>
#include <delay.h>
#include <ENC28J60.h>
#include "ethernet.h"
#include <color_codes.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

extern int dodump;
/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/**
 * In this function, the hardware should be initialized.
 * Called from enc28j60_if_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  /*netif->hwaddr[0] = mac_addr[0];
  netif->hwaddr[1] = mac_addr[1];
  netif->hwaddr[2] = mac_addr[2];
  netif->hwaddr[3] = mac_addr[3];
  netif->hwaddr[4] = mac_addr[4];
  netif->hwaddr[5] = mac_addr[5];*/

  /* maximum transfer unit */
  netif->mtu = MaxFrameLen;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
 
  /* Do whatever else is needed to initialize interface. */  
	// enc28j60Init(netif->hwaddr,1);
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send(e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure(except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
	struct ethernetif *ethernetif = netif->state;
  struct pbuf *q;

	// return;
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

	uint8_t b = enc28j60ReadControl(EIR);
	if (b & EIR_TXERIF)
	{
		myprintf("TXERIF\r\n");

		enc28j60SetBits(ECON1, ECON1_TXRST);
		enc28j60ClearBits(ECON1, ECON1_TXRST);
		enc28j60ClearBits(EIR, EIR_TXERIF);

		// for (;;);
	}

retry:
	if (dodump)
		myprintf(COLOR_GREEN "----- SENDING ----- \r\n" COLOR_OFF);

	if (dodump)
		myprintf("@4");
	enc28j60WriteControlWordCheck(EWRPTL, TXStart);
	uint16_t end = TXStart + 1 + p->tot_len;
	if (dodump)
		myprintf("@5");
	enc28j60WriteControlWordCheck(ETXSTL, TXStart);
	if (dodump)
		myprintf("@6");
	enc28j60WriteControlWordCheck(ETXNDL, end);

	enc28j60_enableChip();
	enc28j60_rw(CMD_WRITE_BUFFER_MEMORY);
	enc28j60_rw(0x00); // control byte
	for (q = p; q; q = q->next)
	{
		int i;
		for (i = 0; i < q->len; i++)
			enc28j60_rw(((char*)q->payload)[i]);
		if (q->len == p->tot_len)
			break;
  }
	enc28j60_disableChip();


	// myprintf("a %d\r\n", p->tot_len);
	// enc28j60PrintMemory();

	enc28j60SetBits(ECON1, ECON1_TXRTS);

	static int totalFails = 0;
	myprintf("waiting until packet is sent (fails: %d)\r\n", totalFails);
	b = enc28j60ReadControl(ECON1);
	uint32_t startTime = ticks;
	while (b & ECON1_TXRTS)
	{
		myprintf("TRANS 0x%02x\r\n", b);
		_delay_ms(10);
		if (ticks - startTime > 100)
		{
			totalFails++;
			enc28j60SetBits(ECON1, ECON1_TXRST);
			enc28j60ClearBits(ECON1, ECON1_TXRST);
			goto retry;
		}
		b = enc28j60ReadControl(ECON1);
	}
	myprintf("packet sent\r\n");

	// enc28j60ClearBits(ECON1, ECON1_TXRTS);

	b = enc28j60ReadControl(ESTAT);
	// myprintf("ESTAT: "); enc28j60PrintESTAT(b);
	if (dodump)
		myprintf("@7");
	enc28j60WriteControlWordCheck(ERDPTL, end + 1);
	ENC28J60_TXSTATUS txstatus;
	enc28j60ReadBuffer(txstatus.d,7);

	if (dodump)
		enc28j60PrintTXStatus(&txstatus);

	if (b & ESTAT_TXABRT)
	{
		myprintf("TXABRT\r\n");
		if (txstatus.bits.LateCollision)
		{
			enc28j60SetBits(ECON1, ECON1_TXRST);
			enc28j60ClearBits(ECON1, ECON1_TXRST);
			enc28j60ClearBits(ESTAT, ESTAT_TXABRT | ESTAT_LATECOL);
		}
	}

	enc28j60ClearBits(EIR, EIR_TXERIF | EIR_TXIF);
	enc28j60ClearBits(ESTAT, ESTAT_TXABRT);

	if (dodump)
		myprintf(COLOR_GREEN "----- SENDING END ----- \r\n" COLOR_OFF);
	// uint16_t sentBytes = *((uint16_t*)d + 0);
	// myprintf("send bytes: %d\r\n", sentBytes);

	// int i; for (i=0;i<7;i++) myprintf("0x%02x, ", d[i]); myprintf("\r\n");


	// enc28j60BeginPacketSend(p->tot_len);

  // for (q = p; q != NULL; q = q->next) {
    // /* Send the data from the pbuf to the interface, one pbuf at a
       // time. The size of the data in each pbuf is kept in the ->len
       // variable. */
		// enc28j60PacketSend(q->payload, q->len);
  // }
	// enc28j60EndPacketSend();

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
  
  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet(including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *p, *q;
  u16_t len;

	uint8_t b = enc28j60ReadControl(EPKTCNT);
	// myprintf("c: %d\r\n", b);
	if (b == 0)
		return 0;

	if (dodump)
		myprintf(COLOR_YELLOW "----- RECEIVING ----- \r\n" COLOR_OFF);

	uint16_t curPtr = ethNextPacketPtr;
	// myprintf("@1");
	enc28j60WriteControlWordCheck(ERDPTL, curPtr);

	uint16_t nextPtr, packetSize, status1, status2;

	if (dodump)
		enc28j60PrintMemory();

	ENC28J60_RXSTATUS status;
	enc28j60ReadBuffer(status.d, 6);

	nextPtr = status.bits.NextPacket;
	ethNextPacketPtr = nextPtr;
	packetSize = status.bits.ByteCount - 4;

	if (dodump)
		enc28j60PrintRXStatus(&status);	
	// myprintf("in\r\n");
	// myprintf("STATUS: "); 
	// myprintf("data len: %u cur ptr %u next ptr: %u\r\n", packetSize, curPtr, nextPtr);

	// myprintf("recv %d\r\n", len);

		// PrintMemory();

	/* Obtain the size of the packet and put it into the "len"
     variable. */
  // len = enc28j60BeginPacketReceive();
	// if (!len) return NULL;

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

	// myprintf("aclloc :%d\r\n", len);
  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, packetSize, PBUF_POOL);
  
	// myprintf("c: %d\r\n", pbuf_clen(p));
	// p=0;
  if (p != NULL)
	{

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
		// int len2=packetSize;
    for (q = p; q; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable. */
			enc28j60ReadBuffer(q->payload, q->len);

			// if (q->len == p->tot_len) break;
		}

		//acknowledge that packet has been read();
		// enc28j60EndPacketReceive();

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    //drop packet();
		// enc28j60EndPacketReceive();
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }

	// enc28j60SetBits(ECON1, ECON1_RXEN);

	if (ethNextPacketPtr == RXStart)
	{
		// if (nextPacketPtr - 1 < RXStart || nextPacketPtr - 1 > RXEnd)
		// myprintf("@2");
		enc28j60WriteControlWordCheck(ERXRDPTL, RXEnd);
		myprintf("nextPtr == RXStart!\r\n");
		// for (;;);
	}
	else
	{
		// myprintf("@3");
		enc28j60WriteControlWordCheck(ERXRDPTL, ethNextPacketPtr - 1);
	}

	enc28j60SetBits(ECON2, ECON2_PKTDEC);

	if (dodump)
		myprintf(COLOR_YELLOW "----- RECEIVING END ----- \r\n" COLOR_OFF);
	// b = enc28j60ReadControl(EPKTCNT);

	// pbuf_free(p); return 0;
  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void enc28j60_if_input(struct netif *netif)
{
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  ethernetif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* no packet could be read, silently ignore this */
  if (p == NULL) return;
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = p->payload;

  switch(htons(ethhdr->type)) {
  /* IP packet? */
  case ETHTYPE_IP:
#if 0
/* CSi disabled ARP table update on ingress IP packets.
   This seems to work but needs thorough testing. */
    /* update ARP table */
    etharp_ip_input(netif, p);
#endif
    /* skip Ethernet header */
    pbuf_header(p, -(s16_t)sizeof(struct eth_hdr));
    LWIP_DEBUGF(NETIF_DEBUG,("enc28j60_if_input: passing packet up to IP\n"));
    // pbuf_free(p);
    /* pass to network layer */
    netif->input(p, netif);
    break;
  /* ARP packet? */
  case ETHTYPE_ARP:
    /* pass p to ARP module */
    ethernet_input(p, netif);
    break;
  /* unsupported Ethernet packet type */
  default:
    /* free pbuf */
    pbuf_free(p);
    p = NULL;
    break;
  }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t enc28j60_if_init(struct netif *netif)
{
  struct ethernetif *ethernetif;
	const u8_t *mac_addr;

	myprintf("ini\r\n");
  LWIP_ASSERT("netif != NULL",(netif != NULL));
    
  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 10000000); //10Mbit

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending(e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  ethernetif->ethaddr =(struct eth_addr*)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}


void enc28j60_periodic(struct netif *netif)
{
	enc28j60_if_input(netif);
}
