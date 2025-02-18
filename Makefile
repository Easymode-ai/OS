
TARGET := kernel2.elf
LIMINE := limine/
AS 				= nasm 
CC              = gcc
CFLAGS          = -O2
LD              = ld -no-pie     -static   -no-pie   -z max-page-size=0x1000                   
QEMU            = qemu-system-x86_64
BLAH = -enable-kvm -cpu host
QEMUFLAGS       = -m 4G  -machine type=q35 -D /var/log.txt -no-shutdown $(BLAH) 

ASFLAGS			= -f elf64
LDINTERNALFLAGS := -Tlinker.ld -nostdlib -no-pie -static
INTERNALCFLAGS  := -I../stivale -I./ -I./kernel -ffreestanding -fno-stack-protector -fno-pic  -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -masm=att -mcmodel=kernel  -mno-red-zone -g   -std=gnu99    -O3 -pipe -w -m64
CFILES    := $(shell find ./ -type f -name '*.c')
OBJ       := ordered_array.o pmm.o vmm.o pci.o display.o command.o timer.o ahci.o echfs.o  kernel.o main.o
LIB_SOURCES = $(wildcard libc/include/*.c libc/string/*.c libc/stdlib/*.c kernel/*.c kernel/fs/*.c)

install:

	make clean
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c kernel.c $(LIB_SOURCES)
	$(AS) $(ASFLAGS) main.asm -o main.o
	$(LD) $(LDINTERNALFLAGS) $(OBJ) -o $(TARGET)
#make

	rm -rf test.img


	dd if=/dev/zero bs=512K count=0 seek=256 of=test.img
	parted -s test.img mklabel msdos
	parted -s test.img mkpart primary 1 100%
	parted -s test.img set 1 boot on # Workaround for buggy BIOSes

	echfs-utils -m -p0 test.img quick-format 512
	echfs-utils -m -p0 test.img import $(LIMINE)limine.cfg /boot/limine.cfg
	echfs-utils -m -p0 test.img import kernel2.elf /boot/kernel2.elf
	echfs-utils -m -p0 test.img import $(LIMINE)limine/bg.bmp /a/bg.bmp
	echfs-utils -m -p0 test.img import $(LIMINE)limine/test3.txt /a/text3.txt
	echfs-utils -m -p0 test.img import $(LIMINE)limine/bg3.bmp /b/bg.bmp
	echfs-utils -m -p0 test.img import $(LIMINE)limine/amiga.bmp wallpaper/amiga.bmp
	echfs-utils -m -p0 test.img import $(LIMINE)limine/liquid.bmp wallpaper/liquid.bmp
	echfs-utils -m -p0 test.img import $(LIMINE)limine/sgi.bmp wallpaper/sgi.bmp
	echfs-utils -m -p0 test.img import $(LIMINE)limine/test.txt /b/text1.txt
	echfs-utils -m -p0 test.img import $(LIMINE)limine/test2.txt /b/text2.txt
	echfs-utils -m -p0 test.img import $(LIMINE)limine/test2.txt /a/b/c/d/text4.txt
	echfs-utils -m -p0 test.img import $(LIMINE)limine/stage2.map /boot/stage2.map
	echfs-utils -m -p0 test.img import apps/hello.elf /hello.elf
	
	
	$(LIMINE)limine/./limine-install $(LIMINE)limine/limine.bin test.img
	
	sudo modprobe kvm_intel
	sudo qemu-system-x86_64 -no-reboot -D /tmp/log.txt test.img  -d in_asm,exec,cpu,int -debugcon stdio $(QEMUFLAGS)
clean:
	rm -rf $(TARGET) *.o

virtualbox:
	rm -rf test.raw
	rm -rf test.vdi
	sudo qemu-img convert test.img -O raw test.raw
	vboxmanage convertdd test.raw test.vdi	
