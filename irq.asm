
%macro pushaq 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
%endmacro ; pushaq

%macro popaq 0
    pop rdi    
    pop rsi    
    pop rbp    
    pop rbx    
    pop rdx    
    pop rcx
    pop rax
%endmacro ; popaq

[bits 64]





extern keyboard_handler
extern keyboard_handler_int
keyboard_handler_int:
pushaq
call keyboard_handler
popaq
iretq

extern disable_int
disable_int:
cli
ret

extern enable_int
enable_int:
sti
ret

extern irq0_handler
extern irq0_handler_int
irq0_handler_int:
pushaq
call irq0_handler
popaq
iretq

extern irq1_handler
extern irq1_handler_int
irq1_handler_int:
pushaq
call irq1_handler
popaq
iretq

extern irq2_handler
extern irq2_handler_int
irq2_handler_int:
pushaq
call irq2_handler
popaq
iretq

extern irq3_handler
extern irq3_handler_int
irq3_handler_int:
pushaq
call irq3_handler
popaq
iretq

extern irq4_handler
extern irq4_handler_int
irq4_handler_int:
pushaq
call irq4_handler
popaq
iretq

extern irq5_handler
extern irq5_handler_int
irq5_handler_int:
pushaq
call irq5_handler
popaq
iretq

extern irq6_handler
extern irq6_handler_int
irq6_handler_int:
pushaq
call irq6_handler
popaq
iretq

extern irq7_handler
extern irq7_handler_int
irq7_handler_int:
pushaq
call irq7_handler
popaq
iretq

extern irq8_handler
extern irq8_handler_int
irq8_handler_int:
pushaq
call irq8_handler
popaq
iretq

extern irq9_handler
extern irq9_handler_int
irq9_handler_int:
pushaq
call irq9_handler
popaq
iretq

extern irq10_handler
extern irq10_handler_int
irq10_handler_int:
pushaq
call irq10_handler
popaq
iretq

extern irq11_handler
extern irq11_handler_int
irq11_handler_int:
pushaq
call irq11_handler
popaq
iretq

extern irq12_handler
extern irq12_handler_int
irq12_handler_int:
pushaq
call irq12_handler
popaq
iretq

extern irq13_handler
extern irq13_handler_int
irq13_handler_int:
pushaq
call irq13_handler
popaq
iretq

extern irq14_handler
extern irq14_handler_int
irq14_handler_int:
pushaq
call irq14_handler
popaq
iretq

extern irq15_handler
extern irq15_handler_int
irq15_handler_int:
pushaq
call irq15_handler
popaq
iretq

extern irq64_handler
extern irq64_handler_int
irq64_handler_int:
pushaq
call irq64_handler
popaq
iretq


extern isr0_handler
extern isr0_handler_int
isr0_handler_int:
pushaq
call isr0_handler
popaq
iretq

extern isr1_handler
extern isr1_handler_int
isr1_handler_int:
pushaq
call isr1_handler
popaq
iretq

extern isr2_handler
extern isr2_handler_int
isr2_handler_int:
pushaq
call isr2_handler
popaq
iretq

extern isr3_handler
extern isr3_handler_int
isr3_handler_int:
pushaq
call isr3_handler
popaq
iretq

extern isr4_handler
extern isr4_handler_int
isr4_handler_int:
pushaq
call isr4_handler
popaq
iretq

extern isr5_handler
extern isr5_handler_int
isr5_handler_int:
pushaq
call isr5_handler
popaq
iretq

extern isr6_handler
extern isr6_handler_int
isr6_handler_int:
pushaq
call isr6_handler
popaq
iretq

extern isr7_handler
extern isr7_handler_int
isr7_handler_int:
pushaq
call isr7_handler
popaq
iretq

extern isr8_handler
extern isr8_handler_int
isr8_handler_int:
pushaq
call isr8_handler
popaq
iretq

extern isr9_handler
extern isr9_handler_int
isr9_handler_int:
pushaq
call isr9_handler
popaq
iretq

extern isr10_handler
extern isr10_handler_int
isr10_handler_int:
pushaq
call isr10_handler
popaq
iretq

extern isr11_handler
extern isr11_handler_int
isr11_handler_int:
pushaq
call isr11_handler
popaq
iretq

extern isr12_handler
extern isr12_handler_int
isr12_handler_int:
pushaq
call isr12_handler
popaq
iretq

extern isr13_handler
extern isr13_handler_int
isr13_handler_int:
pushaq
call isr13_handler
popaq
iretq

extern isr14_handler
extern isr14_handler_int
isr14_handler_int:
pushaq
call isr14_handler
popaq
iretq

extern isr15_handler
extern isr15_handler_int
isr15_handler_int:
pushaq
call isr15_handler
popaq
iretq

extern isr16_handler
extern isr16_handler_int
isr16_handler_int:
pushaq
call isr16_handler
popaq
iretq

extern isr17_handler
extern isr17_handler_int
isr17_handler_int:
pushaq
call isr17_handler
popaq
iretq

extern isr18_handler
extern isr18_handler_int
isr18_handler_int:
pushaq
call isr18_handler
popaq
iretq

extern isr19_handler
extern isr19_handler_int
isr19_handler_int:
pushaq
call isr19_handler
popaq
iretq

extern isr20_handler
extern isr20_handler_int
isr20_handler_int:
pushaq
call isr20_handler
popaq
iretq

extern isr21_handler
extern isr21_handler_int
isr21_handler_int:
pushaq
call isr21_handler
popaq
iretq

extern isr22_handler
extern isr22_handler_int
isr22_handler_int:
pushaq
call isr22_handler
popaq
iretq

extern isr23_handler
extern isr23_handler_int
isr23_handler_int:
pushaq
call isr23_handler
popaq
iretq

extern isr24_handler
extern isr24_handler_int
isr24_handler_int:
pushaq
call isr24_handler
popaq
iretq

extern isr25_handler
extern isr25_handler_int
isr25_handler_int:
pushaq
call isr25_handler
popaq
iretq

extern isr26_handler
extern isr26_handler_int
isr26_handler_int:
pushaq
call isr26_handler
popaq
iretq

extern isr27_handler
extern isr27_handler_int
isr27_handler_int:
pushaq
call isr27_handler
popaq
iretq

extern isr28_handler
extern isr28_handler_int
isr28_handler_int:
pushaq
call isr28_handler
popaq
iretq

extern isr29_handler
extern isr29_handler_int
isr29_handler_int:
pushaq
call isr29_handler
popaq
iretq

extern isr30_handler
extern isr30_handler_int
isr30_handler_int:
pushaq
call isr30_handler
popaq
iretq

extern isr31_handler
extern isr31_handler_int
isr31_handler_int:
pushaq
call isr31_handler
popaq
iretq

