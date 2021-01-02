#include "kernel.h"

#include "kernel/display.h"
#include "kernel/heap.h"
#include "kernel/paging.h"
#include "kernel/pic.h"
#include "kernel/idt.h"
#include "kernel/debug.h"
#include "klib/char.h"
#include "klib/e9print.h"

#include "drivers/keyboard.h"
extern u32int placement_address;
extern u32int end;

int debug = 0;


uint32_t bytes(uint32_t * a, uint32_t  * b)
{
	 return ((b-a)) / 1024 * sizeof(uint32_t);
}
void _start(struct stivale_struct  *info) 
{
	
	/* init pic/idt */
	init_descriptor_tables();

	/* pass video address, pitch, bpp to display driver */
	SetVideo(info->framebuffer_addr,info->framebuffer_pitch, info->framebuffer_bpp);
	
	/* Print bootloader info */
	if (debug)
	DebugStivale(info);

	/* set placement address to end from linker.ld */
	placement_address = &end;
	
	/*init paging/heap */
	initialise_paging(info);
	
	videoptr = info->framebuffer_addr; 
	
	color(0xFFFF000);
	e9_printf("[Operating System]\n");
	color(0xFF0FF0);
	e9_printf("Graphics Mode %dx%d@%dbpp", info->framebuffer_width, info->framebuffer_height, info->framebuffer_bpp);
	
	color(0xFFFFFF);
	e9_printf("Allocated Virtual Memory (%x-%x) = %d KBytes", 0x00000000, placement_address-0x1000, bytes(0x0000000,placement_address-0x1000));
	color(0xFFFFFF);
	e9_printf("Allocated Virtual Heap Memory (%x-%x) = %d KBytes", KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, bytes(KHEAP_START,  KHEAP_START+KHEAP_INITIAL_SIZE));
	color(0x00FFFF);
	uint64_t ernd = info->framebuffer_addr + (info->framebuffer_width * info->framebuffer_height * (info->framebuffer_bpp/8)) + 1024;
	e9_printf("Framebuffer Start: %x", info->framebuffer_addr); 
	e9_printf("Framebuffer End: %x", ernd);
	e9_printf("Framebuffer Size: %d KBytes", bytes(info->framebuffer_addr , ernd));
	
	
	

	
	while(1)
	{
		asm("hlt");
	}
}
