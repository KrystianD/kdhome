
#define DELAY_TIMER TIM3
#define DELAY_TIMER_APB APB1ENR
#define DELAY_TIMER_APBEN RCC_APB1ENR_TIM3EN

#include <myprintf.h>
#define RFM70_DEBUG(x,...) myprintf(x, ##__VA_ARGS__)
