#pragma once

void e9_putchar(char * arg)
{
	write_port(0xE9 , arg);
}

static const char lut[17] = { '0', '1', '2','3','4','5','6','7','8','9','0','A','B','C','D','E','F'};

static char* hexlookup(char buffer[11], int value){
	buffer[0] = '0';
	buffer[1] = 'x';
	
	buffer[2] = lut[(value & 0xF0000000) >> 28];
	buffer[3] = lut[(value & 0x0F000000) >> 24];
	buffer[4] = lut[(value & 0x00F00000) >> 20];
	buffer[5] = lut[(value & 0x000F0000) >> 16];
	buffer[6] = lut[(value & 0x0000F000) >> 12];
	buffer[7] = lut[(value & 0x00000F00) >> 8];
	buffer[8] = lut[(value & 0x000000F0) >> 4];
	buffer[9] = lut[(value & 0x0000000F)];

	return buffer;
}

static int lenHelper(unsigned x) {
	if (x >= 100000000000000) return 15;
	if (x >= 10000000000000) return 14;
	if (x >= 1000000000000) return 13;
	if (x >= 100000000000) return 12;
	if (x >= 10000000000) return 11;
	if (x >= 1000000000) return 10;
	if (x >= 100000000)  return 9;
	if (x >= 10000000)   return 8;
	if (x >= 1000000)    return 7;
	if (x >= 100000)     return 6;
	if (x >= 10000)      return 5;
	if (x >= 1000)       return 4;
	if (x >= 100)        return 3;
	if (x >= 10)         return 2;
	return 1;
}




/*static int printf( char * arg, int x,  int y, char color)
{
	int len =0;
	for(int i=0; arg[i]!='\0';++i){
		vga_putchar((char)arg[i], x + (len *10) , y ,  color);
		e9_putchar(arg[i]);
		len++;
	}
	e9_putchar('\n');
	return len;
}*/

static int printint( int integer, int x,  int y, char color)
{
	char buffer[15];

	int measure = lenHelper(integer);
	int size=measure;
	
	for(int r = measure; r >= 0; r--)
	{
		buffer[r] = (integer % 10);
		integer /= 10;
	}
	
	for(int i =1; i < measure+1; i++)
	vga_putchar((char)buffer[i]+'0', x, y +i-1 ,  color);
	
	return measure-1;
}

static void printsize(int integer, int x, int y, char color, int type)
{
	switch(type)
	{
	case 1: //bytes
		{
			int len = printint(integer*1024,x,y,color);
			//printf("Bytes", x , y + len+1, RED);
		}
		break;
	case 1024: //kb
		{
			int len = printint(integer,x,y,color);
			//printf("KBytes", x , y + len+1, RED);
		}
		break;
	case 1024*1024: //mb
		{
			int len = printint(integer/1024,x,y,color);
			//printf("MB", x , y + len+1, RED);
		}
		break;
	}
}

static void printhex( char * arg, int x,  int y, char color)
{
	char buffer[15];

	hexlookup(buffer, arg);
	
	for(int i=9; i > -1;i--)
	{
	vga_putchar((char)buffer[i], x, y +i ,  color);

	}
		for(int i=0; i  <10;i++)
	{
	
	
		e9_putchar((char)buffer[i]);
	}
}

