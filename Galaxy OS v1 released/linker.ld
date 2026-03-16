ENTRY(_start)
SECTIONS {
    . = 1M;
    .text : {
        *(.multiboot)    /* Ensure the header is first! */
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}