
#define NO_SYS                          1
#define MEM_SIZE                        1000
#define LWIP_RAW                        0
#define LWIP_TCP                        0
#define LWIP_DHCP                       1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0

#define IP_REASSEMBLY                   0
#define IP_FRAG                         0

#define MEMP_NUM_PBUF                   1
#define PBUF_POOL_SIZE                  4

#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
#define ETHARP_DEBUG                    LWIP_DBG_ON
#define NETIF_DEBUG                     LWIP_DBG_ON
#define PBUF_DEBUG                      LWIP_DBG_ON
#define API_LIB_DEBUG                   LWIP_DBG_ON
#define API_MSG_DEBUG                   LWIP_DBG_ON
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#define ICMP_DEBUG                      LWIP_DBG_ON
#define IGMP_DEBUG                      LWIP_DBG_ON
#define INET_DEBUG                      LWIP_DBG_ON
#define IP_DEBUG                        LWIP_DBG_ON
#define IP_REASS_DEBUG                  LWIP_DBG_ON
#define RAW_DEBUG                       LWIP_DBG_ON
#define MEM_DEBUG                       LWIP_DBG_ON
#define MEMP_DEBUG                      LWIP_DBG_ON
#define SYS_DEBUG                       LWIP_DBG_ON
#define TIMERS_DEBUG                    LWIP_DBG_ON
#define TCP_DEBUG                       LWIP_DBG_ON
#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
#define TCP_FR_DEBUG                    LWIP_DBG_ON
#define TCP_RTO_DEBUG                   LWIP_DBG_ON
#define TCP_CWND_DEBUG                  LWIP_DBG_ON
#define TCP_WND_DEBUG                   LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
#define TCP_RST_DEBUG                   LWIP_DBG_ON
#define TCP_QLEN_DEBUG                  LWIP_DBG_ON
#define UDP_DEBUG                       LWIP_DBG_ON
#define TCPIP_DEBUG                     LWIP_DBG_ON
#define PPP_DEBUG                       LWIP_DBG_ON
#define SLIP_DEBUG                      LWIP_DBG_ON
#define DHCP_DEBUG                      LWIP_DBG_ON
#define AUTOIP_DEBUG                    LWIP_DBG_ON
#define SNMP_MSG_DEBUG                  LWIP_DBG_ON
#define SNMP_MIB_DEBUG                  LWIP_DBG_ON
#define DNS_DEBUG                       LWIP_DBG_ON

#define CHECKSUM_CHECK_IP               1
#define CHECKSUM_CHECK_UDP              1

#define CHECKSUM_GEN_IP                 1
#define CHECKSUM_GEN_UDP                1
#define CHECKSUM_GEN_ICMP               1

#define MaxFrameLen	(1500)
#define MaxFrameLenRX	(6 + MaxFrameLen)
#define TXStart			(8192 - (1 + MaxFrameLen + 7)) // 1 control byte + 7 status bytes
#define RXStart			0
#define RXEnd				((TXStart - 2) | 1)

#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(MaxFrameLen+40+PBUF_LINK_HLEN)
