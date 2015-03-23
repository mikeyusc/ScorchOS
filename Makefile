# Scorch OS 2008-2010 Scorch OS Developers
# Makefile for Compiling the Base Kernel
# Requirements are nasm, and a i586 cross compiler
# Directions to download is at http://wiki.osdev.org/GCC_Cross-Compiler_on_Debian_Linux

C_COMPILER=i586-elf-gcc
ASM_COMPILER=nasm
LINKER=i586-elf-ld

CFLAGS=-I./src/include -fleading-underscore -O -ffreestanding -finline-functions-called-once -nostdlib
LDFLAGS=-T./src/link.ld
ASFLAGS=-felf

C_SOURCES = $(shell ls src/*.c)
ASM_SOURCES = $(shell ls src/*.s)
OBJECTS	= $(ASM_SOURCES:.s=.o) $(C_SOURCES:.c=.o)

all: $(OBJECTS) link
clean:
	-rm ./src/*.o
link:
	$(LINKER) $(LDFLAGS) -o bin/kernel.x $(OBJECTS)
.c.o:
	$(C_COMPILER) $(CFLAGS) -c $< -o $@

.s.o:
	$(ASM_COMPILER) $(ASFLAGS) $< -o $@
