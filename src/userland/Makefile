all:
	nasm -f elf ./helloworld/helloworld.asm -o ./helloworld/helloworld.o
	i686-elf-gcc -h -T ./linker.ld ./helloworld/helloworld.o -g -fpic -nostdlib -ffreestanding -O0 -o ../../bin/userland/helloworld.elf

clean:
	rm -rf ../bin/userland/helloworld.o