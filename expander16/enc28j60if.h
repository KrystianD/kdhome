#ifndef __ENC29J60IF_H__
#define __ENC29J60IF_H__

err_t enc28j60_if_init(struct netif *netif);
void enc28j60_if_input(struct netif *netif);

#endif //__ENC29J60IF_H__
