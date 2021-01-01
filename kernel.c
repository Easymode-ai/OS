#include "kernel.h"
#include "kernel/memory.h"
#include "kernel/display.h"
#include "kernel/pic.h"
#include "kernel/idt.h"

#include "klib/char.h"
#include "klib/e9print.h"

#include "drivers/keyboard.h"

void _start(struct stivale_struct  *info) 
{

	/* init pic/idt */
	init_descriptor_tables();

	/* set video display */
	SetVideo(info->framebuffer_addr,info->framebuffer_pitch, info->framebuffer_bpp);

	initMemory();
	//e9_printf("Framebuffer at %x with specifics:", info->framebuffer_addr);
	//int bytes =  &first_page_table[MAXMEMORY]- &first_page_table[0] ;
		
	//e9_printf("memory address %x for page memroy ", &first_page_table[0]);
	//e9_printf("memory address %x for page memroy max ", &first_page_table[MAXMEMORY]);

	
	
	e9_printf("Framebuffer at %x with specifics:", info->framebuffer_addr);
	e9_printf("\tPitch:   %d", info->framebuffer_pitch);
	e9_printf("\tWidth:   %d", info->framebuffer_width);
	e9_printf("\tHeight:  %d", info->framebuffer_height);
	e9_printf("\tBPP:     %d", info->framebuffer_bpp);
	e9_printf("\tcmdline: %x", info->cmdline);
	//e9_printf("\tcmdline: %s", info->cmdline);
	/*
	if (info->flags & (1 << 1)) {
		e9_printf("\tExtended colour information passed:");
		e9_printf("\t\tMemory model:     %d", info->fb_memory_model);
		e9_printf("\t\tRed mask size:    %d", info->fb_red_mask_size);
		e9_printf("\t\tRed mask shift:   %d", info->fb_red_mask_shift);
		e9_printf("\t\tGreen mask size:  %d", info->fb_green_mask_size);
		e9_printf("\t\tGreen mask shift: %d", info->fb_green_mask_shift);
		e9_printf("\t\tBlue mask size:   %d", info->fb_blue_mask_size);
		e9_printf("\t\tBlue mask shift:  %d", info->fb_blue_mask_shift);
	}*/
	
	while(1)
	{
		asm("hlt");
	}
}
