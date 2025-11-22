#include "flash_helpers.h"
#include "stm32f4xx.h"

// Very simplified flash helpers for educational purposes.
// Verify with reference manual and device-specific flash layout before use.

int flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123U;
        FLASH->KEYR = 0xCDEF89ABU;
    }
    return 0;
}

int flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
    return 0;
}

static int flash_wait_busy(void)
{
    while (FLASH->SR & FLASH_SR_BSY) { }
    if (FLASH->SR & (FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR | FLASH_SR_WRPERR)) {
        FLASH->SR |= FLASH_SR_EOP;
        return -1;
    }
    return 0;
}

int flash_erase_range(uint32_t start_address, uint32_t end_address)
{
    // For STM32F4 (different devices use sectors): We'll use page/sector erase via FLASH_CR_PER.
    // This implementation assumes uniform page size; adjust to your device.
    const uint32_t PAGE_SIZE = 0x4000; // 16KB example - adjust for STM32F411
    uint32_t addr = start_address & ~(PAGE_SIZE - 1);
    flash_unlock();
    while (addr < end_address) {
        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = addr;
        FLASH->CR |= FLASH_CR_STRT;
        if (flash_wait_busy() != 0) return -1;
        FLASH->CR &= ~FLASH_CR_PER;
        addr += PAGE_SIZE;
    }
    flash_lock();
    return 0;
}

int flash_program_word(uint32_t address, uint32_t data)
{
    flash_unlock();
    if (flash_wait_busy() != 0) return -1;
    FLASH->CR |= FLASH_CR_PG;
    uint32_t *ptr = (uint32_t *)address;
    *ptr = data;
    if (flash_wait_busy() != 0) return -1;
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    return 0;
}
