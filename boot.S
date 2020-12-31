# set magic number to 0x1BADB002 to identified by bootloader 
.set MAGIC,    0x1BADB002

# bits
.set ALIGN,    1<<0             
.set MEMINFO,  1<<1
.set VIDINFO,  1<<2

# set flags to 0
.set FLAGS,    0#ALIGN | MEMINFO | VIDINFO

# set the checksum
.set CHECKSUM, -(MAGIC + FLAGS)

# set multiboot enabled
.section .multiboot
.align 4
# define type to long for each data defined as above
.long MAGIC
.long FLAGS
.long CHECKSUM
.set WIDTH, 800  /* requested width */
.set HEIGHT, 600  /* requested height */
.set DEPTH, 32    /* requested bits per pixel BPP */
#.long 0 	#12	u32	header_addr	if flags[16] is set
#.long 0 	#16	u32	load_addr	if flags[16] is set
#.long 0 	#20	u32	load_end_addr	if flags[16] is set
#.long 0 	#24	u32	bss_end_addr	if flags[16] is set
#.long 0 	#28	u32	entry_addr	if flags[16] is set
#.long 0 	#32	u32	mode_type	if flags[2] is set
#.long 800 	#36	u32	width	if flags[2] is set
#.long 600 	#40	u32	height	if flags[2] is set
#.long 32 	#44	u32	depth	if flags[2] is set

#.section .stivalehdr
#.long 0
#.long 0
#.long 0
#.long 0
#.long 0
#.long 0

# set the stack bottom 
.section .bss
stackBottom:

# define the maximum size of stack to 512 bytes
.skip 524288

# set the stack top which grows from higher to lower
stackTop:

.section .text
.global _start
.type _start, @function


_start:
#mov 0x4F00,%ax  #set VBE graphic mode
#mov 0x4114,%bx  #graphic mode 800x600 16bpp
#int $0x10  #start BIOS interrupt



# assign current stack pointer location to stackTop
mov $stackTop, %esp
hltLoop:
call kernel_entry				# call the kernel main source
#cli
hlt
jmp hltLoop						# put system in infinite loop

.size _start, . - _start

.text
.global enablePaging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret

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

#.text
#.global _free
#_free:
#push %ebp
#mov %esp, %ebp
#sub $0x10, %esp
#movb $0x8, -0x1(%ebp)
#mov $0x0, %eax  
#leave
#ret

#.text
#.global malloc
#malloc:
#push %ebp
#mov %esp, %ebp
#mov 8(%esp), %eax
#leave
#ret*/

#STACK
#.text
#.global sbrk
#sbrk:
#push %ebp
#mov %esp, %ebp
#sub $0x10, %esp
#subtrack stack pointer down 0x10 to allocate ram
#movb $0x8, -0x1(%ebp)
#move byte uint8_t i = 8
#mov $0x0, %eax  
#return 0
#leave
#ret

.text
.global load_idt
load_idt:
mov 4(%esp),%eax
lidt (%eax)
sti
ret

#.text
#.global _key
#_key:
#xor %ah, %ah
#int $0x16 #BIOS Keyboard Service 
#ret*/

#.text
#.global idt_flush
#idt_flush:
#mov 4(%esp), %eax  	# Get the pointer to the IDT, passed as a parameter.
#lidt (%eax)        		# Load the IDT pointer.
#ret

#arg: int, port number.
.global read_port
read_port:
mov 4(%esp), %edx	#mov edx, [esp + 4]
in %dx, %al 		#in al, dx   
ret

# arg: int, (dx)port number
#      int, (al)value to write
.global write_port
write_port:
mov   4(%esp), %edx #edx, [esp + 4]    
mov   8(%esp), %al #al, [esp + 4 + 4]  
out   %al, %dx #dx, al  
ret

.global keyboard_handler_int
keyboard_handler_int:
call keyboard_handler
iret

#.text
#.global memset
#memset:
#kmemset:      
#push    (%edi)          

#mov     16(%esp), %ecx, #ecx, [esp + 16] ; size_t num
#mov     12(%esp), %al #, [esp + 12]  ; int value 
#mov     8(%esp), %edi # void * ptr
#rep     stosb 

#mov     8(%esp),%eax #return ptr
# pop     %edi            #edi
#ret                     # let caller adjust stack
#
