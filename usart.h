#ifndef _USART_H
#define _USART_H
#include <stdint.h>

void usart1_init(uint32_t pclk2, uint32_t baud);
void usart_putc(uint8_t c);
uint8_t usart_getc_blocking(void);
void usart_write(const uint8_t *buf, uint32_t len);

#endif
