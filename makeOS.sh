#!/bin/sh
cd `dirname $0`
mkdir -p OS
dd if=/dev/zero of=OS/os.img bs=512 count=200 
mkfs.fat OS/os.img 
(
echo n # Add a new partition
echo p # Primary partition
echo 2 # Partition number
echo 5 # First sector (Accept default: 1)
echo 100 # Last sector (Accept default: varies)
echo a
echo w # Write changes
) | fdisk OS/os.img

dd if=boot/boot.bin of=OS/os.img  conv=notrunc 
dd if=kern/vmaim.elf of=OS/os.img seek=5 bs=512 conv=notrunc 
echo "Success:type \"qemu-system-i386 OS/os.img\" to run"