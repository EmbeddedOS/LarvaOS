#!/bin/bash

# Set up environment variables.
export PREFIX="$(pwd)/bin/cross/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

OS_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

[ ! -d $OS_DIR/build ] && mkdir $OS_DIR/build

cd $OS_DIR

make clean
make all

sudo mount -t vfat ./bin/LavaOS.img /mnt/d/
cp ./data.txt /mnt/d/
cp ./bin/userland/loop.bin /mnt/d/
sudo umount /mnt/d/
qemu-system-i386 -hda ./bin/LavaOS.img
