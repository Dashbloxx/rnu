#include "terminal.h"

extern void cga_clear();
extern void cga_putc(char character);
extern void cga_puts(char *string);
extern void cga_printf(const char* format, ...);

extern terminal_t *current_terminal;

terminal_t terminals[32];

void terminal_initialize()
{
    cga_clear();

    terminals[0].terminal_putc = &cga_putc;
    terminals[0].terminal_puts = &cga_puts;
    terminals[0].terminal_printf = &cga_printf;
    
    current_terminal = &terminals[0];
}