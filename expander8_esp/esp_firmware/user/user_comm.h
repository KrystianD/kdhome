/*
 * user_comm.h
 * Copyright (C) 2015 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#ifndef __USER_COMM_H__
#define __USER_COMM_H__

void commProcessChar(unsigned char c);
void commProcess();

void commSendPacket(unsigned char cmd, const unsigned char* buf, int len);
void commProcessCommand(unsigned char cmd, const unsigned char* buf, int len);

#endif
