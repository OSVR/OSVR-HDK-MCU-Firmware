# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2016 Sensics, Inc.
# All rights reserved.

ifeq ($(OS),Windows_NT)
# this is Windows

PATH_SEPARATOR = \
PATH_CONVERT = $(subst /,$(PATH_SEPARATOR),$1)

ifdef ProgramFiles(x86)
ATMEL_STUDIO_ROOT := ${subst \,/,${ProgramFiles(x86)}}/Atmel/Studio/7.0
else
ATMEL_STUDIO_ROOT := $(subst \,/,${ProgramFiles})/Atmel/Studio/7.0
endif
TOOLCHAIN_ROOT := $(ATMEL_STUDIO_ROOT)/toolchain/avr8/avr8-gnu-toolchain
TOOL_EXTENSION := .exe

ifeq ($(strip $(NO_ATMEL_STUDIO)),)
# Can use "shellutils" folder of utilities.
RM := "$(ATMEL_STUDIO_ROOT)/shellutils/rm.exe" -f
# must be recursively evaluated - it's a function
FUNC_MKDIR_P = "$(ATMEL_STUDIO_ROOT)/shellutils/mkdir.exe" -p "$1"
FUNC_CP = "$(ATMEL_STUDIO_ROOT)/shellutils/cp.exe" "$1" "$2"
else

# fall back to cmd...
FUNC_CP := copy
RM := del /q
# must be recursively evaluated - they're functions
FUNC_MKDIR_P = if not exist "$(call PATH_CONVERT,$1)" mkdir "$(call PATH_CONVERT,$1)"
FUNC_CP := copy /Y "$(call PATH_CONVERT,$1)" "$(call PATH_CONVERT,$2)"
endif


else
# not Windows
TOOLCHAIN_ROOT ?= /usr
TOOL_EXTENSION :=
PATH_SEPARATOR = /
PATH_CONVERT = $1
RM := rm -f
FUNC_CP := cp
FUNC_MKDIR_P = mkdir -p "$1"

endif

CC := "$(TOOLCHAIN_ROOT)/bin/avr-gcc$(TOOL_EXTENSION)"
OBJCOPY := "$(TOOLCHAIN_ROOT)/bin/avr-objcopy$(TOOL_EXTENSION)"
OBJDUMP := "$(TOOLCHAIN_ROOT)/bin/avr-objdump$(TOOL_EXTENSION)"
AVRSIZE := "$(TOOLCHAIN_ROOT)/bin/avr-size$(TOOL_EXTENSION)"
