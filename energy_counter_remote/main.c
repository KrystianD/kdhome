#include <public.h>
#include <hardware.h>
#include <settings.h>
#include <delay.h>
#include <myprintf.h>

#include <RFM70.h>

#define UART_TX GPIOA,9
#define UART_RX GPIOA,10

#define INPUT GPIOA,0

#define CE GPIOA,14
#define CSN GPIOB,1
#define MOSI GPIOA,7
#define MISO GPIOA,6
#define SCK GPIOA,5

volatile uint32_t ticks = 0;

struct
{
	uint32_t counter, time, vdd;
} data;

void myputchar(int c)
{
	USART1->TDR = c;
	while (!(USART1->ISR & USART_ISR_TC));
}

uint8_t rfm70EnableChip()
{
	IO_HIGH(CE);
	return 0;
}
uint8_t rfm70DisableChip()
{
	IO_LOW(CE);
	return 0;
}
uint8_t rfm70_SPI_RW(uint8_t val)
{
	*(volatile uint8_t*)&SPI1->DR = val;
	while (!(SPI1->SR & SPI_SR_TXE));
	while (!(SPI1->SR & SPI_SR_RXNE));
	return SPI1->DR;
}

uint8_t rfm70SPIReadCommand(uint8_t cmd, uint8_t* data, uint8_t len)
{
	IO_LOW(CSN);
	rfm70_SPI_RW(cmd);
	while (len--)
		*data++ = rfm70_SPI_RW(0xff);
	IO_HIGH(CSN);
	return 0;
}
uint8_t rfm70SPISendCommand(uint8_t cmd, const uint8_t* data, uint8_t len)
{
	IO_LOW(CSN);
	rfm70_SPI_RW(cmd);
	while (len--)
		rfm70_SPI_RW(*data++);
	IO_HIGH(CSN);
	return 0;
}

volatile uint16_t *VREFINT_CAL = 0x1ffff7ba;

void main()
{
	SYSCFG->CFGR1 = 0;
	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_USART1EN;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	SysTick->LOAD = SysTick->VAL = (F_CPU / 1000) / 8;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
	
	data.counter = 0;
	
	IO_ALT_PUSH_PULL(UART_TX);
	IO_ALT_PUSH_PULL(UART_RX);
	IO_ALT_SET(UART_TX, 1);
	IO_ALT_SET(UART_RX, 1);
	USART1->BRR = USART_BRR(115200);
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	USART1->CR3 = USART_CR3_OVRDIS;
	ENABLE_INTERRUPT(USART1_IRQn);
	ENABLE_INTERRUPT(EXTI0_1_IRQn);
	_delay_init();
	_delay_ms(500);
	myprintf("START\r\n");
	
	IO_PUSH_PULL(CSN);
	IO_PUSH_PULL(CE);
	
	IO_ALT_PUSH_PULL(SCK);
	IO_ALT_PUSH_PULL(MISO);
	IO_ALT_PUSH_PULL(MOSI);
	IO_ALT_SET(SCK, 0);
	IO_ALT_SET(MISO, 0);
	IO_ALT_SET(MOSI, 0);
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH;
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SPE | SPI_CR1_SSM | SPI_CR1_SSI;
	  
	rfm70Init();
	rfm70EnableFeatures();
	
	uint8_t val;
	rfm70EnableCRC();
	rfm70Set1Mbps();
	rfm70SetCRC2bytes();
	rfm70WriteRegisterValue(RFM70_SETUP_RETR, 0xff);
	rfm70WriteRegisterValue(RFM70_RF_CH, 30);
	rfm70WriteRegisterValue(RFM70_RX_PW_P0, 12);
	
	rfm70SetTxAddress("\xe7\xe7\xe7\xe7\xe7", 5);
	rfm70SetRxAddress(0, "\xe7\xe7\xe7\xe7\xe8", 5);
	
	IO_INPUT(INPUT);
	
	rfm70PrintStatus();
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	ADC1->CR = 0;
	ADC1->CR = ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL);
	
	ADC1->SMPR = 0x07;
	ADC1->CHSELR = ADC_CHSELR_CHSEL17;
	
	myprintf("VREFINT_CAL: %d\r\n", *VREFINT_CAL);
	
	EXTI->IMR |= 1;
	EXTI->FTSR |= 1;
	
	rfm70PowerDown();
	rfm70DisableChip();
	myprintf("GO\r\n");
	for (;;)
	{
		PWR->CR |= PWR_CR_CWUF;
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		__WFI();
	}
}

void USART1_Handler()
{
	if (USART1->ISR & USART_ISR_ORE)
	{
		USART1->ICR = USART_ICR_ORECF;
	}
	if (USART1->ISR & USART_ISR_RXNE)
	{
		uint8_t d = USART1->RDR;
		// if (d == 0x7f)
		// {
			// NVIC_SystemReset();
		// }
	}
}
void SysTick_Handler()
{
	ticks++;
}
void _errorloop()
{
	while (1)
	{
		_delay_ms(100);
	}
}
void EXTI0_1_Handler()
{
	EXTI->PR = 1;
	
	data.counter++;

	ADC1->CR = ADC_CR_ADEN;
	while (!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->ISR = ADC_ISR_ADRDY;
	ADC->CCR = ADC_CCR_VREFEN;
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	ADC1->CR = ADC_CR_ADSTART;
	
	rfm70PowerUp();
	rfm70EnableChip();
	rfm70SwitchToTxMode();
	rfm70WriteRegisterValue(RFM70_SETUP_RETR, 0x1a);
	rfm70WriteRegisterValue(RFM70_RF_CH, 30);
	rfm70SPISendCommand(RFM70_FLUSH_TX, 0, 0);
	// rfm70WriteTxPayloadNOACK((uint8_t*)&counter, 3);
	rfm70WriteTxPayload((uint8_t*)&data, 12);
	
	uint32_t time = ticks;
	uint8_t s;
	do
	{
		rfm70DataSentOrMaxRetr(&s);
	}
	while (!s);
	time = ticks - time;
	myprintf("send time: %d\r\n", time);
	
	rfm70PowerDown();
	rfm70DisableChip();
	
	while (!(ADC1->ISR & ADC_ISR_EOC));
	uint32_t d = ADC1->DR;
	uint32_t vdd = *VREFINT_CAL * 3300 / d;
	myprintf("cnd %d vdd %d\r\n", data.counter, vdd);
	data.vdd = vdd;

	ADC1->CR = ADC_CR_ADDIS;
	while (ADC1->CR & ADC_CR_ADEN);
}
