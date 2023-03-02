all:
	nasm -f elf ./loop/loop.asm -o ./loop/loop.o
	i686-elf-gcc -h -T ./linker.ld -o ../../bin/userland/loop.bin ./loop/loop.o -g -fpic -nostdlib -ffreestanding -O0

clean:
	rm -rf ../bin/userland/loop.o