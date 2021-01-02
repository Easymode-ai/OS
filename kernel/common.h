

#ifndef COMMON_H
#define COMMON_H

// Some nice typedefs, to standardise sizes accross platforms.
// These typedefs are written for 32-bit x86. 
typedef unsigned int	u32int;
typedef int	s32int;
typedef unsigned short	u16int;
typedef	short	s16int;
typedef unsigned char	u8int;
typedef	char	s8int;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint32_t;
typedef unsigned int size_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t;

void	outb(u16int port, u8int value);
u8int	inb(u16int port);
u16int	inw(u16int port);

#define PANIC(msg)	panic(msg, __FILE__, __LINE__);
#define ASSERT(b)	((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

extern void panic(const char *message, const char *file, u32int line);
extern void panic_assert(const char *file, u32int line, const char *desc);

typedef struct registers
{
	u32int	ds;					// Data segment selector
	u32int	edi, esi, ebp, esp, ebx, edx, ecx, eax;	// Pushed by pusha
	u32int	int_no, err_code;			// Interrupt number and error code (if applicable)
	u32int	eip, cs, eflags, useresp, ss;		// Pushed by the processor automatically.
} registers_t;

#endif // COMMON_H