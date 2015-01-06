/*
 * esp.h
 * Copyright (C) 2014 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#ifndef __ESP_H__
#define __ESP_H__

void espInit();
void espProcess();
void espUsartHandler();

void espSendPacket(unsigned char cmd, const unsigned char* buf, int len);

#endif
