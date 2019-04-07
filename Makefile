
# Makefile for Compiling the Base Kernel
# Requirements are nasm, and a i586 cross compiler
# Directions to download is at http://wiki.osdev.org/GCC_Cross-Compiler_on_Debian_Linux

# These aren't "real" targets
.PHONY:		all clean link

# This can be overridden by a user
BUILD_DIR	?= build

CWARNINGS	:= -Wall -Wextra
CFLAGS		:= -I./src/include -std=gnu99 -fleading-underscore -O -finline-functions-called-once

# In Case we ever wanna make for another platform
TARGET	?= x86

# We need lowercase for the name, just be sure with a `tr`
TARGETL	!= echo $(TARGET) | tr '[:upper:]' '[:lower:]'

BUILD	:= $(BUILD_DIR)-$(TARGETL)

# Include the sub-makefile
-include target-$(TARGETL).mk

# Rewrite paths into the build directory
C_SOURCES	!= find src/core  -type f -name '*.c'
C_SOURCES	+= $(TARGET_CSOURCES)
C_OBJECTS	:= $(patsubst %.c,$(BUILD)/%.c.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst %.s,$(BUILD)/%.s.o,$(ASM_SOURCES))

OBJECTS		:= $(C_OBJECTS) $(ASM_OBJECTS)

# All of the directories we need for the build
SRC_DIR		!= find src/ -type d | tr '\n' ' '

all: $(OBJECTS) link

clean:
	@printf "\033[1mCLEAN\033[0m \n"
	@find $(BUILD) -type f -name '*.o' -exec rm {} +

# $(CC) is a perfectly good linker, no need to use ld
link: $(OBJECTS)
	@printf "\033[1mLINK\033[0m $@\n"
	@$(CC) $(TARGET_CFLAGS) $(CFLAGS) $(CWARNINGS) $(LDFLAGS) -o./bin/$(BIN) $(OBJECTS)

# Set up build directories
$(BUILD)/%.c.o: %.c Makefile | setup_build_dir
	@printf "\033[1mCC\033[0m $@\n"
	@$(CC) $(TARGET_CFLAGS) $(CWARNINGS) $(CFLAGS) -c $< -o $@

setup_build_dir:
	@mkdir -p $(BUILD)
	@cd $(BUILD) && mkdir -p `echo $(SRC_DIR)`
