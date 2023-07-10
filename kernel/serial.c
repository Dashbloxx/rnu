#include <stdint.h>
#include <stdarg.h>

#define COM1 0x3F8

extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);

int serial_initialize()
{
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1 + 0, 0xAE);

    if(inb(COM1 + 0) != 0xAE)
        return -1;
 
    outb(COM1 + 4, 0x0F);
    return 0;
}

static int is_transmit_empty()
{
    return inb(COM1 + 5) & 0x20;
}

void serial_putc(char character)
{
    while (is_transmit_empty() == 0);

    if(character == '\n')
    {
        outb(COM1, '\r');
        outb(COM1, '\n');
        
        return;
    }

    outb(COM1, character);
}

void serial_puts(char *string)
{
    int i = 0;
    while (string[i])
        serial_putc(string[i++]);
}

void serial_printf(const char* format, ...)
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
                    serial_puts(str);
                    break;
                }
                case 'd':
                {
                    int num = va_arg(args, int);
                    if (num == 0)
                    {
                        serial_putc('0');
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
                    serial_puts(c2);
                    break;
                }
                case 'x':
                {
                    unsigned int num = va_arg(args, unsigned int);
                    
                    int32_t tmp;
                    serial_puts("0x");

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
                            serial_putc(tmp-0xA+'a' );
                        }
                        else
                        {
                            no_zeroes = 0;
                            serial_putc( tmp+'0' );
                        }
                    }
                
                    tmp = num & 0xF;
                    if (tmp >= 0xA)
                    {
                        serial_putc(tmp-0xA+'a');
                    }
                    else
                    {
                        serial_putc(tmp+'0');
                    }
                    break;
                }
                case 'c':
                {
                    int c = va_arg(args, int);
                    serial_putc((char)c);
                    break;
                }
                default:
                    serial_putc(*format);
                    break;
            }
        }
        else {
            serial_putc(*format);
        }
        
        format++;
    }

    va_end(args);
}