#include <stdint.h>

extern void cga_clear();
extern void cga_printf(const char* format, ...);

void kernel_main()
{
    cga_clear();
    cga_printf("Hello, world!\n");
}