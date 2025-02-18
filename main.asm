; A boot sector that enters 32-bit protected  mode.
[bits 64]
%include "gdtidt.asm"
%include "mem.asm"
%include "irq.asm"
%include "syscall.asm"


[bits 64]
seed resq 1

extern Random_init
Random_init:
	MOV 	[seed], RCX	
	RET
	
extern Random_next
Random_next:
	MOV 	RCX, [seed]
	MOV 	RDX, RCX
	SHR 	RDX, 12
	XOR 	RCX, RDX
	MOV 	RDX, RCX
	SHL 	RDX, 25
	XOR 	RCX, RDX
	MOV 	RDX, RCX
	SHR 	RDX, 27
	XOR 	RCX, RDX
	MOV 	RAX, 2685821657736338717
	MUL 	RCX
	MOV 	[seed], RCX	
	RET


[bits 64]
extern end
global end

section .bss
global stack_space
stack_space:
resb 18192; 8KB for stack
stack_end:
section .text


