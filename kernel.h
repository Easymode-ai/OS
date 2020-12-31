#pragma once

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint32_t;
typedef unsigned int size_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t;



	
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


static uint8_t stack[22097152] = {0};

struct stivale_struct {
    uint64_t cmdline;
    uint64_t memory_map_addr;
    uint64_t memory_map_entries;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t rsdp;
    uint64_t module_count;
    uint64_t modules;
    uint64_t epoch;
    uint64_t flags; // bit 0: 1 if booted with BIOS, 0 if booted with UEFI
                    // bit 1: 1 if extended colour information passed, 0 if not
    uint8_t  fb_memory_model;
    uint8_t  fb_red_mask_size;
    uint8_t  fb_red_mask_shift;
    uint8_t  fb_green_mask_size;
    uint8_t  fb_green_mask_shift;
    uint8_t  fb_blue_mask_size;
    uint8_t  fb_blue_mask_shift;
} __attribute__((packed));

struct stivale_header {
    uint64_t stack;
    uint16_t flags;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t entry_point;
} __attribute__((packed));



__attribute__((section(".stivalehdr"), used))
struct stivale_header header = {
    .stack = sizeof(stack),
    .framebuffer_bpp = 32,
    .framebuffer_width = 800,
    .framebuffer_height = 600,
    .flags = 1,
    .entry_point = 0
};


#define IDT_SIZE 256
