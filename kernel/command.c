#include "command.h"
#include <stdint.h>
#include <stddef.h>
#include "pmm.h"
#include "vmm.h"
#include "display.h"
#include "fs/echfs.h"
#include "ahci.h"
#include "process.h"


void cmd(void (*ptr)())
{
	fontcolor(0xFFFFFF);
	(*ptr) ();
	fontcolor(0xDDDDDD);
}


void loadbg(char const * filename)
{
	
		  uint16_t hint = 0;

		
	///	  e9_printf("Reading disk\n");
		disk_read(0, &hint, 444, sizeof(uint16_t));
		
		e9_printf("hint value %x\n", hint);
		     if (hint && hint != 0x5a5a)
			 {
				 e9_printf("invalid table\n");
				 
				 	struct gpt_table_header header = {0};
					disk_read(0, &header, (void *)(4096 * 1), sizeof(header));
					if (strncmp(header.signature, "EFI PART", 8))
					e9_printf("invalid table\n");
						else
					e9_printf("Valid table\n");
				
					if (header.revision != 0x00010000)
					e9_printf("end of table\n");
			 }else
			 {
				 e9_printf("valid table\n");
			 
			 	e9_printf("read file\n");
	struct echfs_file_handle * fd = malloc(sizeof(struct echfs_file_handle));
			 	e9_printf("readd file\n");

	struct part extended_part;
	
	int rr = echfs_open(fd, &extended_part,filename);

	if (rr)
	e9_printf("[AHCI] Could not find file: %s\n", filename);
	else{
		
//e9_printf("WOO\n");

		struct file_handle *ret = malloc(sizeof(struct file_handle));
		
		ret->fd   = (void *)fd;
		ret->read = (void *)echfs_read;
		ret->size = fd->dir_entry.size;
		
		if (ret->size > 0)
		{

		
		
		struct  tagBITMAPFILEHEADER * bitmapdata = malloc(sizeof( struct tagBITMAPFILEHEADER));
		memcpy(bitmapdata, fd->alloc_map , sizeof(struct tagBITMAPFILEHEADER));
		
		struct  tagBITMAPINFOHEADER * bitmapheader = malloc(sizeof( struct tagBITMAPINFOHEADER));
		memcpy(bitmapheader, fd->alloc_map + sizeof(struct tagBITMAPFILEHEADER), sizeof(struct tagBITMAPINFOHEADER));

		int offset=bitmapdata->bfOffBits-3;
		

		for(int y =bitmapheader->biHeight; y > 0; y--)
		{	
			for(int x = 0; x < bitmapheader->biWidth; x++)
			{				
				uint32_t where =  x + (y * videoheader->framebuffer_pitch/4)- videoheader->framebuffer_pitch/4;
				
				int b = (int)fd->alloc_map [offset];
				int g = (int)fd->alloc_map [offset+1];
				int r = (int)fd->alloc_map [offset+2];
				int a = (int)fd->alloc_map [offset+3];
				
				uint32_t color = ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8)
				+ (a & 0xff);
				
				uint32_t * screen = videoheader->framebuffer_addr;	
				screen[where] = color;
				
				offset += 4;
			}
		}
			}
		else
			e9_printf("[AHCI] File empty or not found\n");
		
			}
	}

}
