BIN = MyOS.bin
LIB_SOURCES = $(wildcard libc/include/*.c libc/string/*.c libc/stdlib/*.c)
HEADERS = $(wildcard *.h kernel/*.h drivers/*.h libc/include/*.h)
AS = as
CC = gcc
LD = ld
ASFLAGS = --32 boot.S -o boot.o
CLIB = -m32 -c $(LIB_SOURCES) -std=gnu99 -O2 -w 
CFLAGS = -m32 -c kernel.c -std=gnu99 -ffreestanding -O2 -w -o kernel.o
ARFLAGS = -crs library.a *.o
LDFLAGS = -m elf_i386 -T linker.ld kernel.o memset.o boot.o -o $(BIN) -nostdlib

make:
	make clean
	$(AS) $(ASFLAGS)
	$(CC) $(CLIB)
	$(CC) $(CFLAGS)
	$(LD) $(LDFLAGS)

clean:
	rm -rf *.bin
	rm -rf kernel.o
	rm -rf boot.o