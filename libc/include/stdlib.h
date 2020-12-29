#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

void abort(void);
void itoa(int n, char str[]);
void __itoa(int i, unsigned int base, char buf[]);

#endif