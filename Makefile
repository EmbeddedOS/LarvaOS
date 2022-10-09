all:
	nasm -f bin src/kernel/arch/x86/boot/boot.asm -o bin/boot.bin

clean:
	rm -rf bin/*.bin
