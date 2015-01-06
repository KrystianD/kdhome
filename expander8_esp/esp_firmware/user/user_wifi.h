/*
 * user_wifi.h
 * Copyright (C) 2015 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#ifndef __USER_WIFI_H__
#define __USER_WIFI_H__

void wifiInit();
void wifiProcess();

void wifiSendStatus();
void wifiSendFrame();
void wifiTransmitFrame(const unsigned char* buffer, int len);

#endif
