#pragma once

#include "kernel/common.h"
#include "kernel/stivale.h"


	
extern void loadpage(unsigned int*);
extern void enablePaging();

extern unsigned char read_port (int port);
extern void write_port (int port, unsigned char val);
extern void kb_init(void);
extern unsigned char _key();

extern void keyboard_handler_int();
extern void load_idt(void *);



#define BUFSIZE 2200

uint16* vga_buffer;

#define NULL 0

enum vga_color {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	GREY,
	DARK_GREY,
	BRIGHT_BLUE,
	BRIGHT_GREEN,
	BRIGHT_CYAN,
	BRIGHT_RED,
	BRIGHT_MAGENTA,
	YELLOW,
	WHITE,
};


//static uint8_t stack[222097152] = {0};


