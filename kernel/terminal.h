#pragma once

typedef void (*terminal_putc_t)(char data);
typedef void (*terminal_puts_t)(char *data);
typedef void (*terminal_printf_t)(const char* format, ...);

typedef struct {
    terminal_putc_t terminal_putc;
    terminal_puts_t terminal_puts;
    terminal_printf_t terminal_printf;
} terminal_t;