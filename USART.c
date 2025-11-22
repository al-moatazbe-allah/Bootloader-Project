#include "usart.h"
#include "stm32f4xx.h"

void usart1_init(uint32_t pclk2, uint32_t baud)
{
    // Enable GPIOA and USART1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    __DSB();

    // PA9: TX, PA10: RX -> AF7
    // PA9
    GPIOA->MODER &= ~(0x3 << (9 * 2));
    GPIOA->MODER |=  (0x2 << (9 * 2));
    GPIOA->OSPEEDR |= (0x3 << (9 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((9-8)*4));
    GPIOA->AFR[1] |=  (7 << ((9-8)*4));
    // PA10
    GPIOA->MODER &= ~(0x3 << (10 * 2));
    GPIOA->MODER |=  (0x2 << (10 * 2));
    GPIOA->OSPEEDR |= (0x3 << (10 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((10-8)*4));
    GPIOA->AFR[1] |=  (7 << ((10-8)*4));

    // Disable USART before config
    USART1->CR1 &= ~USART_CR1_UE;
    // word length 8, parity none
    USART1->CR1 &= ~(USART_CR1_M | USART_CR1_PCE);

    // stop bits 1
    USART1->CR2 &= ~USART_CR2_STOP;

    // Baud rate: BRR = pclk2 / baud
    uint32_t usartdiv = (pclk2 + (baud/2)) / baud;
    USART1->BRR = usartdiv;

    // enable TX/RX
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    // enable
    USART1->CR1 |= USART_CR1_UE;
}

int usart_tx_ready(void) { return (USART1->SR & USART_SR_TXE); }
int usart_rx_ready(void) { return (USART1->SR & USART_SR_RXNE); }

void usart_putc(uint8_t c)
{
    while (!usart_tx_ready());
    USART1->DR = c;
}

uint8_t usart_getc_blocking(void)
{
    while (!usart_rx_ready());
    return (uint8_t)(USART1->DR & 0xFF);
}

void usart_write(const uint8_t *buf, uint32_t len)
{
    for (uint32_t i=0;i<len;i++) usart_putc(buf[i]);
}
