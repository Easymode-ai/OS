#include "display.h"


void fontcolor (int c)
{
	keycolor=c;
}

static void CreateBackbuffer()
{
	size_t size = videoheader->framebuffer_pitch * videoheader->framebuffer_height * (videoheader->framebuffer_bpp/8);
	
	//bufferr = (uint32_t *)malloc(size);

}

static void PushLine(uint32_t * screen)
{
	
	//return;
	//if (buffer == 0x0) return;
		
	size_t size = videoheader->framebuffer_pitch * videoheader->framebuffer_height * (videoheader->framebuffer_bpp/8);

	for(int x =0; x < videoheader->framebuffer_width; x++)
		for(int y =0; y < videoheader->framebuffer_height;y++)
		{
			unsigned where = x + (y*videoheader->framebuffer_pitch/4);
			unsigned where_forward = x + ((y + 15 )*videoheader->framebuffer_pitch/4);
			
			screen[where] = screen[where_forward];
		}
	
		//memcpy(videoheader->framebuffer_addr,buffer, size);

}

static void putpixel(int x, int y, int color)
{

	if(!displayEnabled)
	return;

	uint32_t * screen = videoheader->framebuffer_addr;
	unsigned where = x + (y*videoheader->framebuffer_pitch/4);
	
	//if (color != 0x000000)
	screen[where] = color;
		
}


static void video_char(char character, int pos_x, int pos_y, int color)
{
		
	/*char *bitmap = font8x8_basic[character];

	int q, j = 0;
	int width = 8;
	int height = 8;
	
	for (int z = 0; z < width; z++)
	{
		q++;
		for (int h = 0; h < height; h++)
		{
			
			
			j++;
				int ret =((bitmap[z] & 1 << h) ?  color : 0x000000 );
			
			
				putpixel(videoheader->framebuffer_addr, j +x, q + y, ret);
		
		}
		j=0;
	}*/
	
	int q;
	int j;
	q = j = 0;
	int width = 6;
	int height = 13;

	for (int x = 0; x < width; x++)
	{
		q++;
		for (int y = 0; y < height; y++)
		{
		
			j++;
			uint64_t bitmap = fixed6x13_bin[y + ((uint64_t)character - 0x20) * height * ((width + 7)/8)];
			int shift = width - 1 - x;
			int pixel = ((bitmap >> shift) & 1) == 1;
			if (pixel != 0)
			{
			putpixel(q + pos_x, j + pos_y+1,  0x222222); 
			}
			
		}
		j = 0;
	}
	
	q = j = 0;
	
	for (int x = 0; x < width; x++)
	{
		q++;
		for (int y = 0; y < height; y++)
		{
		
			j++;
			uint64_t bitmap = fixed6x13_bin[y + ((uint64_t)character - 0x20) * height * ((width + 7)/8)];
			int shift = width - 1 - x;
			int pixel = ((bitmap >> shift) & 1) == 1;
			if (pixel != 0)
			{
			putpixel(q + pos_x+1, j + pos_y,  0x222222);
			putpixel(q + pos_x, j + pos_y, ((pixel ) ? color : 0x000000));
			}
			
		}
		j = 0;
	}

}



void ResetPosition()
{
	charoffset=10;
	lineoffset=10;
}
void processchars(char mappedchar)
{
	if(!displayEnabled)
		return;

	if (mappedchar == '\n')
	{
	
		charoffset = 10;
		
		if (lineoffset > videoheader->framebuffer_height-31 ) 
		{
		PushLine(videoheader->framebuffer_addr);
		}
		else
			lineoffset += 15;
		
		return;
	}
		
	if (mappedchar == '\b') 
	{ 
		charoffset -= 8;
		if (charoffset < 8)
			charoffset = 8;
		video_char((char)' ', charoffset,lineoffset, 0x000000);

	}else{
		video_char((char)mappedchar, charoffset, lineoffset, keycolor);
		charoffset += 8;
	}
	
}

void EnableVideo()
{
	displayEnabled = 1;
}

int cmdbufferoffset = 0;
void processkeys(char keycode)
{		
	fontcolor(0xFFFFFF);
	
	if (keycode == 0x48)
	{
		keycolor = 0xFF0000;
	}
	if (keycode == 0x50)
	{
		keycolor = 0xFFFFFF;
		
	}
	
	unsigned char mapchar = keyboard_map[keycode];
		
	if(keycode >= 0 && mapchar) 
	{
		//e9_putchar(mapchar);		
		
		if (mapchar == '\b')
		{
			cmdbufferoffset--;
			cmdbuffer[cmdbufferoffset]='\0';
			
			if (cmdbufferoffset < 1) 
				cmdbufferoffset =0;
		}	
	
		processchars(mapchar);
		
		if (mapchar == '\n') //enter command
		{			
		
			char **arr = NULL;//malloc(10);
		
			e9_printf("%02X ", cmdbuffer[cmdbufferoffset-1]);
			split(cmdbuffer, ' ', &arr);
				
			
			//e9_printf("%s\n", &arr[0]);
			//e9_printf("%s\n", &arr[1]);
			
			if (strcmp(arr[0], "ls") == 0)
			{
			
				echfs_ls(arr[1]);
			}
			else if (strcmp(arr[0], "load") == 0)
			{
			
				e9_printf("[SYS] Load File fs=echfs disk=0 path=%s\n", arr[1]); 

				loadbg(arr[1]);
				ResetPosition();
				
				e9_printf("[ECHFS] Loaded %s\n", arr[1]);
				
			}
			else if (strcmp(arr[0], "ticks") == 0)
			{
			 e9_printf("Ticks %d\n", GetTick());
			}
			
			else if (strcmp(arr[0], "run") == 0)
			{
				Process(arr[1]);
			}
		
			for(int x = 0; x < 255; x++)
				memset(cmdbuffer, '\0', 255);
		
			cmdbufferoffset =0;	
			
			
			e9_printf("\n> ");
		}
		
		
		if (mapchar != '\n' && mapchar != '\b')
		{
			cmdbuffer[cmdbufferoffset] = mapchar;
			cmdbufferoffset++;
		}
		
		
	}
}

void ClearScreen()
{
	uint32_t * screen = videoheader->framebuffer_addr;
	size_t size = videoheader->framebuffer_pitch * videoheader->framebuffer_height * (videoheader->framebuffer_bpp/8);

	for(int x =0; x < videoheader->framebuffer_width; x++)
		for(int y =0; y < videoheader->framebuffer_height;y++)
		{
			unsigned where = x + (y*videoheader->framebuffer_pitch/4);
			unsigned where_forward = x + ((y + 15 )*videoheader->framebuffer_pitch/4);
			
			screen[where] = 0x000000;
		}
}

