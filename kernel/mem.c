#include <stdint.h>
#include <stddef.h>

void *memset(void *s, int c, size_t l)
{
    uint8_t* p = s;
    while(l--)
    {
        *p++ = (uint8_t)c;
    }
    return s;
}