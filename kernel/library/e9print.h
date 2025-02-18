#pragma once
#include <stdarg.h>
#include <display.h>

void write_port (uint16_t  port, uint8_t  val)
{
 /*asm volatile ("out %1, %0" : : "a"(val), "Nd"(port));*/
	 asm volatile ("out %0, %1" : : "a"(val), "Nd"(port));
}

char read_port (uint16_t  port)
{
	uint8_t ret;
    /*asm volatile ("in %0, %1"
                   : "=a"(ret)
                   : "Nd"(port));*/
	asm volatile ("in %1, %0"
                   : "=a"(ret)
                   : "Nd"(port));
    return ret;
}

uint32_t read_portl(uint16_t portid)
{
	uint32_t ret;
	/*asm volatile("in eax, dx"
				:"=a"(ret)
				:"Nd"(portid));*/
				
	asm volatile("in %%dx, %%eax"
				:"=a"(ret)
				:"Nd"(portid));
	return ret;
}
void write_portl(uint16_t portid, uint32_t value)
{
	/*asm volatile("out dx, eax": :"d" (portid), "a" (value));*/
	asm volatile("out %%eax, %%dx": :"d" (portid), "a" (value));
}

void e9_putchar(char * arg)
{
	write_port(0xE9 , arg);
}

void e9_putc(char c) {
    e9_putchar(c);
	processchars(c);    
}

void e9_print(const char *msg) {
    for (size_t i = 0; msg[i]; i++) {
        e9_putc(msg[i]);
    }
}

void e9_puts(const char *msg) 
{
    e9_print(msg);
    e9_putc('\n');
}

static const char CONVERSION_TABLE[] = "0123456789ABCDEF";
static void e9_printhex(size_t num, uint8_t prefix, int type) 
{
    int i = 0;
    char buf[17];

    buf[16] = 0;
	
	for(int x =0; x < 16; x++)
	{
		buf[x] = '0';
	}
	
	if (type == 1)
		e9_print("0x");

	int inc = 0;
	
    for (i = 15; num; i--) {
        buf[i] = (CONVERSION_TABLE[num % 16]);
        num /= 16;
		inc++;
    }

	int l = 16;
	for(int x =0; x < 16; x++)
	{
		if (l < prefix + 1)
		e9_putc(buf[x]);

		l -= 1;
	}
}

static void e9_printdec(size_t num) 
{
    int i;
    char buf[21] = {0};

    if (!num) {
        e9_putc('0');
        return;
    }

    for (i = 19; num; i--) {
        buf[i] = (num % 10) + 0x30;
        num = num / 10;
    }

    i++;
    e9_print(buf + i);
}

void e9_printf(const char *format, ...) 
{
	
	
    va_list argp;
    va_start(argp, format);

    while (*format != '\0') {
        if (*format == '%') {
            format++;
			if (*format == '0') {
				 format++;
				 if (*format != 0){
					  uint8_t size = (int)((char)*format - '0');
					  format++;
					  if (*format == 'x'){
					   e9_printhex(va_arg(argp, size_t), size, 1);
					  }
					  else if (*format == 'X'){
					   e9_printhex(va_arg(argp, size_t), size, 0);
					  }
				 }
			}
			else if (*format == 'x') {
		
                e9_printhex(va_arg(argp, size_t), 16, 1);
            } 
			
			else if (*format == 'd') {
                e9_printdec(va_arg(argp, size_t));
            } else if (*format == 's') {
                e9_print(va_arg(argp, char*));
				
            } else if (*format == 'c') {
                e9_putc(va_arg(argp, size_t));	
            } 
        } else {
            e9_putc(*format);	
        }
        format++;
    }

    va_end(argp);
}

