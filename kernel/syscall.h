#pragma once
typedef struct
{
uint64_t 
		r15, r14, r13, r12, r11, r10, r9, r8,
		rdi, rsi, rbp, rbx, rcx, rdx, rax,
		rip, cs, rflags, rsp, ss;
} __attribute__((packed)) regs_t;

uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx)
{

	asm("int $0x80;"
		:
		: "a"(rax), "b"(rbx), "c"(rcx), "d"(rdx)
		);
}
struct cpu_gpr_context {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};


void syscall_debug_log() 
{

	asm volatile ("out %0, %1" : : "a"('H'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('E'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('L'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('L'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('O'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('\n'), "Nd"(0xe9));

}

void *sys_calls[] = 
{
	syscall_debug_log,

};

void syscall_int(regs_t *regs)	
{
	
	asm volatile ("out %0, %1" : : "a"('r'), "Nd"(0xe9));
	void (*f)(uint64_t, uint64_t, uint64_t) = sys_calls[regs->rax];
	f(regs->rbx, regs->rcx, regs->rdx);
	
}

static void *sys_handlers[] = {
	syscall_debug_log
};
#define ARR_SIZE(x) \
	(int) ((sizeof(x)) / (sizeof(*(x))))
void syscall_handler(registers_t *r)
{
	
	int num = r->eax;
	int (*func) (void);

	int irqf = r;
	if (num < ARR_SIZE(sys_handlers) && sys_handlers[r->eax]) {
		func = sys_handlers[r->eax];
		r->eax = func();
	} else {
		e9_printf("Unsupported syscall %d\n", num);
	}
	irqf = NULL;
}

void irq64_handler(registers_t *r){
	
	//asm volatile ("out %0, %1" : : "a"('H'), "Nd"(0xe9));
	syscall_handler(r);
}
static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t edx, eax;
    asm volatile ("rdmsr"
                  : "=a" (eax), "=d" (edx)
                  : "c" (msr)
                  : "memory");
    return ((uint64_t)edx << 32) | eax;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t edx = value >> 32;
    uint32_t eax = (uint32_t)value;
    asm volatile ("wrmsr"
                  :
                  : "a" (eax), "d" (edx), "c" (msr)
                  : "memory");
}