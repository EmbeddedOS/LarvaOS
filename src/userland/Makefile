all:
	nasm -f elf ./helloworld/helloworld.asm -o ./helloworld/helloworld.o
	i686-elf-gcc -h -T ./linker.ld -o ../../bin/userland/helloworld.bin ./helloworld/helloworld.o -g -fpic -nostdlib -ffreestanding -O0

clean:
	rm -rf ../bin/userland/helloworld.o