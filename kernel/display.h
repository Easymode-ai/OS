#pragma once

#define SCREENWIDTH 80
#define VGA_ADDRESS 0xB8000
#include "../kernel/font.h"

 uint64_t videoptr = 0;
 uint16_t videopitch = 0;
 uint16_t videobpp =0;
 
 static SetVideo(uint32_t ptr, uint32_t pitch, uint32_t  bpp)
 {
	 
	 videoptr=ptr;
	 videopitch=pitch;
	 videobpp=bpp;
 }
 
 
static void putpixel(uint32_t * screen, int x, int y, int color) {
	
	  unsigned where = x + (y*videopitch/4);
    screen[where] = color;
   // screen[where + 1] = g;
   // screen[where + 2] = b;
}

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

int colorc = 0;
void video_char(char character, int x, int y, char color)
{
	colorc++;
	int r,g,b =0;
	int q =0;
	int j =0;
	int colorr = 0x000000;
		switch(colorc)
	{
		case 1:
		colorr = 0xFF0000;
		break;
		case 2:
		colorr = 0x00FF00;
		break;
		case 3:
		colorr = 0xFFFFFF;
		break;
	}
	
		if (colorc > 2) colorc=0;
	
	//character = 'X';
for (int z = 0; z < 8; z++)
 {
	int color = 0;
	 q++;
	for (int h = 8; h >= 0; h--)
	{
		color = 0;
		
		j++;
	char retc =(((font[(int)character][z] << h) & 0x80) ?  1 : 0 );
	
	if (retc == 1)
	{
	
		color = colorr;
	}
	
		
	putpixel(videoptr, j +x, q + y  ,color);
	}
	j=0;
 }
}


static int video_printf( char * arg, int x,  int y, char color)
{
	int len =0;
	for(int i=0; arg[i]!='\0';++i){
		video_char((char)arg[i], x + (len *10) , y ,  color);
		e9_putchar(arg[i]);
		len++;
	}
	e9_putchar('\n');
	return len;
}