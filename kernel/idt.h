#pragma once

#include "descriptor_tables.h"
#include "isr.h"

#define IDT_SIZE 256
#define PIC_1_CTRL 0x20
#define PIC_2_CTRL 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

struct idt_entry
{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short int offset_higherbits;
} __attribute__((packed));

struct idt_pointer
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct IDT_entry{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

extern void gdt_flush(u32int);
extern void idt_flush(u32int);

idt_entry_t	idt_entries[256];
idt_ptr_t	idt_ptr;
gdt_entry_t	gdt_entries[5];
gdt_ptr_t	gdt_ptr;

static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags)
{
	idt_entries[num].base_lo	= base & 0xFFFF;
	idt_entries[num].base_hi	= (base >> 16) & 0xFFFF;
	idt_entries[num].sel		= sel;
	idt_entries[num].always0	= 0;
	// We must uncomment the OR below when we get to using user-mode.
	// It sets the interrupt gate's privilege level to 3.
	idt_entries[num].flags		= flags /* | 0x60 */;
}

static void gdt_set_gate(s32int num, u32int base, u32int limit, u8int access, u8int gran)
{
	gdt_entries[num].base_low	= (base & 0xFFFF);
	gdt_entries[num].base_middle	= (base >> 16) & 0xFF;
	gdt_entries[num].base_high	= (base >> 24) & 0xFF;
	gdt_entries[num].limit_low	= (limit & 0xFFFF);
	gdt_entries[num].granularity	= (limit >> 16) & 0x0F;
	gdt_entries[num].granularity	|= gran & 0xF0;
	gdt_entries[num].access		= access;
}

static void init_gdt()
{
	gdt_ptr.limit	= (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base	= (u32int)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);			// Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	// Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	// Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);	// User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);	// User mode data segment
	
	gdt_flush(&gdt_ptr);

}

void init_descriptor_tables()
{
	// Initialise the global descriptor table.
	init_gdt();
	init_idt();
	memset(&interrupt_handlers, 0, sizeof(isr_t) * 256);

	idt_set_gate(33, keyboard_handler_int, 0x08, 0x8E);
	//idt_set_gate(0x21, (unsigned long) keyboard_handler_int, 0x08, 0x8e);
	write_port(0x21, 0xFD);	
	
	// Nullify all the interrupt handlers.
	//memset(&interrupt_handlers, 0, sizeof(isr_t) * 256);

}

void init_idt()
{
	idt_ptr.limit	= sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base	= (u32int)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

	// Remap the irq table.
	write_port(0x20, 0x11);
	write_port(0xA0, 0x11);
	write_port(0x21, 0x20);
	write_port(0xA1, 0x28);
	write_port(0x21, 0x04);
	write_port(0xA1, 0x02);
	write_port(0x21, 0x01);
	write_port(0xA1, 0x01);
	write_port(0x21, 0x0);
	write_port(0xA1, 0x0);

	//set gates
	idt_set_gate( 0, (u32int)isr0 , 0x08, 0x8E);
	idt_set_gate( 1, (u32int)isr1 , 0x08, 0x8E);
	idt_set_gate( 2, (u32int)isr2 , 0x08, 0x8E);
	idt_set_gate( 3, (u32int)isr3 , 0x08, 0x8E);
	idt_set_gate( 4, (u32int)isr4 , 0x08, 0x8E);
	idt_set_gate( 5, (u32int)isr5 , 0x08, 0x8E);
	idt_set_gate( 6, (u32int)isr6 , 0x08, 0x8E);
	idt_set_gate( 7, (u32int)isr7 , 0x08, 0x8E);
	idt_set_gate( 8, (u32int)isr8 , 0x08, 0x8E);
	idt_set_gate( 9, (u32int)isr9 , 0x08, 0x8E);
	idt_set_gate(10, (u32int)isr10, 0x08, 0x8E);
	idt_set_gate(11, (u32int)isr11, 0x08, 0x8E);
	idt_set_gate(12, (u32int)isr12, 0x08, 0x8E);
	idt_set_gate(13, (u32int)isr13, 0x08, 0x8E);
	idt_set_gate(14, (u32int)isr14, 0x08, 0x8E);
	idt_set_gate(15, (u32int)isr15, 0x08, 0x8E);
	idt_set_gate(16, (u32int)isr16, 0x08, 0x8E);
	idt_set_gate(17, (u32int)isr17, 0x08, 0x8E);
	idt_set_gate(18, (u32int)isr18, 0x08, 0x8E);
	idt_set_gate(19, (u32int)isr19, 0x08, 0x8E);
	idt_set_gate(20, (u32int)isr20, 0x08, 0x8E);
	idt_set_gate(21, (u32int)isr21, 0x08, 0x8E);
	idt_set_gate(22, (u32int)isr22, 0x08, 0x8E);
	idt_set_gate(23, (u32int)isr23, 0x08, 0x8E);
	idt_set_gate(24, (u32int)isr24, 0x08, 0x8E);
	idt_set_gate(25, (u32int)isr25, 0x08, 0x8E);
	idt_set_gate(26, (u32int)isr26, 0x08, 0x8E);
	idt_set_gate(27, (u32int)isr27, 0x08, 0x8E);
	idt_set_gate(28, (u32int)isr28, 0x08, 0x8E);
	idt_set_gate(29, (u32int)isr29, 0x08, 0x8E);
	idt_set_gate(30, (u32int)isr30, 0x08, 0x8E);
	idt_set_gate(31, (u32int)isr31, 0x08, 0x8E);
	idt_set_gate(IRQ0, (u32int)irq0, 0x08, 0x8E);
	idt_set_gate(IRQ1, (u32int)irq1, 0x08, 0x8E);
	idt_set_gate(IRQ2, (u32int)irq2, 0x08, 0x8E);
	idt_set_gate(IRQ3, (u32int)irq3, 0x08, 0x8E);
	idt_set_gate(IRQ4, (u32int)irq4, 0x08, 0x8E);
	idt_set_gate(IRQ5, (u32int)irq5, 0x08, 0x8E);
	idt_set_gate(IRQ6, (u32int)irq6, 0x08, 0x8E);
	idt_set_gate(IRQ7, (u32int)irq7, 0x08, 0x8E);
	idt_set_gate(IRQ8, (u32int)irq8, 0x08, 0x8E);
	idt_set_gate(IRQ9, (u32int)irq9, 0x08, 0x8E);
	idt_set_gate(IRQ10, (u32int)irq10, 0x08, 0x8E);
	idt_set_gate(IRQ11, (u32int)irq11, 0x08, 0x8E);
	idt_set_gate(IRQ12, (u32int)irq12, 0x08, 0x8E);
	idt_set_gate(IRQ13, (u32int)irq13, 0x08, 0x8E);
	idt_set_gate(IRQ14, (u32int)irq14, 0x08, 0x8E);
	idt_set_gate(IRQ15, (u32int)irq15, 0x08, 0x8E);
	
	load_idt((u32int)&idt_ptr);
}
