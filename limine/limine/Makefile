CC = cc
CFLAGS = -O2 -pipe -Wall -Wextra
PATH := $(shell pwd)/toolchain/bin:$(PATH)

.PHONY: all clean stage2 stage2-clean decompressor decompressor-clean toolchain test.hdd echfs-test ext2-test fat32-test

all: stage2 decompressor
	gzip -n -9 < stage2/stage2.bin > stage2/stage2.bin.gz
	cd bootsect && nasm bootsect.asm -fbin -o ../limine.bin
	cd pxeboot && nasm bootsect.asm -fbin -o ../limine-pxe.bin
	cp stage2/stage2.map ./

clean: stage2-clean decompressor-clean test-clean
	rm -f stage2/stage2.bin.gz test/stage2.map test.hdd

distclean: clean
	rm -f limine-install

stage2:
	$(MAKE) -C stage2 all

stage2-clean:
	$(MAKE) -C stage2 clean

decompressor:
	$(MAKE) -C decompressor all

decompressor-clean:
	$(MAKE) -C decompressor clean

test-clean:
	$(MAKE) -C test clean

toolchain:
	cd toolchain && ./make_toolchain.sh -j`nproc`

limine-install: limine-install.c
	$(CC) $(CFLAGS) limine-install.c -o limine-install

test.hdd:
	rm -f test.hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=test.hdd
	parted -s test.hdd mklabel gpt
	parted -s test.hdd mkpart primary 2048s 100%

echfs-test: all limine-install test.hdd
	$(MAKE) -C test
	echfs-utils -g -p0 test.hdd quick-format 512 > part_guid
	sed "s/@GUID@/`cat part_guid`/g" < test/limine.cfg > limine.cfg.tmp
	echfs-utils -g -p0 test.hdd import limine.cfg.tmp limine.cfg
	rm -f limine.cfg.tmp part_guid
	echfs-utils -g -p0 test.hdd import stage2.map boot/stage2.map
	echfs-utils -g -p0 test.hdd import test/test.elf boot/test.elf
	echfs-utils -g -p0 test.hdd import test/bg.bmp boot/bg.bmp
	echfs-utils -g -p0 test.hdd import test/font.bin boot/font.bin
	./limine-install limine.bin test.hdd
	qemu-system-x86_64 -net none -smp 4 -enable-kvm -cpu host -hda test.hdd -debugcon stdio

ext2-test: all limine-install test.hdd
	$(MAKE) -C test
	cp stage2.map test/
	rm -rf test_image/
	mkdir test_image
	sudo losetup -Pf --show test.hdd > loopback_dev
	sudo partprobe `cat loopback_dev`
	sudo mkfs.ext2 `cat loopback_dev`p1
	sudo mount `cat loopback_dev`p1 test_image
	sudo mkdir test_image/boot
	sudo cp -rv test/* test_image/boot/
	sync
	sudo umount test_image/
	sudo losetup -d `cat loopback_dev`
	rm -rf test_image loopback_dev
	./limine-install limine.bin test.hdd
	qemu-system-x86_64 -net none -smp 4 -enable-kvm -cpu host -hda test.hdd -debugcon stdio

fat32-test: all limine-install test.hdd
	$(MAKE) -C test
	cp stage2.map test/
	rm -rf test_image/
	mkdir test_image
	sudo losetup -Pf --show test.hdd > loopback_dev
	sudo partprobe `cat loopback_dev`
	sudo mkfs.fat -F 32 `cat loopback_dev`p1
	sudo mount `cat loopback_dev`p1 test_image
	sudo mkdir test_image/boot
	sudo cp -rv test/* test_image/boot/
	sync
	sudo umount test_image/
	sudo losetup -d `cat loopback_dev`
	rm -rf test_image loopback_dev
	./limine-install limine.bin test.hdd
	qemu-system-x86_64 -net none -smp 4 -enable-kvm -cpu host -hda test.hdd -debugcon stdio
