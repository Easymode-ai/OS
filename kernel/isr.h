/**
* isr.h - Interface to structures for high level interrupt service routines.
* @version $Id$
*/
#pragma once
#include "common.h"

// A few defines to make life a little easier
#define IRQ0	32
#define IRQ1	33
#define IRQ2	34
#define IRQ3	35
#define IRQ4	36
#define IRQ5	37
#define IRQ6	38
#define IRQ7	39
#define IRQ8	40
#define IRQ9	41
#define IRQ10	42
#define IRQ11	43
#define IRQ12	44
#define IRQ13	45
#define IRQ14	46
#define IRQ15	47

typedef struct registers
{
	u32int	ds;					// Data segment selector
	u32int	edi, esi, ebp, esp, ebx, edx, ecx, eax;	// Pushed by pusha
	u32int	int_no, err_code;			// Interrupt number and error code (if applicable)
	u32int	eip, cs, eflags, useresp, ss;		// Pushed by the processor automatically.
} registers_t;

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the 
// first parameter.
typedef void (*isr_t)(registers_t);
isr_t interrupt_handlers[256];

extern isr_t interrupt_handlers[];

void register_interrupt_handler(u8int n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t regs)
{
	
	if (interrupt_handlers[regs.int_no] != 0) {
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	} else {
		//monitor_write("unhandled interrupt: ");
		//monitor_write_dec(regs.int_no);
		//monitor_put('\n');
	}
}

// This gets called from our ASM interrupt handler stub.d
void irq_handler(registers_t regs)
{
	
	e9_printf("%d", regs.int_no);

	// Send EOI (end of interrupt) signal to the PICs.
	// If this interrupt involved the slave.
	if (regs.int_no >= 40) {
		// Send reset signal to slave.
		write_port(0xA0, 0x20);
		e9_printf("reset signal to slave");
	}

	// Send reset signal to master. (As well as slave, if necessary).
	write_port(0x20, 0x20);

	if (interrupt_handlers[regs.int_no] != 0) {
		
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		e9_printf("IRQ HANDLER");
		
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
		
	}
}


void isr0_handler(void) {
	
	e9_printf("\nISR0 - Divide by zero");
	write_port(0x20, 0x20); //EOI
}

void isr1_handler(void) {
	
	e9_printf("\nISR1 - Debug");
	write_port(0x20, 0x20); //EOI
}

void isr2_handler(void) {
	
	e9_printf("\nISR2 - NMI");
	write_port(0x20, 0x20); //EOI
}

void isr3_handler(void) {
	
	e9_printf("\nISR3 - Breakpoint");
	write_port(0x20, 0x20); //EOI
}

void isr4_handler(void) {
	
	e9_printf("\nISR4 - Detected overflow");
	write_port(0x20, 0x20); //EOI
}

void isr5_handler(void) {
	
	e9_printf("\nISR5 - Out of bounds");
	write_port(0x20, 0x20); //EOI
}

void isr6_handler(void) {
	
	e9_printf("\nISR6 - Invalid opcode");
	write_port(0x20, 0x20); //EOI
}

void isr7_handler(void) {
	
	e9_printf("\nISR7 - No coprocessor");
	write_port(0x20, 0x20); //EOI
}

void isr8_handler(void) {
	
	e9_printf("\nISR8 - Double Fault ");
	write_port(0x20, 0x20); //EOI
}

void isr9_handler(void) {
	
	e9_printf("\nISR - CoProcessor Segment Overrun");
	write_port(0x20, 0x20); //EOI
}

void isr10_handler(void) {
	
	e9_printf("\nISR10 - BAD TSS");
	write_port(0x20, 0x20); //EOI
}

void isr11_handler(void) {
	
	e9_printf("\nISR11 - Segement Not Present");
	write_port(0x20, 0x20); //EOI
}

void isr12_handler(void) {
	
	e9_printf("\nISR12 - Stack Fault\n");
	write_port(0x20, 0x20); //EOI
}

void isr13_handler(void) {
	
	e9_printf("\nISR13 - General Protection Failure\n");
	write_port(0x20, 0x20); //EOI
		
//	*(uint8_t*)~0 = 0x69;
}

