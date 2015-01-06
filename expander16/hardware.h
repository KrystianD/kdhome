#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define UART_TX GPIOA,9

#define EXP_I2C I2C2
#define SCL GPIOB,10
#define SDA GPIOB,11
#define INT GPIOB,1

#define LED GPIOA,15
#define OW GPIOA,2

#define IR GPIOA,7

#define SCK GPIOB,13
#define MISO GPIOB,14
#define MOSI GPIOB,15
#define CS GPIOA,8
#define INT_ETH GPIOB,12

#define PCF_ADDR 0b01110000
#define PCF_OUT0 (0b010 << 1)
#define PCF_OUT1 (0b001 << 1)
#define PCF_IN0  (0b011 << 1)
#define PCF_IN1  (0b000 << 1)

// // <debug only>
// #define ETHERNET_MODULE
// #define RESET GPIOA,11
// #define LED GPIOA,3
// #define INT_ETH GPIOB,11
// #define SCK GPIOB,13
// #define MISO GPIOB,14
// #define MOSI GPIOB,15
// #define CS GPIOB,12
// #define IN0 GPIOA,4
// #define IN1 GPIOA,5
// #define OUT0 GPIOA,3 // led
// // </debug only>

#endif
