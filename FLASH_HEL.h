#ifndef _FLASH_HELPERS_H
#define _FLASH_HELPERS_H
#include <stdint.h>

int flash_erase_range(uint32_t start_address, uint32_t end_address);
int flash_program_word(uint32_t address, uint32_t data);

#endif
