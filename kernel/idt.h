#pragma once

#include "../drivers/keyboard.h"
#include "descriptor_tables.h"
#include "isr.h"
#include "pic.h"
#include "gdt.h"


#define IDT_SIZE 256
#define PIC_1_CTRL 0x20
#define PIC_2_CTRL 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

idt_entry_t	idt_entries[256];
idt_ptr_t idt_ptr;

void init_idt()
{
	idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base = (uint64_t)&idt_entries;
	
	idt_flush();
}

void idt_set_gate( uint32_t id, uint64_t offset, uint16_t sel, uint8_t flags)
{
	idt_entries[id].offset_low  = (offset >> 0x00) & 0xFFFF;
	idt_entries[id].offset_mid  = (offset >> 0x10) & 0xFFFF;
	idt_entries[id].offset_high = (offset >> 0x20) & 0xFFFFFFFF;
	idt_entries[id].selector = sel;
	idt_entries[id].set_to_0 = 0;
	idt_entries[id].flags = flags;
}

void init_descriptor_tables()
{
	e9_printf("[GDT] loading gdt\n");
	
	setup_gdt();
	//gdt_install();
	//init_gdt();
	//load_tss();
	e9_printf("TSS Setup\n");
//	init_usergdt();
	e9_printf("[IDT] loading idt\n");
	init_idt();
}

void irq_install(void)
{
	pic_remap();
	
	//write_port(0x21, 0xFD);
	//write_port(0xA1 ,0xff);

	idt_set_gate( 0, isr0_handler_int , 0x08, 0x8E);
	idt_set_gate( 1, isr1_handler_int , 0x08, 0x8E);
	idt_set_gate( 2, isr2_handler_int , 0x08, 0x8E);
	idt_set_gate( 3, isr3_handler_int , 0x08, 0x8E);
	idt_set_gate( 4, isr4_handler_int , 0x08, 0x8E);
	idt_set_gate( 5, isr5_handler_int , 0x08, 0x8E);
	idt_set_gate( 6, isr6_handler_int , 0x08, 0x8E);
	idt_set_gate( 7, isr7_handler_int , 0x08, 0x8E);
	idt_set_gate( 8, isr8_handler_int , 0x08, 0x8E);
	idt_set_gate( 9, isr9_handler_int , 0x08, 0x8E);
	idt_set_gate( 10, isr10_handler_int , 0x08, 0x8E);
	idt_set_gate( 11, isr11_handler_int , 0x08, 0x8E);
	idt_set_gate( 12, isr12_handler_int , 0x08, 0x8E);
	idt_set_gate( 13, isr13_handler_int , 0x08, 0x8E);
	idt_set_gate( 14, isr14_handler_int , 0x08, 0x8E);
	idt_set_gate( 15, isr15_handler_int , 0x08, 0x8E);
	idt_set_gate( 16, isr16_handler_int , 0x08, 0x8E);
	idt_set_gate( 17, isr17_handler_int , 0x08, 0x8E);
	idt_set_gate( 18, isr18_handler_int , 0x08, 0x8E);
	idt_set_gate( 19, isr19_handler_int , 0x08, 0x8E);
	idt_set_gate( 20, isr20_handler_int , 0x08, 0x8E);
	idt_set_gate( 21, isr21_handler_int , 0x08, 0x8E);
	idt_set_gate( 22, isr22_handler_int , 0x08, 0x8E);
	idt_set_gate( 23, isr23_handler_int , 0x08, 0x8E);
	idt_set_gate( 24, isr24_handler_int , 0x08, 0x8E);
	idt_set_gate( 25, isr25_handler_int , 0x08, 0x8E);
	idt_set_gate( 26, isr26_handler_int , 0x08, 0x8E);
	idt_set_gate( 27, isr27_handler_int , 0x08, 0x8E);
	idt_set_gate( 28, isr28_handler_int , 0x08, 0x8E);
	idt_set_gate( 29, isr29_handler_int , 0x08, 0x8E);
	idt_set_gate( 30, isr30_handler_int , 0x08, 0x8E);
	idt_set_gate( 31, isr31_handler_int , 0x08, 0x8E);
	idt_set_gate(IRQ0, irq0_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ1, irq1_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ2, irq2_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ3, irq3_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ4, irq4_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ5, irq5_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ6, irq6_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ7, irq7_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ8, irq8_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ9, irq9_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ10, irq10_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ11, irq11_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ12, irq12_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ13, irq13_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ14, irq14_handler_int, 0x08, 0x8E);
	idt_set_gate(IRQ15, irq15_handler_int, 0x08, 0x8E);
		
	extern isr128 (regs_t );
	idt_set_gate(0x80, isr128, 0x08, 0xEE);

	//extern irq64_handler_int(registers_t );
//	idt_set_gate(64,  irq64_handler_int, 0x08, 0x8E | 0x60);


	extern keyboard_handler_int(registers_t );
	idt_set_gate(IRQ1, keyboard_handler_int, 0x08, 0x8E);

	enable_int();
}


