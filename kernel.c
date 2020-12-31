#include "kernel.h"

#include "klib/char.h"

#include "kernel/memory.h"
#include "kernel/display.h"
#include "kernel/pic.h"
#include "kernel/idt.h"


#include "drivers/keyboard.h"

//#include "libc/include/string.h"


void kernel_entry(struct stivale_struct *info) 
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



//printf("hello", 300,0, RED);

for(int x = 0; x < 800; x++)
	for(int y = 0; y < 600; y++)
	{
		
	uint32_t * location = &info->framebuffer_addr + ((x * 800) + y);
	*location= 0xFF0000;
		//putpixel(videoptr, rx, ry ,255,255,0);
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
