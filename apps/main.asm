[bits 64]

section .text
extern main
global _start
_start:
mov dx, 0x21
in al, dx
out 0xE9, al
jmp main
ret
