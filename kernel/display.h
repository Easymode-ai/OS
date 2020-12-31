#pragma once

#define SCREENWIDTH 80
#define VGA_ADDRESS 0xB8000
#include "../kernel/font.h"
/*
16 bit video buffer elements(register ax)
8 bits(ah) higher : 
lower 4 bits - forec olor
higher 4 bits - back color

8 bits(al) lower :
8 bits : ASCII character to print
*/
/*
 uint64_t * videoptr = 0;
 uint16_t videopitch = 0;
 uint16_t videobpp =0;
 
 static SetVideo(uint32_t * ptr, uint32_t pitch, uint32_t  bpp)
 {
	 

	
	 videoptr=ptr;
	 videopitch=pitch;
	 videobpp=bpp;
 }
 */
 /*
static void putpixel(uint32_t * screen, int x,int y, int r, int g, int b) {
	
	//unsigned uint32_t * index = (unsigned uint32_t*)videoptr + x + 800 * y;//((uint32_t)videopitch  / ((uint32_t)videobpp/8)) * y;
  // uint32_t  *  j = (x + y)  + videoptr;
   //uint32_t * pix = (uint32_t*)(j  );
	
	int * location = videoptr + (((x * 800) + y) /2);
	*location= 0x0000FFFF;
	
	//memset(&videoptr[x * y], 0xFF0000, 1);
   //*j = 0xffffff;
}*/

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

void graphics_char(char character, int x, int y, char color)
{
	int r,g,b =0;
	int q =0;
	int j =0;
	//character = 'X';
for (int z = 0; z < 8; z++)
 {
	
	 q++;
	for (int h = 8; h > 0; h--)
	{
		j++;
	char retc =(((font[(int)character][z] << h) & 0x80) ?  1 : 0 );
	
	if (retc == 1)
	{
		r=255;
		g=0;
		b=0;
	}
	else
	{
		r=0;
		g=0;
		b=0;
	}
		
	//putpixel(videoptr, j +x +5, q + y + 5 ,r,g,b);
	}
	j=0;
 }
}

void vga_putchar( char arg, int x,  int y, char color)
{
	//vga_buffer[x*SCREENWIDTH+y]= vga_entry((char)arg, WHITE,  color);	
	graphics_char((char)arg, x,y,color);

}
