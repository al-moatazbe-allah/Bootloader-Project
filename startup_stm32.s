    .syntax unified
    .cpu cortex-m4
    .thumb

    .section .isr_vector, "a", %progbits
    .word 0x20020000     /* initial stack pointer (adjust to your MCU SRAM size) */
    .word Reset_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler

    .type Reset_Handler, %function
Reset_Handler:
    /* Set up stack, call SystemInit and main */
    bl SystemInit
    bl main
hang:
    b hang

Default_Handler:
    b Default_Handler
