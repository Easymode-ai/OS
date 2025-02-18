#include <stivale2.h>
#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "common.h"
#include "bootlogo.h"
#include "kernel/pmm.h"
#include "kernel/vmm.h"
#include "kernel/idt.h"
#include "kernel/display.h"
#include "kernel/library/e9print.h"
#include "kernel/ahci.h"
#include "kernel/pci.h"
#include "kernel/fs/echfs.h"
#include "gdt.h"
#include "kernel/syscall.h"
uint64_t totalmemory = 0;

void stivale2_main(struct stivale2_struct *info) {


	init_descriptor_tables();
	irq_install();

	uintptr_t  cpuaddress = 0;

	// Print stivale tags
	struct stivale2_tag *tag = (struct stivale2_tag *)info->tags;

	e9_puts("Stivale2 info passed to the kernel:\n");
	e9_printf("Bootloader brand:   %s\n", info->bootloader_brand);
	e9_printf("Bootloader version: %s\n", info->bootloader_version);

	while (tag != NULL) {
		switch (tag->identifier) {
			case STIVALE2_STRUCT_TAG_CMDLINE_ID: {
								     struct stivale2_struct_tag_cmdline *c = (struct stivale2_struct_tag_cmdline *)tag;
								     e9_puts("Commandline tag:\n");
								     e9_printf("\tCmdline: %s", (char*)(c->cmdline));
								     break;
							     }
			case STIVALE2_STRUCT_TAG_MEMMAP_ID: {
								    struct stivale2_struct_tag_memmap *m = (struct stivale2_struct_tag_memmap *)tag;
								    e9_puts("Memmap tag:\n");
								    e9_printf("\tEntries: %d\n", m->entries);
								    for (size_t i = 0; i < m->entries; i++) {
									    struct stivale2_mmap_entry me = m->memmap[i];
									    e9_printf("\t\t[%x+%x] %x\n", me.base, me.length, me.type);

									    if (me.type == 0x1)
									    {
										    totalmemory =  me.length;
										    //	InsertMap(me);

									    }
								    }
								    break;
							    }
			case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID: {
									 videoheader = (struct stivale2_struct_tag_framebuffer *)tag;
									 e9_puts("Framebuffer tag:");
									 e9_printf("\tAddress: %x\n", videoheader->framebuffer_addr);
									 e9_printf("\tWidth:   %d\n", videoheader->framebuffer_width);
									 e9_printf("\tHeight:  %d\n", videoheader->framebuffer_height);
									 e9_printf("\tPitch:   %d\n", videoheader->framebuffer_pitch);
									 e9_printf("\tBPP:     %d\n", videoheader->framebuffer_bpp);			
									 break;
								 }
			case STIVALE2_STRUCT_TAG_MODULES_ID: {
								     struct stivale2_struct_tag_modules *m = (struct stivale2_struct_tag_modules *)tag;
								     e9_puts("Modules tag:");
								     e9_printf("\tCount: %d", m->module_count);
								     for (size_t i = 0; i < m->module_count; i++) {
									     struct stivale2_module me = m->modules[i];
									     e9_printf("\t\t[%x+%x] %s", me.begin, me.end, me.string);
								     }
								     break;
							     }
			case STIVALE2_STRUCT_TAG_RSDP_ID: {
								  struct stivale2_struct_tag_rsdp *r = (struct stivale2_struct_tag_rsdp *)tag;
								  e9_puts("RSDP tag:");
								  e9_printf("\tRSDP: %x", r->rsdp);
								  break;
							  }
			case STIVALE2_STRUCT_TAG_EPOCH_ID: {
								   struct stivale2_struct_tag_epoch *e = (struct stivale2_struct_tag_epoch *)tag;
								   e9_puts("Epoch tag:");
								   e9_printf("\tEpoch: %x", e->epoch);
								   break;
							   }
			case STIVALE2_STRUCT_TAG_FIRMWARE_ID: {
								      struct stivale2_struct_tag_firmware *f = (struct stivale2_struct_tag_firmware *)tag;
								      e9_puts("Firmware tag:");
								      e9_printf("\tFlags: %x", f->flags);
								      break;
							      }
			case STIVALE2_STRUCT_TAG_SMP_ID: {
								 struct stivale2_struct_tag_smp *s = (struct stivale2_struct_tag_smp *)tag;
								 e9_puts("SMP tag:");
								 e9_printf("\tFlags:        %x", s->flags);

								 e9_printf("\tCPU Count:    %d", s->cpu_count);
								 for (size_t i = 0; i < s->cpu_count; i++) {
									 struct stivale2_smp_info *in = &s->smp_info[i];
									 e9_printf("\t\tProcessor ID:   %d", in->processor_id);
									 e9_printf("\t\tLAPIC ID:       %d", in->lapic_id);
									 e9_printf("\t\tTarget Stack:   %x", in->target_stack);
									 e9_printf("\t\tGOTO Address:   %x", in->goto_address);
									 e9_printf("\t\tExtra Argument: %x", in->extra_argument);
									 cpuaddress = in->extra_argument;

								 }
								 break;
							 }
			default:
							 e9_printf("BUG: Unidentifier tag %x", tag->identifier);
		}

		tag = (struct stivale2_tag *)tag->next;
	}

	e9_printf("Total memory %x\n", totalmemory);
	
	initpmm(totalmemory);
	initvmm(totalmemory);
	e9_printf("hi\n");
	// Initialise the PAT
   uint64_t pat_msr = rdmsr(0x277);
    pat_msr &= 0xffffffff;
    // write-protect / write-combining
   pat_msr |= (uint64_t)0x0105 << 32;
    wrmsr(0x277, pat_msr);

    // Enable syscall in EFER
   uint64_t efer = rdmsr(0xc0000080);
    efer |= 1;
    wrmsr(0xc0000080, efer);
	// Set up syscall
    wrmsr(0xc0000081, 0x0013000800000000);
    // Syscall entry address
	
	extern syscall_entry();
   wrmsr(0xc0000082, (uint64_t)syscall_entry);
    // Flags mask
   wrmsr(0xc0000084, (uint64_t)~((uint32_t)0x002));

	
	//syscall_debug_log();
	e9_printf("Usermode:\n");
	//syscall(0,0,0,0);


   // wrmsr(0xc0000101, &stack_space);
	//wrmsr(0xc0000102, &stack_space);
	//wrmsr(0xc0000100, 0);
	usermode();
	syscall(0,0,0,0);
	asm volatile ("out %0, %1" : : "a"('H'), "Nd"(0xe9));
	
	//debug_log();
	//asm("jmp .");
	
	
	//usermodetest();
	
	e9_printf("bob\n");
	int ret;
	int errno;

	uint64_t stack = 0;
	__asm__ volatile ("mov %%rsp, %0" : "=r"(stack));
	
	e9_printf("Stack is %x\n", stack);
	asm volatile ("syscall"                  
                  : "=a"(ret), "=d"(errno)   
                  : "a"(0)                 
                  : "rcx", "r11", "memory"); 
			  
  
	//e9_printf("DONE YAY ! %d %d \n", ret, errno);
	
	//asm("jmp .");
	
	e9_printf("back\n");
	//asm("jmp .");


	e9_printf("hi\n");
	charoffset=10;
	lineoffset = videoheader->framebuffer_height-20;
	
	e9_printf("BO1\n");
	uint64_t sizefb = videoheader->framebuffer_addr + (videoheader->framebuffer_width * videoheader->framebuffer_height * (videoheader->framebuffer_bpp/8));
	//uint64_t * vbuffer = malloc(sizefb);
//	e9_printf("%x fb address\n", vbuffer);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0x00000000FD000000,(uint64_t)0x00000000FD000000, (sizefb)/PAGE_SIZE, 1,0);
	

	
	//videoheader->framebuffer_addr = (uint64_t)0xFD000000;
//0x3FFFFFB50
//0xFD000000
	//print_table(kernel_p4,PAGE_SIZE);
	write_cr3((uint64_t)kernel_p4);
	//asm("jmp .");
		e9_printf("BOB\n");
	/* Enable Video */
	EnableVideo();
	
				

	/* Set cursor position */
	charoffset=10;
	lineoffset=10;

	/* Welcome message */
	fontcolor(0xAAAAAA);

	//e9_printf("12345\n", GetTick());

	
	fontcolor(0xFFFFFF);
	
	/* Draw a logo to the framebuffer */
	//uint32_t r,q =0;
		
	/*for(uint32_t y =0; y< 300; y++)
	{
		for(uint32_t x =0; x < 300; x++)
		{
			uint32_t * screen = videoheader->framebuffer_addr;
			uint32_t where =( x + videoheader->framebuffer_width-300) + ((y + 0)*videoheader->framebuffer_pitch/4);
			//uint32_t s = (r * 300) * q;

			if(bootlogo[y*300+x] == 0x00ffffff)
				screen[where] = 0x00;
			else
				screen[where] = 0x2222222;	
				
				
		}
		//r = 0;
		//q +=1;
	}*/

//asm("jmp .");
	/* List PCI */
	pci_init();


	e9_printf("[%d] [AHCI] Init\n");
	/* AHCI Drive */
	ahci_device_init(satadrive);

	/* fs */
	
		
			 
		e9_printf("[ECHFS] read file\n");
		
	 

	
	Process("hello.elf");
	
		
		//loadbg("wallpaper/liquid.bmp");

	while(1)
		asm volatile ("hlt");
}
