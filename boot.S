# res
.set WIDTH, 800  /* requested width */
.set HEIGHT, 600  /* requested height */
.set DEPTH, 32    /* requested bits per pixel BPP */

# set magic number to 0x1BADB002 to identified by bootloader 
.set MAGIC,    0x1BADB002

# set flags to 0
.set FLAGS,    0#ALIGN | MEMINFO | VIDINFO

# set the checksum
.set CHECKSUM, -(MAGIC + FLAGS)

# set multiboot enabled
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .stivalehdr, "ax", @progbits
.global stivale_header
stivale_header:
.long stackTop
.long 0
.word 1
.word WIDTH
.word HEIGHT
.word DEPTH
.quad 0

.section .bss
stack:
.space 4096

# set the stack bottom 
.section .bss
stackBottom:

.skip 524288

stackTop:

.section .text
.type _start, @function
cli
# assign current stack pointer location to stackTop
mov $stackTop, %esp
hltLoop:
call _start	
hlt
jmp hltLoop	

# enable paging cr0
.text
.global enablePaging
enablePaging:
#push %ebp
#mov %esp, %ebp
mov %cr0, %eax
or $0x80000001, %eax
mov %eax, %cr0
#mov %ebp, %esp
#pop %ebp
ret

# loadpage cr3
.text
.global loadpage
loadpage:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
leave
ret

.text
.global load_idt
load_idt:
mov 4(%esp),%eax
lidt (%eax)
sti
ret

#arg: int, port number.
.global read_port
read_port:
mov 4(%esp), %edx
in %dx, %al 		 
ret

# arg: int, (dx)port number
#      int, (al)value to write
.global write_port
write_port:
mov   4(%esp), %edx  
mov   8(%esp), %al 
out   %al, %dx 
ret

.global keyboard_handler_int
keyboard_handler_int:
cli
cld
pusha
call keyboard_handler
mov $32, %al
out %al, $160
out %al, $32
popa
sti
iret

.global gdt_flush
gdt_flush:
mov 4(%esp), %eax
lgdt (%eax)
jmp $0x08, $complete_flush
complete_flush:
mov $0x10,%ax
mov %ax, %ds
mov %ax, %es
mov %ax, %fs
mov %ax, %gs
mov %ax, %ss
ret

.global irq_common_stub
irq_common_stub:
pusha
push %ds
push %es
push %fs
push %gs
movw $0x10,%ax
movw %ax,%ds
movw %ax,%es
movw %ax,%fs
movw %ax,%gs
movl %esp,%eax
pushl %eax
movl $irq_handler, %eax
call *%eax
popl %eax
pop %gs 
pop %fs
pop %es
pop %ds
popa
addl $8,%esp
iret

.global isr0
isr0:
pusha
call isr0_handler
popa
iret

.global isr1
isr1:
pusha
call isr1_handler
popa
iret

.global isr2
isr2:
pusha
call isr2_handler
popa
iret

.global isr3
isr3:
pusha
call isr3_handler
popa
iret

.global isr4
isr4:
pusha
call isr4_handler
popa
iret

.global isr5
isr5:
pusha
call isr5_handler
popa
iret

.global isr6
isr6:
pusha
call isr6_handler
popa
iret

.global isr7
isr7:
pusha
call isr7_handler
popa
iret

.global isr8
isr8:
pusha
call isr8_handler
popa
iret

.global isr9
isr9:
pusha
call isr9_handler
popa
iret

.global isr10
isr10:
pusha
call isr10_handler
popa
iret

.global isr11
isr11:
pusha
call isr11_handler
popa
iret

.global isr12
isr12:
pusha
call isr12_handler
popa
iret

.global isr13
isr13:
pusha
call isr13_handler
popa
iret

.global isr14
isr14:
pusha
call isr14_handler
popa
iret

.global isr15
isr15:
pusha
call isr15_handler
popa
iret

.global isr16
isr16:
pusha
call isr16_handler
popa
iret

.global isr17
isr17:
pusha
call isr17_handler
popa
iret

.global isr18
isr18:
pusha
call isr18_handler
popa
iret

.global isr19
isr19:
pusha
call isr19_handler
popa
iret

.global isr20
isr20:
pusha
call isr20_handler
popa
iret

.global isr21
isr21:
pusha
call isr21_handler
popa

ret
.global isr22
isr22:
pusha
call isr22_handler
popa
iret

.global isr23
isr23:
pusha
call isr23_handler
popa
iret

.global isr24
isr24:
pusha
call isr24_handler
popa
iret

.global isr25
isr25:
pusha
call isr25_handler
popa
iret

.global isr26
isr26:
pusha
call isr26_handler
popa
iret

.global isr27
isr27:
pusha
call isr27_handler
popa

.global isr28
isr28:
pusha
call isr28_handler
popa
iret

.global isr29
isr29:
pusha
call isr29_handler
popa
iret

.global isr30
isr30:
pusha
call isr30_handler
popa
iret

.global isr31
isr31:
pusha
call isr31_handler
popa
iret

.global irq0
irq0:
pusha
call irq0_handler
popa
iret

.global irq1
irq1:
cli
pushl $1
pushl $33
jmp irq_common_stub
iret

.global irq2
irq2:
pusha
call irq2_handler
popa
iret

.global irq3
irq3:
pusha
call irq3_handler
popa
iret

.global irq4
irq4:
pusha
call irq4_handler
popa
iret

.global irq5
irq5:
pusha
call irq5_handler
popa
iret

.global irq6
irq6:
pusha
call irq6_handler
popa
iret

.global irq7
irq7:
pusha
call irq7_handler
popa
iret

.global irq8
irq8:
pusha
call irq8_handler
popa
iret

.global irq9
irq9:
pusha
call irq9_handler
popa
iret

.global irq10
irq10:
pusha
call irq10_handler
popa
iret

.global irq11
irq11:
pusha
call irq11_handler
popa
iret

.global irq12
irq12:
pusha
call irq12_handler
popa
iret

.global irq13
irq13:
pusha
call irq13_handler
popa
iret

.global irq14
irq14:
pusha
call irq14_handler
popa
iret

.global irq15
irq15:
pusha
call irq15_handler
popa
iret

