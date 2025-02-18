#include <stdint.h>
#include <stddef.h>

#include <kernel.h>
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

int main(){
	
	EnableVideo();
	
	videoheader = malloc(sizeof(struct stivale2_struct_tag_framebuffer));
	videoheader->framebuffer_addr = 0x1337000000;
	videoheader->framebuffer_width = 1280;
	videoheader->framebuffer_height = 720;
	videoheader->framebuffer_pitch = 5120;
	videoheader->framebuffer_bpp = 32;

	e9_printf("\n\nHello world from hello.elf\n");

	return 1;
}

void stivale2_main(struct stivale2_struct *info){}
