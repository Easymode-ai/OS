
TARGET := hello.elf
LIMINE := limine/
AS 				= nasm 
CC              = clang
CFLAGS          = -O2
LD              = ld

ASFLAGS			= -f elf64
LDINTERNALFLAGS := -Tlinker.ld -nostdlib -no-pie -static
INTERNALCFLAGS  := -I../../stivale -I../ -I./ -I../kernel -I../kernel/library/ -I../kernel/fs/ -o hello.elf  -fomit-frame-pointer  -nostdlib -no-pie -mno-sse
OBJ       :=  hello.o

install:

	make clean
	$(AS) $(ASFLAGS) main.asm
	$(CC) $(CFLAGS) -static main.o ../ordered_array.o ../pmm.o ../vmm.o ../pci.o ../display.o ../command.o ../timer.o ../ahci.o ../echfs.o   ../main.o $(INTERNALCFLAGS) -masm=att -fPIC -g hello.c -v 
	
	#$(LD) $(LDINTERNALFLAGS) $(OBJ) -o $(TARGET)
	
#make

	
clean:
	rm -rf $(TARGET) *.elf
