LIMINE=/home/scx/limine/limine-barebones-master/
BIN = MyOS.bin
LIB_SOURCES = $(wildcard libc/include/*.c libc/string/*.c libc/stdlib/*.c)
HEADERS = $(wildcard *.h kernel/*.h drivers/*.h libc/include/*.h)
AS = as
CC = gcc
LD = ld
ASFLAGS = --32 boot.S -o boot.o
CLIB = -m32 -c $(LIB_SOURCES) -std=gnu99 -O2 -w 
 CFLAGS = -m32 -c kernel.c -std=gnu99 -ffreestanding -fno-stack-protector -O2    \
	-fno-pic -fomit-frame-pointer -mno-80387 -mno-mmx -mno-3dnow -mno-sse \
	-mno-sse2 -w -o kernel.o
ARFLAGS = -crs library.a *.o
LDFLAGS = -m elf_i386 -T linker.ld kernel.o memset.o boot.o -o $(BIN) -nostdlib 

make:
	make clean
	$(AS) $(ASFLAGS)
	$(CC) $(CLIB)
	$(CC) $(CFLAGS)
	$(LD) $(LDFLAGS)

	rm -rf test.img

	dd if=/dev/zero bs=1M count=0 seek=64 of=test.img
	parted -s test.img mklabel msdos
	parted -s test.img mkpart primary 1 100%
	parted -s test.img set 1 boot on # Workaround for buggy BIOSes

	echfs-utils -m -p0 test.img quick-format 32768
	echfs-utils -m -p0 test.img import $(LIMINE)limine.cfg limine.cfg
	echfs-utils -m -p0 test.img import $(LIMINE)src-stivale/kernel.elf /boot/kernel.elf
	echfs-utils -m -p0 test.img import MyOS.bin /boot/MyOS.bin
	echfs-utils -m -p0 test.img import $(LIMINE)limine/stage2.map /boot/stage2.map
	echfs-utils -m -p0 test.img import $(LIMINE)limine/bg.bmp /boot/bg.bmp

	$(LIMINE)limine/./limine-install $(LIMINE)limine/limine.bin test.img
	
	sudo modprobe kvm_intel
	sudo qemu-system-x86_64  -drive format=raw,file=test.img -debugcon stdio -m 1G -enable-kvm -cpu host

clean:
	rm -rf *.bin
	rm -rf kernel.o
	rm -rf boot.o
