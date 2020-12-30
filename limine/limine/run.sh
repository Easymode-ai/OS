rm -rf test.img

dd if=/dev/zero bs=1M count=0 seek=64 of=test.img
parted -s test.img mklabel msdos
parted -s test.img mkpart primary 1 100%
parted -s test.img set 1 boot on # Workaround for buggy BIOSes

echfs-utils -m -p0 test.img quick-format 32768
echfs-utils -m -p0 test.img import ../limine.cfg limine.cfg
echfs-utils -m -p0 test.img import ../src-stivale/kernel.elf /boot/kernel.elf
echfs-utils -m -p0 test.img import ../../../OS-Checkout/MyOS.bin /boot/MyOS.bin
echfs-utils -m -p0 test.img import stage2.map /boot/stage2.map
echfs-utils -m -p0 test.img import bg.bmp /boot/bg.bmp

./limine-install limine.bin test.img

export DISPLAY=192.168.0.10:0.0
qemu-system-x86_64  -drive format=raw,file=test.img -debugcon stdio -m 1G

cp test.img ../../../OS-Checkout
