# Modular Make File
# Dr. Brian C. Ladd
# laddbc@potsdam.edu

# Using this =Makefile* (Org mode comment: =...= means code)
#
# This =Makefile= represents an attempt to support both "convention over
# configuration" and "folder-based modules" in standard C++ projects.
#
# Convention
#
# Below the folder containing this =Makefile= lies a =src/=
# folder. That folder contains the modules for this C++ project, each
# in a directory that should be named for the module. The =src/=
# folder also has =allModule.mk=, a file included into this
# =Makefile=.
#
# =allModule.mk= defines two lists: =MODULES= and =BOOT=. =MODULES= is
# a space-separated list of the modules' folder names. =BOOT= is
# similar but only lists modules that should be compiled into flat
# binary files (like a bootloader, hence the name).
#
# Each module must contain a different include file for =make=,
# =module.mk=. There are three (3) versions of =module.mk=:
# tools :: in the executables directory. Each compilable file is
#   compiled/assembled and then linked with all other object files
#   (except those in the tools directory) into an executable program.
# boot :: in the boot loader directory. Each compilable file is
#   compiled/assembled directly into a flat binary file.  The linker
#   is never run.
# default :: for all other modules, compilable files are built into
#   object files and added to the link list for executable files.


# Compiling x86 code on x86 Linux (or similar)
#   CC - the C++ compiler to use
#   ASM - the assembler to use
CC := g++
ASM := nasm

# Directory for compiled object files.
BUILD = build

# Base source directory
SOURCE = src

# CFLAGS - flags for the C compiler
#   -std=c++20 use the C++ 2020 standard
#   -O<n>      set optimization level applied; 0 means no optimization
#              optimization can change behavior of low-level code
#   -Wall      report all possible warnings
#   -Werror    treat any warning as an error and stop the compile
#   -g         include debug information in the .o and executable files
CFLAGS = -std=c++2a -O0 -Wall -Werror -g

# ASMFLAGS - flags for the NASM assembler
#   -fbin    output format flat 16-bit binary (bootloader, DOS-like)
#   -felf64  output Format 64-bit ELF object code
ASMFLAGS = -felf64
boot: ASMFLAGS = -fbin

# LDFLAGS - flags for the linker
#   -nostdlib  do not link the standard run-time library to the program
#   -lstdc++fs link with the library (-l means this) stdc++-fs
#              (the c++17 standard filesystem implementation)
#   -static    link all libraries statically rather than dynamically
LDFLAGS := -lstdc++fs
## FOR RawOS programs: LDFLAGS := -nostdlib

# The information defined in the source directories
#   INCLUDES - directories to search for ".h" (and, for 3rd-party libs, ".hpp") include files
#   SRC - additional source directories
#   BUILDDIRS - required $(BUILD) subdirectories+

INCLUDES := -I include
SRC :=
BUILDDIRS :=
TOOLSRC :=
# Define the important information locally in the source directories
include $(SOURCE)/allModule.mk

# SRC is constructed by the included module make files; modify it by
# replacing .s, .c, and .cpp extensions with .o Notice change to
# manipulated list in second assignment; patsubst passes non-matches
# through unchanged so first map assembly source to object files, then
# map remaining sorce (C source) to object files
OBJ := $(patsubst $(SOURCE)/%.s,$(BUILD)/%.o,$(SRC))
OBJ := $(patsubst $(SOURCE)/%.c,$(BUILD)/%.o,$(OBJ))
OBJ := $(patsubst $(SOURCE)/%.cpp,$(BUILD)/%.o,$(OBJ))

TESTOBJ := $(foreach o, $(OBJ), $(if $(findstring test, $o),$o))

OBJ := $(filter-out $(TESTOBJ), $(OBJ))

EXEC := $(addprefix $(BUILD)/, $(basename $(notdir $(TOOLSRC))))

# Default rule; builds target and tests
all:  $(BUILD) $(EXEC) boot


, := ,

define \n


endef
blank :=
space := $(blank) $(blank)
\t := $(blank)	$(blank)

define executable_rule =
$(addprefix $(BUILD)/, $(basename $(notdir $1))): $(OBJ) $(if $(findstring test, $1), $(TESTOBJ)) $(patsubst $(SOURCE)/%.cpp,$(BUILD)/%.o,$1)
	$$(CC) $$(CFLAGS) -o $$@ $$^ $$(LDFLAGS)
endef

$(foreach t, $(TOOLSRC), $(eval $(call executable_rule, $t)))


# BOOT is the list of files to build from assembler to flat img files
.PHONY: boot
boot: $(BOOT)

$(BOOT): % : $(BUILD)/%.img $(BUILD)

$(BUILD)/%.img: $(SOURCE)/$(BOOT_DIR)/%.s $(BUILD)
	$(ASM) $(ASMFLAGS) $(INCLUDES) $< -o $@

# Rule to make the object files; two rules since there are different
# commands depending on the type of the source code (C or  assembly).
# The directory structure below $(BUILD) must match that below $(SOURCE)
$(BUILD)/%.o: $(SOURCE)/%.s
	$(ASM) $(ASMFLAGS) $(INCLUDES) $< -o $@

$(BUILD)/%.o: $(SOURCE)/%.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# So: the build directory might not exist (see clean); it, and all
# subordinate directories, must be made. Base dir depends on any subdirs
# and is build after them. $(BUILD) is made because $(BUILDDIRS) might
# be empty.
$(BUILD): $(BUILDDIRS)
	@mkdir -p $@

# mkdir -p permits making a deeper than 1 tree AND remaking an existing
# directory. So the previous rule runs fine even if this one runs first
$(BUILDDIRS):
	@mkdir -p $@

# Rule to clean files.
.PHONY:	clean
clean :
	-rm -rf $(BUILD)
