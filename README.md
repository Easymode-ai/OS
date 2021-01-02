# <Give me a name> OS
Feel free to contribute your code suggestions/fixes.

The purpose of this project is to learn how to design an OS and all the hoops involved.

Please abuse me with code submissions and hassle me on discord Mir2Euro#1825

# Current state

- Basic VMM
- Base IDT/GDT/RTS
- Keyboard Interrupt
- Graphics Mode

# Perquisites 

Linux OR compatible (Subsystem on insider program with KVM is possible!)

QEMU with KVM or compatible (must boot img, or you will need to convert)

echfs-utils @ (https://github.com/echfs/echfs) 

# Getting Started
Run make
Run qemu qemu-system-x86_64 -kernel MyOS.bin

# Important 

The makefile assumes that you have Ubuntu subsystem installed, and latest insider program (dev) build with a kvm enabled rebuild of the kernel.

Please modify the makefile to suit your machines needs, I know this is fugly right now maybe we/I can improve that.
