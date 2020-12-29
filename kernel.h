#pragma once

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint32_t;
typedef unsigned int size_t;


extern void loadpage(unsigned int*);
extern void enablePaging();

extern unsigned char read_port (int port);
extern void write_port (int port, unsigned char val);
extern void kb_init(void);
extern unsigned char _key();

extern void keyboard_handler_int();
extern void load_idt(void *);


uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

#define MAXMEMORY 1024
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

#define IDT_SIZE 256
