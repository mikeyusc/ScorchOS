# Scorch OS 2008-2019 Scorch OS Developers
# Makefile for Compiling x86 kernel
# Requirements are nasm, and a i586 cross compiler
# Directions to download is at http://wiki.osdev.org/GCC_Cross-Compiler_on_Debian_Linux

CC  := i686-elf-gcc
AS  := nasm

BIN := kernel.x

ASFLAGS         := -felf -I"./src/arch/x86/"
LDFLAGS         := -m32 -T./src/arch/x86/link.ld -nostdlib -lgcc -n
TARGET_CFLAGS   := -ffreestanding -nostdlib
TARGET_CSOURCES != find src/arch/x86 -type f -name '*.c'
ASM_SOURCES     != find src/arch/x86 -type f -name '*.s'

$(BUILD)/%.s.o: %.s | setup_build_dir
	@printf "\033[1mAS\033[0m   $<\n"
	@$(AS) $(ASFLAGS) $< -o $@
