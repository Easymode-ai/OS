#pragma once

#include "../kernel/isr.h"
#include "keyboard/keyboard_map.h"


void keyboard_handler(registers_t regs) {
	
	signed  char keycode = read_port(0x60);
	processkeys(keycode);
	
	write_port(0x20, 0x20);
}
