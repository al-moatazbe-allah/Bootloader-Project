# Simple Makefile - requires arm-none-eabi toolchain and CMSIS headers in include path
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -mcpu=cortex-m4 -mthumb -Os -ffunction-sections -fdata-sections -Wall -Wextra -I.
LDFLAGS = -T stm32f411re_boot.ld -nostartfiles -Wl,--gc-sections

SRCS = bootloader.c usart.c flash_helpers.c
OBJS = $(SRCS:.c=.o)

all: bootloader.bin bootloader.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bootloader.elf: startup_stm32.o $(OBJS)
	$(CC) $(CFLAGS) startup_stm32.o $(OBJS) $(LDFLAGS) -o $@

bootloader.bin: bootloader.elf
	$(OBJCOPY) -O binary bootloader.elf bootloader.bin

startup_stm32.o: startup_stm32.s
	$(CC) $(CFLAGS) -c startup_stm32.s -o startup_stm32.o

clean:
	rm -f *.o *.elf *.bin

.PHONY: all clean
