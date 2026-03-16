; Multiboot header constants
MB_MAGIC    equ 0x1BADB002
MB_FLAGS    equ 1 << 0 | 1 << 1
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .text
[BITS 32]
global _start
extern kernel_main

_start:
    mov esp, stack_top
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 4
stack_bottom:
    resb 16384
stack_top: