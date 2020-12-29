#pragma once

#define SCREENWIDTH 80
#define VGA_ADDRESS 0xB8000

/*
16 bit video buffer elements(register ax)
8 bits(ah) higher : 
lower 4 bits - forec olor
higher 4 bits - back color

8 bits(al) lower :
8 bits : ASCII character to print
*/

static uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
	uint16 ax = 0;
	uint8 ah = 0, al = 0;

	ah = back_color;
	ah <<= 4;
	ah |= fore_color;
	ax = ah;
	ax <<= 8;
	al = ch;
	ax |= al;

	return ax;
}

//clear video buffer array
void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
	uint32 i;
	for(i = 0; i < BUFSIZE; i++){
		(*buffer)[i] = vga_entry(NULL, fore_color, back_color);
	}
}

//initialize vga buffer
void initVga(uint8 fore_color, uint8 back_color)
{
	vga_buffer = (uint16*)VGA_ADDRESS;  //point vga_buffer pointer to VGA_ADDRESS 
	clear_vga_buffer(&vga_buffer, fore_color, back_color);  //clear buffer
}

void vga_putchar( char * arg, int x,  int y, char color)
{
	vga_buffer[x*SCREENWIDTH+y]= vga_entry((char)arg, WHITE,  color);	

}
