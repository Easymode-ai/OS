[bits 64]
extern gdt_flush
extern gdtr_ptr
gdt_flush:
	MOV RAX, gdtr_ptr
	lgdt [RAX]
	MOV AX, 0x10
	MOV ds, ax 
	MOV SS, ax                
	MOV es, ax
	MOV fs, ax
	MOV gs, ax  

	PUSH 0x08
	PUSH .retr
	RETFQ	
.retr:
	RET
	

extern idt_ptr
extern idt_flush
idt_flush:
	MOV RAX, idt_ptr
	LIDT [RAX]
	STI
	RET

;extern tss_ptr
global load_tss
load_tss:
	;MOV AX,  0x28
	ltr [tss_ptr]
	ret

tss_ptr:
	dw 0x28

	
[bits 64]
;extern usermode
;extern usermodetest
;;usermode:
; cli
 ;   MOV AX, 0x23
 ;   mov ds,ax
  ;  mov es,ax 
 ;   mov fs,ax 
;    mov gs,ax ;we don't need to worry about SS. it's handled by iret

;	MOV eax,esp    
 ;   PUSH 0x23
;	PUSH RAX
 ; 	PUSHF
;    PUSH 0x1B
;	PUSH usermodetest
;	iretq	


extern usermode
extern usermodetest
usermode:
 	cli
	mov ax, 0x23
    mov ds,ax
   mov es,ax 
   mov fs,ax 
    mov gs,ax ;we don't need to worry about SS. it's handled by iret
	mov rax, 0x7FC0000000	; Stack
    PUSH 0x23
	PUSH RAX
  	PUSHF
    PUSH 0x1B
	PUSH usermodetest
	iretq	

	
extern e9write
e9write:
;pushaq
call syscall_debug_log
;popaq
iretq

global isr128
isr128:

harr:
	
	
	mov ax, 0x61
	out 0xe9, ax
	
	jmp harr;
	
	push_context
	mov rdi, rsp
	extern syscall_int
	call   syscall_int
	pop_context
	pop rax
	
	
	
	iretq