void isr14_handler(void) {
	
	uint64_t cr2;
	asm("mov %%cr2, %[out]": [out]"=r"(cr2));
  
	e9_printf("\nISR14 - Page Fault %x \n ", cr2);
	write_port(0x20, 0x20); //EOI
}

void isr15_handler(void) {
	
	e9_printf("\nISR15 - Unknown Interrupt");
	write_port(0x20, 0x20); //EOI
}

void isr16_handler(void) {
	
	e9_printf("\nISR16 - Coprocessor Fault");
	write_port(0x20, 0x20); //EOI
}

void isr17_handler(void) {
	
	e9_printf("\nISR17 - Alignment Check");
	write_port(0x20, 0x20); //EOI
}

void isr18_handler(void) {
	
	e9_printf("\nISR18 - Machine Check");
	write_port(0x20, 0x20); //EOI
}

void isr19_handler(void) {
	
	e9_printf("\nISR19 - Reserved");
	write_port(0x20, 0x20); //EOI
}

void isr20_handler(void) {
	
	e9_printf("\nISR20 - Reserved");
	write_port(0x20, 0x20); //EOI
}

void isr21_handler(void) {
	
	e9_printf("\nISR21 - Reserved");
	write_port(0x20, 0x20); //EOI
}

void isr22_handler(void) {
	
	e9_printf("hi22");
	write_port(0x20, 0x20); //EOI
}

void isr23_handler(void) {
	
	e9_printf("hi23");
	write_port(0x20, 0x20); //EOI
}

void isr24_handler(void) {
	
	e9_printf("hi24");
	write_port(0x20, 0x20); //EOI
}

void isr25_handler(void) {
	
	e9_printf("hi25");
	write_port(0x20, 0x20); //EOI
}

void isr26_handler(void) {
	
	e9_printf("hi26");
	write_port(0x20, 0x20); //EOI
}

void isr27_handler(void) {
	
	e9_printf("hi27");
	write_port(0x20, 0x20); //EOI
}

void isr28_handler(void) {
	
	e9_printf("hi28");
	write_port(0x20, 0x20); //EOI
}

void isr29_handler(void) {
	
	e9_printf("hi29");
	write_port(0x20, 0x20); //EOI
}

void isr30_handler(void) {
	
	e9_printf("hi30");
	write_port(0x20, 0x20); //EOI
}

void isr31_handler(void) {
	
	e9_printf("hi31");
	write_port(0x20, 0x20); //EOI
}

void irq0_handler(void) {
	
	e9_printf("hi0");
	write_port(0x20, 0x20); //EOI
}

void irq1_handler(void) {
	
	e9_printf("hi from irq1");
	write_port(0x20, 0x20); //EOI
}

void irq2_handler(void) {
	
	e9_printf("hi2");
	write_port(0x20, 0x20); //EOI
}

void irq3_handler(void) {
	
	e9_printf("hi3");
	write_port(0x20, 0x20); //EOI
}

void irq4_handler(void) {
	
	e9_printf("hi4");
	write_port(0x20, 0x20); //EOI
}

void irq5_handler(void) {
	
	e9_printf("hi5");
	write_port(0x20, 0x20); //EOI
}

void irq6_handler(void) {
	
	e9_printf("hi6");
	write_port(0x20, 0x20); //EOI
}

void irq7_handler(void) {
	
	e9_printf("hi7");
	write_port(0x20, 0x20); //EOI
}

void irq8_handler(void) {
	
	//e9_printf("hi8");
	write_port(0x20, 0x20); //EOI
}

void irq9_handler(void) {
	
	e9_printf("hi9");
	write_port(0x20, 0x20); //EOI
}

void irq10_handler(void) {
	
	e9_printf("hi10");
	write_port(0x20, 0x20); //EOI
}

void irq11_handler(void) {
	
	e9_printf("hi11");
	write_port(0x20, 0x20); //EOI
}

void irq12_handler(void) {
	
	e9_printf("hi12");
	write_port(0x20, 0x20); //EOI
}

void irq13_handler(void) {
	
	//e9_printf("hi13");
	write_port(0x20, 0x20); //EOI
}

void irq14_handler(void) {
	
	e9_printf("hi14");
	write_port(0x20, 0x20); //EOI
}

void irq15_handler(void) {
	
	e9_printf("hi15");
	write_port(0x20, 0x20); //EOI
}