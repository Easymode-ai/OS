#pragma once

#include "font.h"
#include "paging.h"
#include "../drivers/keyboard/keyboard_map.h"



static SetVideo(uint32_t ptr, uint32_t pitch, uint32_t  bpp)
{
	videoptr=ptr;
	videopitch=pitch;
	videobpp=bpp;
}

static void putpixel(uint32_t * screen, int x, int y, int color)
{
	
	unsigned where = x + (y*videopitch/4);
	screen[where] = color;
	
}

int colorc = 0;
void video_char(char character, int x, int y, int color)
{
	int q, j = 0;
	
	for (int z = 0; z < 8; z++)
	{
		q++;
		for (int h = 8; h >= 0; h--)
		{
			
			
			j++;
			int ret =(((font[(int)character][z] << h) & 0x80) ?  color : 0x000000 );
			
			
				putpixel(videoptr, j +x, q + y, ret);
		
		}
		j=0;
	}
}


static int video_printf( char * arg, int x,  int y, int color)
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



int charoffset=10;
int lineoffset = 10;

int keycolor = 0xFFFFFF;
void processchars(char mappedchar)
{
	
	if (mappedchar == '\n')
		{
			lineoffset += 10;
			charoffset = 0;
		}
		
		
		if (mappedchar == '\b') 
		{ 
			charoffset -= 10;
			video_char((char)0, charoffset,lineoffset, 0x000000);
			//e9_putchar((char)keyboard_map[keycode]);

		}else{
			video_char((char)mappedchar, charoffset, lineoffset, keycolor);
			//e9_putchar((char)keyboard_map[keycode]);
			charoffset += 10;
		}
}
void processkeys(char keycode)
{
	

		if (keycode == 0x48)
		{
			keycolor = 0xFF0000;
		}
		if (keycode == 0x50)
		{
			keycolor = 0xFFFFFF;
		}
		/*if (keycode == 0x1c)
		{
			lineoffset += 10;
			charoffset = 0;
		}*/
	
		if(keycode >= 0 && keyboard_map[keycode]) 
			{
			processchars(keyboard_map[keycode]);
			}
	
	
}

void color (int c)
{
	keycolor=c;
}