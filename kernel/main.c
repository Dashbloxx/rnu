#include <stdint.h>

#define RELEASE "1.0"
#define VERSION "1.0"

#include "terminal.h"

terminal_t *current_terminal;

extern void gdt_initialize();
extern void terminal_initialize();

void kernel_main()
{
    /* Initialize terminal & print title, version, and copyright info... */
    terminal_initialize();
    current_terminal->terminal_printf("Radeon/386 Release %s Version %s\nCopyright (c) 2023 Matthew Majfud-Wilinski\nAll rights reserved\n\n", RELEASE, VERSION);
    
    /* Initialize the GDT... */
    gdt_initialize();
}