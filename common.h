#pragma once
#include <stdint.h>
#include <stddef.h>
#include "kernel/pmm.h"

// Write a byte out to the specified port.
void outb(uint16_t port, uint8_t value)
{
	//asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

// get byte from port
uint8_t inb(uint16_t port)
{
	uint8_t ret;
	//asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

// get word from port
uint16_t inw(uint16_t port)
{
	uint16_t ret;
	//asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

// Copy len bytes from src to dest.
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
	const uint8_t *sp = (const uint8_t *)src;
	uint8_t *dp = (uint8_t *)dest;
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
int strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        char c1 = s1[i], c2 = s2[i];
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            return 0;
    }

    return 0;
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

extern void panic(const char *message, const char *file, uint32_t line)
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

extern void panic_assert(const char *file, uint32_t line, const char *desc)
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

size_t
strnlen (const char *s, size_t maxlen)
{
  size_t i;
  for (i = 0; i < maxlen; ++i)
    if (s[i] == '\0')
      break;
  return i;
}
char *
strncpy (char *s1, const char *s2, size_t n)
{
  size_t size = strnlen (s2, n);
  if (size != n)
    memset (s1 + size, '\0', n - size);
  memcpy (s1, s2, size);
  return 1;
}
size_t lfind(const char * str, const char accept) {
	size_t i = 0;
	while ( str[i] != accept) {
		i++;
	}
	return (size_t)(str) + i;
}

size_t strspn(const char * str, const char * accept) {
	const char * ptr = str;
	const char * acc;

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc == '\0') {
			break;
		}

		str++;
	}

	return str - ptr;
}

char * strpbrk(const char * str, const char * accept) {
	const char *acc = accept;

	if (!*str) {
		return NULL;
	}

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc) {
			break;
		}
		++str;
	}

	if (*acc == '\0') {
		return NULL;
	}

	return (char *)str;
}

char * strtok_r(char * str, const char * delim, char ** saveptr) {
	char * token;
	if (str == NULL) {
		str = *saveptr;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		*saveptr = str;
		return NULL;
	}
	token = str;
	str = strpbrk(token, delim);
	if (str == NULL) {
		*saveptr = (char *)lfind(token, '\0');
	} else {
		*str = '\0';
		*saveptr = str + 1;
	}
	return token;
}

char * strdup(const char * s)
{
  size_t len = 1+strlen(s);
  char *p = malloc(len);
  memcpy(p, s, len);
  return p;
}

int split (const char *txt, char delim, char ***tokens)
{
    int *tklen, *t, count = 1;
    char **arr, *p = (char *) txt;

    while (*p != '\0') if (*p++ == delim) count += 1;
    t = tklen = calloc (count, sizeof (int));
    for (p = (char *) txt; *p != '\0'; p++) *p == delim ? *t++ : (*t)++;
    *tokens = arr = malloc (count * sizeof (char *));
    t = tklen;
    p = *arr++ = calloc (*(t++) + 1, sizeof (char *));
    while (*txt != '\0')
    {
        if (*txt == delim)
        {
            p = *arr++ = calloc (*(t++) + 1, sizeof (char *));
            txt++;
        }
        else *p++ = *txt++;
    }
   
	free (tklen);
    return count;
}