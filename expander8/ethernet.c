#include "ethernet.h"
#include <public.h>
#include <hardware.h>
#include <settings.h>
#include <delay.h>
#include <myprintf.h>

#include "providers.h"

#include "Ethernet/W5500/w5500.h"
#include "Ethernet/socket.h"
#include "Internet/DHCP/dhcp.h"

extern int dodump;

// consts
const static uint8_t _HW_ADDR[] = { HW_ADDR };
// const static uint8_t _IP_ADDR[] = { IP_ADDR };
// const static uint8_t _IP_MASK[] = { IP_MASK };
// const static uint8_t _IP_GW[] = { IP_GW };
const static uint8_t _IP_SRV[] = { IP_SRV };

// public

// private
wiz_NetInfo info;
// struct udp_pcb *eth_udppcb;
// struct netif eth_netif;
uint16_t eth_recvPacketId = 0;

void eth_initRegisters();
// void eth_onUDPDataReceived(void* arg, struct udp_pcb* upcb, struct pbuf* p, struct ip_addr* addr, u16_t port);

// public
uint8_t w5500_rw(uint8_t val)
{
	SPI2->DR = val;
	while (!(SPI2->SR & SPI_SR_TXE));
	while (!(SPI2->SR & SPI_SR_RXNE));
	return SPI2->DR;
}
uint8_t wizchip_spi_readbyte(void)
{
	return w5500_rw(0xff);
}
void wizchip_spi_writebyte(uint8_t wb)
{
	w5500_rw(wb);
}
void wizchip_cs_select(void)
{
	IO_LOW(W_CS);
	_delay_us(1);
}
void wizchip_cs_deselect(void)
{
	_delay_us(1);
	IO_HIGH(W_CS);
}

#define OPT_SIZE                 312               /// Max OPT size of @ref RIP_MSG
#define RIP_MSG_SIZE             (236+OPT_SIZE)    /// Max size of @ref RIP_MSG
uint8_t dhcpData[RIP_MSG_SIZE];
uint8_t dhcpEnabled = 0;

void dhcpAssign()
{
	uint8_t ip[4];
	getIPfromDHCP(info.ip);
	getGWfromDHCP(info.gw);
	getSNfromDHCP(info.sn);
	wizchip_setnetinfo(&info);
	
	myprintf("dhcpAssign eth %d.%d.%d.%d\r\n", info.ip[0], info.ip[1], info.ip[2], info.ip[3]);
}
void dhcpUpdate()
{
	myprintf("dhcpUpdate eth\r\n");
}
void dhcpConflict()
{
	myprintf("dhcpConflict eth\r\n");
}

