.section .multiboot, "aw"
    .align 4
    .long 0x1BADB002
    .long (1<<0 | 1<<1 | 1<<2)
    .long -(0x1BADB002 + (1<<0 | 1<<1 | 1<<2))
    .long 0x00000000  /* header_addr */
    .long 0x00000000  /* load_addr */
    .long 0x00000000  /* load_end_addr */
    .long 0x00000000  /* bss_end_addr */
    .long 0x00000000  /* entry_addr */
    /* Graphics requests */
    .long 0x00000000  /* 0 = linear graphics */
    .long 1024
    .long 768
    .long 32

.section .bss, "aw"
    .align 16
stack_bottom:
    .space 16384  /* 16 KiB */
stack_top:

.global _start
.extern kmain

.section .text

_start:
    movl $stack_top, %esp

    /* push multiboot parameter to kmain() */
    pushl %ebx

    /* ...and run! */
    cli
    call main

    /* never reach here */
    cli
    hlt
