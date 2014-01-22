#ifndef __ENC28J60_H__
#define __ENC28J60_H__

#include <stdint.h>
#include <ENC28J60_defs.h>

extern void enc28j60_enableChip();
extern void enc28j60_disableChip();
extern uint8_t enc28j60_rw(uint8_t val);

void enc28j60SelectBank(uint8_t bank);
uint8_t enc28j60SelectBankByAddr(uint8_t addr);
uint8_t enc28j60ReadControl(uint8_t addr);
uint16_t enc28j60ReadControlWord(uint8_t addr);
void enc28j60WriteControl(uint8_t addr, uint8_t val);
void enc28j60WriteControlCheck(uint8_t addr, uint8_t val);
void enc28j60WriteControlWord(uint8_t addr, uint16_t val);
void enc28j60WriteControlWordCheck(uint8_t addr, uint16_t val);
uint16_t enc28j60ReadPhyWord(uint8_t addr);
void enc28j60WritePhyWord(uint8_t addr, uint16_t val);
void enc28j60WritePhyWordCheck(uint8_t addr, uint16_t val);
void enc28j60SetBits(uint8_t addr, uint8_t mask);
void enc28j60ClearBits(uint8_t addr, uint8_t mask);
void enc28j60Reset();

void enc28j60ReadBuffer(uint8_t* data, uint16_t len);
void enc28j60WriteBuffer(const uint8_t* data, uint16_t len);
void enc28j60WriteBufferByte(uint8_t data);
void enc28j60WriteBufferWordBE(uint16_t data);

void enc28j60WaitForTransmitReady();

// utils
uint8_t ENC28J60GetLinkStatus();
void enc28j60SendPacket();
void enc28j60SetFullDuplex(uint8_t enabled);

#ifdef ENC28J60_DEBUG
void enc28j60PrintMemory();
void enc28j60PrintRXStatus(ENC28J60_RXSTATUS* status);
void enc28j60PrintTXStatus(ENC28J60_TXSTATUS* status);
void enc28j60PrintESTAT(uint8_t status);
void enc28j60Dump();
#endif

#endif
