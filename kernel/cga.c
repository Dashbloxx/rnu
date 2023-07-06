#include <stdint.h>
#include <stdarg.h>

extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);
extern uint16_t inw(uint16_t port);
extern void outw(uint16_t port, uint16_t value);
extern uint32_t inl(uint16_t port);
extern void outl(uint16_t port, uint32_t value);

uint16_t *video_memory = (uint16_t *)0xB8000;

struct
{
    uint8_t x, y;
} coordinates;

struct
{
    uint8_t foreground;
    uint8_t background;
} colors;

static void scroll()
{
    uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
    uint16_t blank = 0x20 | (attribute_byte << 8);

    if(coordinates.y >= 25)
    {
        int i;
        for (i = 0 * 80; i < 24 * 80; i++)
        {
            video_memory[i] = video_memory[i+80];
        }

        for (i = 24 * 80; i < 25 * 80; i++)
        {
            video_memory[i] = blank;
        }
        coordinates.y = 24;
    }
}

static void update_cursor()
{
    uint16_t cursor_location = coordinates.y * 80 + coordinates.x;
    outb(0x3D4, 14);
    outb(0x3D5, cursor_location >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursor_location);

    /*
     *  Here we got some potentially future code that makes the entire cursor a block instead of an underscore...
     *
     *  uint8_t attribute_byte = (colors.foreground << 4) | (colors.background & 0x0F);
     *  uint8_t current = inb(0x3D4) & 0xE0;
     *  outb(0x3D4, 0x0A);
     *  outb(0x3D5, current | (attribute_byte & 0x0F));
     */
}

void cga_clear()
{
    uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
    uint16_t blank = 0x20 | (attribute_byte << 8);

    int i;
    for (i = 0; i < 80*25; i++)
    {
        video_memory[i] = blank;
    }

    coordinates.x = 0;
    coordinates.y = 0;
}


void cga_putc(char character)
{
    colors.background = 0;
    colors.foreground = 15;

    uint8_t attribute_byte = (colors.background << 4) | (colors.foreground & 0x0F);

    uint16_t attribute = attribute_byte << 8;
    uint16_t *location;

    if (character == 0x08 && coordinates.x)
    {
        coordinates.x--;
    }
    else if (character == 0x09)
    {
        coordinates.x = (coordinates.x + 8) & ~(8 - 1);
    }
    else if (character == '\r')
    {
        coordinates.x = 0;
    }
    else if (character == '\n')
    {
        coordinates.x = 0;
        coordinates.y++;
    }
    else if(character >= ' ')
    {
        location = video_memory + (coordinates.y * 80 + coordinates.x);
        *location = character | attribute;
        coordinates.x++;
    }

    if (coordinates.x >= 80)
    {
        coordinates.x = 0;
        coordinates.y ++;
    }

    scroll();
    update_cursor();
}

void cga_puts(char *string)
{
    int i = 0;
    while (string[i])
        cga_putc(string[i++]);
}

void cga_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            
            switch (*format)
            {
                case 's':
                {
                    char* str = va_arg(args, char*);
                    cga_puts(str);
                    break;
                }
                case 'd':
                {
                    int num = va_arg(args, int);
                    if (num == 0)
                    {
                        cga_putc('0');
                        return;
                    }

                    int32_t acc = num;
                    char c[32];
                    int i = 0;
                    while (acc > 0)
                    {
                        c[i] = '0' + acc%10;
                        acc /= 10;
                        i++;
                    }
                    c[i] = 0;

                    char c2[32];
                    c2[i--] = 0;
                    int j = 0;
                    while(i >= 0)
                    {
                        c2[i--] = c[j++];
                    }
                    cga_puts(c2);
                    break;
                }
                case 'x':
                {
                    unsigned int num = va_arg(args, unsigned int);
                    
                    int32_t tmp;
                    cga_puts("0x");

                    char no_zeroes = 1;

                    int i;
                    for (i = 28; i > 0; i -= 4)
                    {
                        tmp = (num >> i) & 0xF;
                        if (tmp == 0 && no_zeroes != 0)
                        {
                            continue;
                        }
                    
                        if (tmp >= 0xA)
                        {
                            no_zeroes = 0;
                            cga_putc(tmp-0xA+'a' );
                        }
                        else
                        {
                            no_zeroes = 0;
                            cga_putc( tmp+'0' );
                        }
                    }
                
                    tmp = num & 0xF;
                    if (tmp >= 0xA)
                    {
                        cga_putc(tmp-0xA+'a');
                    }
                    else
                    {
                        cga_putc(tmp+'0');
                    }
                    break;
                }
                case 'c':
                {
                    int c = va_arg(args, int);
                    cga_putc((char)c);
                    break;
                }
                default:
                    cga_putc(*format);
                    break;
            }
        }
        else {
            cga_putc(*format);
        }
        
        format++;
    }

    va_end(args);
}