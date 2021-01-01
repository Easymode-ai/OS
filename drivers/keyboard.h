#pragma once

#include "../kernel/isr.h"
#include "keyboard/keyboard_map.h"

int charoffset=0;

void keyboard_handler(registers_t regs) {
	
	signed  char keycode = read_port(0x60);

	if(keycode >= 0 && keyboard_map[keycode]) {
		
		if (keyboard_map[keycode] == '\b') 
		{ 
			charoffset -= 10;
			video_char((char)0, charoffset,10, BLACK);
			e9_putchar((char)keyboard_map[keycode]);

		}else{
			video_char((char)keyboard_map[keycode], charoffset, 10, BLACK);
			e9_putchar((char)keyboard_map[keycode]);
			charoffset += 10;
		}
	}
	
	write_port(0x20, 0x20);
}
