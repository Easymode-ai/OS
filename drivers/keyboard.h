#pragma once

#include "keyboard/keyboard_map.h"

static int charoffset=0;

void keyboard_handler(void)
{
	signed char keycode;

	keycode = read_port(0x60);
	/* Only print characters on keydown event that have
	* a non-zero mapping */
	if(keycode >= 0 && keyboard_map[keycode]) {
		
		if (keyboard_map[keycode] == '\b') 
		{ 
			charoffset--;
			vga_putchar((char)0, 10,charoffset,BLACK);
			e9_putchar((char)keyboard_map[keycode]);

		}else{
			vga_putchar((char)keyboard_map[keycode], 10,charoffset,BLACK);
			e9_putchar((char)keyboard_map[keycode]);
			charoffset++;
		}
		/* Attribute 0x07 is white on black characters */
		// vidptr[current_loc++] = 0x07;
	}

	write_port(0x20, 0x20);
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}