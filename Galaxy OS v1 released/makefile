all: kernel.bin

kernel.bin: zinux.o super_kernel.o
	ld -m elf_i386 -T linker.ld zinux.o super_kernel.o -o kernel.bin

zinux.o: zinux.asm
	nasm -f elf32 zinux.asm -o zinux.o

super_kernel.o: super_kernel.c
	gcc -m32 -ffreestanding -c super_kernel.c -o super_kernel.o

clean:
	rm -f *.o kernel.bin