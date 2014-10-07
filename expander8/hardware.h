#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define UART_TX GPIOA,9

#define LED GPIOA,11
#define OW GPIOB,10

#define SCK GPIOB,13
#define MISO GPIOB,14
#define MOSI GPIOB,15
#define W_CS GPIOA,8
#define INT_ETH GPIOB,12

#define IN1 GPIOA,14
#define IN2 GPIOA,15
#define IN3 GPIOB,3
#define IN4 GPIOB,4
#define IN5 GPIOB,5
#define IN6 GPIOB,6
#define IN7 GPIOB,7
#define IN8 GPIOB,8

#if VERSION == 1
#define OUT1 GPIOB,11
#define OUT2 GPIOB,1
#define OUT3 GPIOB,0
#define OUT4 GPIOA,7
#define OUT5 GPIOA,6
#define OUT6 GPIOA,5
#define OUT7 GPIOA,4
#define OUT8 GPIOB,9
#elif VERSION == 2
#define OUT1 GPIOC,15
#define OUT2 GPIOB,1
#define OUT3 GPIOB,0
#define OUT4 GPIOA,7
#define OUT5 GPIOA,6
#define OUT6 GPIOA,5
#define OUT7 GPIOA,4
#define OUT8 GPIOB,9
#endif

#endif
