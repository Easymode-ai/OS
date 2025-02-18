#pragma once

#include "../kernel/library/e9print.h"
#include "../kernel/isr.h"
#include "keyboard/keyboard_map.h"

void extern keyboard_handler(registers_t regs) {
	
	char keycode = (char)read_port(0x60);
	
	processkeys(keycode);
	write_port(0x20, 0x20);
	
}
