#include <stdint.h>

#define RELEASE "1.0"
#define VERSION "1.0"

#include "terminal.h"
#include "debug.h"
#include "multiboot.h"

terminal_t *current_terminal;
terminal_t terminals[32];

extern void terminal_initialize();

void main(multiboot_t *multiboot)
{
    /* Initialize terminal & print title, version, and copyright info... */
    terminal_initialize();
    current_terminal->terminal_printf("Radeon/386 Release %s Version %s\nCopyright (c) 2023 Matthew Majfud-Wilinski\nAll rights reserved\n\n", RELEASE, VERSION);

    DEBUG("Loaded by bootloader `%s`...\n", multiboot->boot_loader_name);
}