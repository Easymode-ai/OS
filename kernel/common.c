/**
 * common.c	Defines some global functions.
 * @version $Id$
 */

#include "common.h"

// Write a byte out to the specified port.
void outb(u16int port, u8int value)
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

// get byte from port
u8int inb(u16int port)
{
	u8int ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

// get word from port
u16int inw(u16int port)
{
	u16int ret;
	asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

// Copy len bytes from src to dest.
void memcpy(u8int *dest, const u8int *src, u32int len)
{
	const u8int *sp = (const u8int *)src;
	u8int *dp = (u8int *)dest;
	for ( ; len != 0; len-- )
		*dp++ = *sp++;
}

// Write len copies of val  into dest.
void *memset(void *dest, int value, size_t count)
{
   uint8_t val = (uint8_t)(value & 0xFF);
   uint8_t *dest2 = (uint8_t*)(dest);

   size_t i = 0;

   while(i < count)
   {
      dest2[i] = val;
      i++;
   }

   return dest;
}

// Compare two strings. Should return -1 if
// str1 < str2, 0 if they equal or 1 otherwise.
int strcmp(char *str1, char *str2)
{
	int i = 0;
	int failed = 0;
	while (str1[i] != '\0' && str2[i] != '\0') {
		if (str1[i] != str2[i]) {
			failed = 1;
			break;
		}
		i++;
	}
	// why did the loop exit ?
	if ( (str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0') )
		failed = 1;

	return failed;
}

// Copy the null-terminated string src into dest, and
// return dest.
char *strcpy(char *dest, const char *src)
{
	do {
		*dest++ = *src++;
	} while (*src != 0);
}

// Concatenate the null-terminated string src onto
// the end of dest, and return dest.
char *strcat(char *dest, const char *src)
{
	while (*dest != 0)
		*dest = *dest++;

	do
		*dest++ = *src++;
	while (*src != 0);

	return dest;
}

int strlen(char *src)
{
	int i = 0;
	while (*src++)
		i++;
	return i;
}

extern void panic(const char *message, const char *file, u32int line)
{
	// We encountered a massive problem and have to stop.
	asm volatile("cli");	// Disable interrupts.

/*	monitor_write("PANIC(");
	monitor_write(message);
	monitor_write(") at ");
	monitor_write(file);
	monitor_write(":");
	monitor_write_dec(line);
	monitor_write("\n");*/
	// Halt by going into an infinite loop.
	for ( ; ; );
}

extern void panic_assert(const char *file, u32int line, const char *desc)
{
	// An assertion failed, and we have to panic.
	asm volatile("cli");	// Disable interrupts.

	/*monitor_write("ASSERTION-FAILED(");
	monitor_write(desc);
	monitor_write(") at ");
	monitor_write(file);
	monitor_write(":");
	monitor_write_dec(line);
	monitor_write("\n");*/
	// Halt by going into an infinite loop.
	for ( ; ; );
}