void ethInit()
{
	myprintf("conf eth\r\n");
	// configure SPI
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	IO_PUSH_PULL(W_CS);
	IO_ALT_PUSH_PULL(SCK);
	IO_ALT_PUSH_PULL(MOSI);
	IO_INPUT(MISO);
	SPI2->CR1 = //SPI_CR1_BR_0 | //SPI_CR1_BR_1 | SPI_CR1_BR_0 |
	  SPI_CR1_MSTR | SPI_CR1_SPE | SPI_CR1_SSM | SPI_CR1_SSI;
	  
	IO_INPUT_PP(INT_ETH);
	IO_HIGH(INT_ETH);
	
	memcpy(info.mac, HW_ADDR, 6);
	info.ip[0] = 192;
	info.ip[1] = 168;
	info.ip[2] = 2;
	info.ip[3] = 52;
	info.gw[0] = 192;
	info.gw[1] = 168;
	info.gw[2] = 2;
	info.gw[3] = 1;
	info.sn[0] = 255;
	info.sn[1] = 255;
	info.sn[2] = 255;
	info.sn[3] = 0;
	info.dhcp = NETINFO_DHCP;
	
	
	u8 rx[] = { 1, 4, 0, 0, 0, 0, 0, 0 };
	wizchip_init(rx, rx);
	wizchip_setnetinfo(&info);
	
	// reg_dhcp_cbfunc(dhcpAssign, dhcpUpdate, dhcpConflict);
	// DHCP_init(0, dhcpData);
	
	int q = socket(1, Sn_MR_UDP, 9999, SF_IO_NONBLOCK);
	myprintf("socket: %d\r\n", q);
	
	// configure module
	// _delay_ms(100);
	// enc28j60Reset();
	// _delay_ms(500);
	
	// eth_initRegister();
	
	// struct ip_addr ip_addr;
	// struct ip_addr net_mask;
	// struct ip_addr gw_addr;
	
	// // IP4_ADDR(&ip_addr, _IP_ADDR[0], _IP_ADDR[1], _IP_ADDR[2], _IP_ADDR[3]);
	// // IP4_ADDR(&net_mask, _IP_MASK[0], _IP_MASK[1], _IP_MASK[2], _IP_MASK[3]);
	// // IP4_ADDR(&gw_addr, _IP_GW[0], _IP_GW[1], _IP_GW[2], _IP_GW[3]);
	
	// myprintf("lwip_init\r\n");
	// lwip_init();
	// myprintf("lwip_init done\r\n");
	
	// eth_netif.hwaddr[0] = _HW_ADDR[0];
	// eth_netif.hwaddr[1] = _HW_ADDR[1];
	// eth_netif.hwaddr[2] = _HW_ADDR[2];
	// eth_netif.hwaddr[3] = _HW_ADDR[3];
	// eth_netif.hwaddr[4] = _HW_ADDR[4];
	// eth_netif.hwaddr[5] = _HW_ADDR[5];
	
	// // netif_add(&eth_netif, &ip_addr, &net_mask, &gw_addr, 0, enc28j60_if_init, ip_input);
	// netif_add(&eth_netif, 0, 0, 0, 0, enc28j60_if_init, ip_input);
	// myprintf("netif_add done\r\n");
	
	// netif_set_default(&eth_netif);
	// myprintf("netif_set_up done\r\n");
	
	// eth_udppcb = udp_new();
	// struct ip_addr addr;
	// udp_bind(eth_udppcb, IP_ADDR_ANY, LOCAL_PORT);
	// IP4_ADDR(&addr, _IP_SRV[0], _IP_SRV[1], _IP_SRV[2], _IP_SRV[3]);
	// udp_connect(eth_udppcb, &addr, SRV_PORT);
	// udp_recv(eth_udppcb, &eth_onUDPDataReceived, 0);
}
void ethProcess()
{
	static uint32_t lastCheck = 0;
	
	if (ticks - lastCheck >= 1000)
	{
		lastCheck = ticks;
		
		TProvHeader header;
		TByteBuffer b;
		provPrepareHeader(&header);
		
		header.type = 0x0000;
		provSendPacket(&header, sizeof(header));
			// ethFreeBuffer(&b);
		
		
		DHCP_time_handler();
		// if (dodump)
		// enc28j60Dump();
		// uint8_t c = getPHYCFGR();
		// if (c & PHYCFGR_LNK_ON)
			// myprintf("link on, ");
		// if (c & PHYCFGR_SPD_100)
			// myprintf("100, ");
		// else
			// myprintf("10, ");
		// if (c & PHYCFGR_DPX_FULL)
			// myprintf("full, ");
		// else
			// myprintf("half, ");
		// myprintf("\r\n");
	}
	
	uint8_t c = getPHYCFGR();
	if (c & PHYCFGR_LNK_ON)
	{
		if (!dhcpEnabled)
		{
			dhcpEnabled = 1;
			DHCP_init(0, dhcpData);
			myprintf("dhcp start\r\n");
		}
		uint8_t r = DHCP_run();
		// myprintf("dhcp %d\r\n", r);
	}
	else
	{
		if (dhcpEnabled)
		{
			DHCP_stop();
			dhcpEnabled = 0;
			myprintf("dhcp stop\r\n");
		}
	}


	uint8_t data[1024];
	uint8_t addr[4];
	uint16_t port;
	int r = recvfrom(1, (char*)data, 1024, (char*)&addr, &port);

	// myprintf("r %d\r\n", r);
	if(r)
	{

		provProcess(data, r);
	}
	
	// if (IO_IS_LOW(INT_ETH))
	// {
	// uint8_t eir = enc28j60ReadControl(EIR);
	// myprintf("eir 0x%02x\r\n", eir);
	// if (eir & EIR_LINKIF)
	// {
	// uint16_t phir = enc28j60ReadPhyWord(PHIR);
	// if (phir & PHIR_PLNKIF)
	// {
	// uint16_t stat = enc28j60ReadPhyWord(PHSTAT2);
	// if (stat & PHSTAT2_LSTAT) // link is up
	// {
	// myprintf("LINK UP\r\n");
	// netif_set_up(&eth_netif);
	// dhcp_start(&eth_netif);
	// }
	// else // link is down or was for a period
	// {
	// myprintf("LINK DOWN\r\n");
	// dhcp_stop(&eth_netif);
	// netif_set_down(&eth_netif);
	// }
	// }
	// myprintf("phir: 0x%04x\r\n", phir);
	// }
	// enc28j60_if_input(&eth_netif);
	// }
}
void ethFreeBuffer(TByteBuffer* buffer)
{
	// pbuf_free(buffer->p);
	// buffer->p = 0;
}
void provSendPacket(const void* buffer, int len)
{
	int s = sendto(1, (char*)buffer, len, (char*)_IP_SRV, SRV_PORT);
	// myprintf("s %d\r\n", s);
	// bufferPrint(buffer);
	// udp_send(eth_udppcb, buffer->p);
}

// void eth_onUDPDataReceived(void* arg, struct udp_pcb* upcb, struct pbuf* p, struct ip_addr* addr, u16_t port)
// {
// TByteBuffer buffer;
// buffer.p = p;
// buffer.pos = 0;

// char *srcAddr = ipaddr_ntoa(addr);
// myprintf("src addr: %s\r\n", srcAddr);

// uint16_t packetId;
// if (BYTEBUFFER_FETCH(&buffer, packetId)) return;

// provProcess(&buffer);

// pbuf_free(p);
// }
