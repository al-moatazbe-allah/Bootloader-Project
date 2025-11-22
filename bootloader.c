#include "usart.h"
#include "flash_helpers.h"
#include <stdint.h>
#include <string.h>

#define APP_START_ADDR   0x08008000U
#define MAX_FIRMWARE_SIZE (256*1024) // 256KB cap

extern uint32_t SystemCoreClock;

typedef void (*pfunc_t)(void);

static void jump_to_app(uint32_t app_addr)
{
    uint32_t sp = *((uint32_t *)app_addr);
    uint32_t reset = *((uint32_t *)(app_addr + 4));

    // Basic stack pointer sanity check (SRAM region for STM32F4 starts at 0x20000000)
    if ((sp & 0xFF000000U) != 0x20000000U) {
        // invalid app
        return;
    }

    __disable_irq();

    // Set vector table
    SCB->VTOR = app_addr;

    __set_MSP(sp);
    pfunc_t reset_handler = (pfunc_t)reset;
    reset_handler();
}

static int receive_u32_le(uint32_t *out) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t b = usart_getc_blocking();
        v |= ((uint32_t)b) << (8 * i);
    }
    *out = v;
    return 0;
}

int main(void)
{
    // Assume system init done (SystemInit) and SystemCoreClock set by startup code
    usart1_init(SystemCoreClock, 115200);

    const char *msg = "BOOTLOADER READY\r\n";
    usart_write((const uint8_t*)msg, strlen(msg));

    // Wait briefly for command
    uint8_t cmd = usart_getc_blocking();
    if (cmd != 'U') {
        // No upload command -> try jump to app if valid
        uint32_t sp = *((uint32_t *)APP_START_ADDR);
        if ((sp & 0xFF000000U) == 0x20000000U) {
            usart_putc('J');
            jump_to_app(APP_START_ADDR);
        }
        // else loop
        while (1) __asm__("wfi");
    }

    usart_putc('A'); // ACK

    uint32_t length = 0;
    receive_u32_le(&length);
    usart_putc('A');

    if (length == 0 || length > MAX_FIRMWARE_SIZE) {
        usart_putc('E');
        while (1) __asm__("wfi");
    }

    // Erase necessary pages/sectors
    uint32_t start = APP_START_ADDR;
    uint32_t end = APP_START_ADDR + length;
    if (flash_erase_range(start, end) != 0) {
        usart_putc('E');
        while (1) __asm__("wfi");
    }

    // Receive and program
    uint32_t addr = APP_START_ADDR;
    uint8_t buf[4];
    uint32_t received = 0;
    while (received < length) {
        for (int i = 0; i < 4; ++i) {
            if (received + i < length) buf[i] = usart_getc_blocking();
            else buf[i] = 0xFF;
        }
        uint32_t word = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
        if (flash_program_word(addr, word) != 0) {
            usart_putc('E');
            while (1) __asm__("wfi");
        }
        addr += 4;
        received += 4;
    }

    usart_putc('O'); // OK
    // Optionally verify here
    jump_to_app(APP_START_ADDR);

    while (1) __asm__("wfi");
    return 0;
}
