#include "kernel.h"

#include "klib/char.h"

#include "kernel/memory.h"
#include "kernel/display.h"
#include "kernel/pic.h"
#include "kernel/idt.h"
#include "e9print.h"


#include "drivers/keyboard.h"

//#include "libc/include/string.h"


void _start(struct stivale_struct  *info) 
{
	/* init pic/idt */
	//initialize_pic();
	//initialize_idt_pointer();
	//load_idt(&idt_ptr);
	
	//load_idt_entry(0x21, (unsigned long) keyboard_handler_int, 0x08, 0x8e);
	//kb_init();
	
	/* init memory */
	//initMemory();

 //char* video  = (char*)0xb8000;
 
	//SetVideo(&info->framebuffer_addr, info->framebuffer_pitch, info->framebuffer_bpp);

		e9_printf("info itself %x ", info);
		e9_printf("info itself & %x ", &info);
 e9_printf("size of uint16_t %x ", sizeof(uint16_t));
 e9_printf("size of uint32_t %x ", sizeof(uint32_t));
 e9_printf("size of uint64_t %x ", sizeof(uint64_t));
  e9_printf("Framebuffer at %x with specifics:", info->framebuffer_addr);
    e9_printf("\tPitch:  %d", info->framebuffer_pitch);
    e9_printf("\tWidth:  %d", info->framebuffer_width);
    e9_printf("\tHeight: %d", info->framebuffer_height);
    e9_printf("\tBPP:    %d", info->framebuffer_bpp);
    if (info->flags & (1 << 1)) {
        e9_printf("\tExtended colour information passed:");
        e9_printf("\t\tMemory model:     %d", info->fb_memory_model);
        e9_printf("\t\tRed mask size:    %d", info->fb_red_mask_size);
        e9_printf("\t\tRed mask shift:   %d", info->fb_red_mask_shift);
        e9_printf("\t\tGreen mask size:  %d", info->fb_green_mask_size);
        e9_printf("\t\tGreen mask shift: %d", info->fb_green_mask_shift);
        e9_printf("\t\tBlue mask size:   %d", info->fb_blue_mask_size);
        e9_printf("\t\tBlue mask shift:  %d", info->fb_blue_mask_shift);
    }
//printf("hello", 300,0, RED);
//printhex(&info->framebuffer_height,1,1,1);

for(int x = 0; x < 600; x++)
	for(int y = 0; y < 800; y++)
	{
	uint32_t* vesaaddr = (uint32_t*)(info->framebuffer_addr)  + (x  * 800 )+ y ;	
	//uint32_t * location = &info->framebuffer_addr + ((x * 800) + y);
	*vesaaddr = 0xFF0000;
		//putpixel(vesaaddr, x, y ,255,255,0);
	}

//char character = 'A';
 
	/* init display */
	//initVga(WHITE, BLACK);
		
   // printf("Stivale information passed to the kernel:", 0,0,RED);	
    //printf("Cmdline: %s", (char*)info->cmdline);
   // printf(info->memory_map_addr);

	
	/* malloc test */
/*	char* blaha = malloc(1);
	char* blahb = malloc(221);
	char* blahc = malloc(1);
	char* blahd = malloc(1);
	
	blaha[0] = '1';
	blahb[0] = '2';
	blahc[0] = '3';
	blahd[0] = '4';
	memset(blahd, '1', 1);
	
	/* let's measure the page memory and print it to the screen 
	int *w = &first_page_table[MAXMEMORY];
	int *x = &first_page_table[0];
	int differenceInBytes = (w-x);//* sizeof(int);

	int mb = differenceInBytes;
	int len = printf("Page Mapped: ", 10,10,RED);	
	printsize(mb* sizeof(int), 0, len,RED, MBYTE);

	int offset= 200;
	printhex(&first_page_table[0],0,offset+1, RED);
	printhex(&first_page_table[MAXMEMORY],0,offset+12, RED);

	printsize(differenceInBytes* sizeof(int),52,250,DARK_GREY, BYTE);
	vga_putchar('(',0,offset,RED); 
	vga_putchar('-',0,offset+11,RED); 
	vga_putchar(')',0,offset+22,RED); 

	printhex(blaha,2,2, BLUE);
	printhex(blahb,3,2, BLUE);
	printhex(blahc,4,2, BLUE);
	printint(((blahd - blahc)),4,20,DARK_GREY);
	printhex(blahd,5,2, BLUE);

	vga_putchar((char)blaha[0], 0,20,DARK_GREY);	
	vga_putchar((char)blahb[0], 0,40,DARK_GREY);
	vga_putchar((char)blahc[0], 0,60,DARK_GREY);
	vga_putchar((char)blahd[0], 0,80,DARK_GREY);

	*/
	while(1)
	{
	asm volatile("hlt");
	}
}
