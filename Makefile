# Scorch OS 2008-2010 Scorch OS Developers
# Makefile for Compiling the Base Kernel
# Requirements are nasm, and a i586 cross compiler
# Directions to download is at http://wiki.osdev.org/GCC_Cross-Compiler_on_Debian_Linux

C_COMPILER=i586-elf-gcc
ASM_COMPILER=nasm
LINKER=i586-elf-ld

CFLAGS=-I./src/include -fleading-underscore -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin
LDFLAGS=-T./src/link.ld
ASFLAGS=-felf

SOURCES=src/start.o src/kernel.o src/scrn.o src/gdt.o src/idt.o src/isrs.o src/irq.o src/timer.o src/kb.o src/shell.o src/common.o src/string.o

all: $(SOURCES) link
clean:
	-rm ./src/*.o
link:
	$(LINKER) $(LDFLAGS) -o bin/kernel.x $(SOURCES)
.c.o:
	$(C_COMPILER) $(CFLAGS) -c $< -o $@

.s.o:
	$(ASM_COMPILER) $(ASFLAGS) $< -o $@
