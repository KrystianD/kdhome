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

// consts
const static uint8_t _HW_ADDR[] = { HW_ADDR };
// const static uint8_t _IP_ADDR[] = { IP_ADDR };
// const static uint8_t _IP_MASK[] = { IP_MASK };
// const static uint8_t _IP_GW[] = { IP_GW };
const static uint8_t _IP_SRV[] = { IP_SRV };

// public

// private
wiz_NetInfo info;
uint16_t eth_recvPacketId = 0;

void eth_initRegisters();

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
	info.dhcp = NETINFO_DHCP;
	
	u8 rx[] = { 1, 4, 0, 0, 0, 0, 0, 0 };
	wizchip_init(rx, rx);
	wizchip_setnetinfo(&info);
	
	int q = socket(1, Sn_MR_UDP, 9999, SF_IO_NONBLOCK);
	myprintf("socket: %d\r\n", q);
}
void ethProcess()
{
	static uint32_t lastCheck = 0;
	
	if (ticks - lastCheck >= 1000)
	{
		lastCheck = ticks;
		
		TProvHeader header;
		provPrepareHeader(&header);
		
		header.type = PROVIDER_TYPE_CONTROL;
		provSendPacket(&header, sizeof(header));
		
		DHCP_time_handler();
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
	
	if (r)
	{
		provProcess(data, r);
	}
}
void provSendPacket(const void* buffer, int len)
{
	int s = sendto(1, (char*)buffer, len, (char*)_IP_SRV, SRV_PORT);
}
