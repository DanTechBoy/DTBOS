# DTBOS
> A basic x86 and ARM operating system

# Planned features
Userland
Filesystem
Multitasking
More commands

# Building
### Build, and run in QEMU
```
$ git clone https://github.com/DanTechBoy/DTBOS.git
$ sudo apt-get install git gcc g++ make m4 bison flex bzip2 xz-utils curl zlib1g-dev gnat python grub-common mtools qemu-system-gui qemu-system-x86 qemu-system-arm ovmf exuberant-ctags u-boot-tools netbpm
$ cd DTBOS
$ make deps
$ make boot
```
### Build for ARM
```
$ make ARCH=arm boot
```
### Build with EFI
```
$ make boot-efi
```