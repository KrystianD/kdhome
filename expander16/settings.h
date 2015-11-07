#include <advinputmanager_memory.h>

#include "providers_settings.h"

#define DELAY_TIMER TIM4
#define DELAY_TIMER_APB APB1ENR
#define DELAY_TIMER_APBEN RCC_APB1ENR_TIM4EN

// #define ADVIM_DEBUG 1
// #define ADVIM_PRINTF myprintf
#define ADVIM_INPUTSCOUNT INPUTS_COUNT 
// #define ADVIM_PATTERNENABLED
// #define ADVIM_PATTERNMAX 20

#define PCLK1_FREQ F_CPU


#define OW_USART       USART2
#define OW_USART_APB   APB1ENR
#define OW_USART_APBEN RCC_APB1ENR_USART2EN

// IP settings
#define HW_ADDR 0x00,0xC4,0xDE,0x6D,0xD5,0xEE
// #define IP_ADDR 192,168,1,50
// #define IP_MASK 255,255,255,0
// #define IP_GW   192,168,1,1
// #define IP_SRV  192,168,100,1
// #define IP_SRV  192,168,100,1
#define IP_SRV  10,12,4,11

#define LOCAL_PORT 9999
#define SRV_PORT 9999

#define ADVIM_INPUT0_ADDR 0
#define ADVIM_INPUT0_FLAGS 0
#define ADVIM_INPUT0_DEBOUNCETIME 20
#define ADVIM_INPUT0_PATTERNTIMEOUT 0
#define ADVIM_INPUT1_ADDR 1
#define ADVIM_INPUT1_FLAGS 0
#define ADVIM_INPUT1_DEBOUNCETIME 20
#define ADVIM_INPUT1_PATTERNTIMEOUT 0
#define ADVIM_INPUT2_ADDR 2
#define ADVIM_INPUT2_FLAGS 0
#define ADVIM_INPUT2_DEBOUNCETIME 20
#define ADVIM_INPUT2_PATTERNTIMEOUT 0
#define ADVIM_INPUT3_ADDR 3
#define ADVIM_INPUT3_FLAGS 0
#define ADVIM_INPUT3_DEBOUNCETIME 20
#define ADVIM_INPUT3_PATTERNTIMEOUT 0
#define ADVIM_INPUT4_ADDR 4
#define ADVIM_INPUT4_FLAGS 0
#define ADVIM_INPUT4_DEBOUNCETIME 20
#define ADVIM_INPUT4_PATTERNTIMEOUT 0
#define ADVIM_INPUT5_ADDR 5
#define ADVIM_INPUT5_FLAGS 0
#define ADVIM_INPUT5_DEBOUNCETIME 20
#define ADVIM_INPUT5_PATTERNTIMEOUT 0
#define ADVIM_INPUT6_ADDR 6
#define ADVIM_INPUT6_FLAGS 0
#define ADVIM_INPUT6_DEBOUNCETIME 20
#define ADVIM_INPUT6_PATTERNTIMEOUT 0
#define ADVIM_INPUT7_ADDR 7
#define ADVIM_INPUT7_FLAGS 0
#define ADVIM_INPUT7_DEBOUNCETIME 20
#define ADVIM_INPUT7_PATTERNTIMEOUT 0
#define ADVIM_INPUT8_ADDR 8
#define ADVIM_INPUT8_FLAGS 0
#define ADVIM_INPUT8_DEBOUNCETIME 20
#define ADVIM_INPUT8_PATTERNTIMEOUT 0
#define ADVIM_INPUT9_ADDR 9
#define ADVIM_INPUT9_FLAGS 0
#define ADVIM_INPUT9_DEBOUNCETIME 20
#define ADVIM_INPUT9_PATTERNTIMEOUT 0
#define ADVIM_INPUT10_ADDR 10
#define ADVIM_INPUT10_FLAGS 0
#define ADVIM_INPUT10_DEBOUNCETIME 20
#define ADVIM_INPUT10_PATTERNTIMEOUT 0
#define ADVIM_INPUT11_ADDR 11
#define ADVIM_INPUT11_FLAGS 0
#define ADVIM_INPUT11_DEBOUNCETIME 20
#define ADVIM_INPUT11_PATTERNTIMEOUT 0
#define ADVIM_INPUT12_ADDR 12
#define ADVIM_INPUT12_FLAGS 0
#define ADVIM_INPUT12_DEBOUNCETIME 20
#define ADVIM_INPUT12_PATTERNTIMEOUT 0
#define ADVIM_INPUT13_ADDR 13
#define ADVIM_INPUT13_FLAGS 0
#define ADVIM_INPUT13_DEBOUNCETIME 20
#define ADVIM_INPUT13_PATTERNTIMEOUT 0
#define ADVIM_INPUT14_ADDR 14
#define ADVIM_INPUT14_FLAGS 0
#define ADVIM_INPUT14_DEBOUNCETIME 20
#define ADVIM_INPUT14_PATTERNTIMEOUT 0
#define ADVIM_INPUT15_ADDR 15
#define ADVIM_INPUT15_FLAGS 0
#define ADVIM_INPUT15_DEBOUNCETIME 20
#define ADVIM_INPUT15_PATTERNTIMEOUT 0
