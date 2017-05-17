# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2016 Sensics, Inc.
# All rights reserved.

ifeq ($(OS),Windows_NT)
# this is Windows
SPACE := $(eval) $(eval)
PATH_SEPARATOR := $(eval)\$(eval)
PATH_CONVERT = $(subst /,$(PATH_SEPARATOR),$1)
ifndef ATMEL_STUDIO_ROOT
ifdef ProgramFiles(x86)
ATMEL_STUDIO_ROOT := ${ProgramFiles(x86)}/Atmel/Studio/7.0
else
ATMEL_STUDIO_ROOT := ${ProgramFiles}/Atmel/Studio/7.0
endif
$(info Atmel Studio Root: $(ATMEL_STUDIO_ROOT))
# Convert to more unixy path
ATMEL_STUDIO_ROOT_ESCAPED := $(ATMEL_STUDIO_ROOT)
# Standardize slashes
ATMEL_STUDIO_ROOT_ESCAPED := $(subst ${PATH_SEPARATOR},/,${ATMEL_STUDIO_ROOT})
# escape spaces
ATMEL_STUDIO_ROOT_ESCAPED := $(subst $(SPACE),\ ,$(ATMEL_STUDIO_ROOT_ESCAPED))
# escape open parens
ATMEL_STUDIO_ROOT_ESCAPED := ${subst (,\(,${ATMEL_STUDIO_ROOT_ESCAPED}}
# escape close parens
ATMEL_STUDIO_ROOT_ESCAPED := ${subst ),\),${ATMEL_STUDIO_ROOT_ESCAPED}}

#$(info Atmel Studio Root - escaped: $(ATMEL_STUDIO_ROOT_ESCAPED))
endif

ifneq (,$(ATMEL_STUDIO_ROOT))
ATMEL_STUDIO_TOOLCHAIN_ROOT := $(ATMEL_STUDIO_ROOT)/toolchain/avr8/avr8-gnu-toolchain
endif

TOOL_EXTENSION := .exe

ifeq ($(strip $(NO_ATMEL_STUDIO)),)

ifndef TOOLCHAIN_ROOT
TOOLCHAIN_ROOT := $(ATMEL_STUDIO_TOOLCHAIN_ROOT)
endif

# Flags implied by the build system but that libtooling/clang wouldn't know
SYSTEM_FLAGS ?= -isystem "$(ATMEL_STUDIO_TOOLCHAIN_ROOT)/avr/include"

# Can use "shellutils" folder of utilities.
RM := "$(ATMEL_STUDIO_ROOT)/shellutils/rm.exe" -f
TR := "$(ATMEL_STUDIO_ROOT)/shellutils/tr.exe"
CAT := "$(ATMEL_STUDIO_ROOT)/shellutils/cat.exe"
LS := "$(ATMEL_STUDIO_ROOT)/shellutils/ls.exe"
# must be recursively evaluated - it's a function
FUNC_MKDIR_P = "$(ATMEL_STUDIO_ROOT)/shellutils/mkdir.exe" -p "$1"
FUNC_CP = "$(ATMEL_STUDIO_ROOT)/shellutils/cp.exe" "$1" "$2"
else

# fall back to cmd...
FUNC_CP := copy
RM := del /q
# must be recursively expanded - they're functions
FUNC_MKDIR_P = if not exist "$(call PATH_CONVERT,$1)" mkdir "$(call PATH_CONVERT,$1)"
FUNC_CP = copy /Y "$(call PATH_CONVERT,$1)" "$(call PATH_CONVERT,$2)"
endif

FUNC_PRINT_LIST = $(foreach elt,$($1),@echo $2 $(elt)&)


ifneq (,$(ATMEL_STUDIO_ROOT_ESCAPED))
# Figure out if we should be using a "device support pack" and if so, which one.
ifneq (,$(strip $(PACK_NAME)))
PACK_BASE := $(ATMEL_STUDIO_ROOT_ESCAPED)/packs/atmel/$(strip $(PACK_NAME))
AVAIL_PACK_VERS := $(shell cd "$(ATMEL_STUDIO_ROOT)/packs/atmel/$(strip $(PACK_NAME))" && $(LS))
$(info Available vers $(AVAIL_PACK_VERS))

# Get the intersection of what's installed and what is acceptable (in the order of "what's acceptable")
VALID_PACK_VERS := $(filter $(AVAIL_PACK_VERS),$(PACK_VERS))

ifeq (,$(strip $(VALID_PACK_VERS)))
${warning Couldn't find an available pack (versions available $(AVAIL_PACK_VERS)) matching the list in PACK_VERS ($(PACK_VERS)) - building without a pack!}
else
PACK_VER_USING := $(firstword $(VALID_PACK_VERS))
ATMEL_STUDIO_PACK_DIR := $(PACK_BASE)/$(PACK_VER_USING) #$(wildcard $(patsubst %,"$(PACK_BASE)/%",$(PACK_VERS)))
$(info Got pack dir $(ATMEL_STUDIO_PACK_DIR))
endif

endif

endif

ifneq (,$(strip $(ATMEL_STUDIO_PACK_DIR)))
$(info Building using 'pack' at $(ATMEL_STUDIO_PACK_DIR))
PACK_FLAGS = -B "$(ATMEL_STUDIO_PACK_DIR)/gcc/dev/$(MCU)"
endif

else
# not Windows
TOOLCHAIN_ROOT ?= /usr
TOOL_EXTENSION :=
PATH_SEPARATOR := /
PATH_CONVERT = $1
RM := rm -f
TR := tr
CAT := cat
FUNC_CP = cp "$1" "$2"
FUNC_MKDIR_P = mkdir -p "$1"

FUNC_PRINT_LIST = foreach elt in $($1); do echo $2 $$elt; done
endif

CC := "$(TOOLCHAIN_ROOT)/bin/avr-gcc$(TOOL_EXTENSION)"
OBJCOPY := "$(TOOLCHAIN_ROOT)/bin/avr-objcopy$(TOOL_EXTENSION)"
OBJDUMP := "$(TOOLCHAIN_ROOT)/bin/avr-objdump$(TOOL_EXTENSION)"
AVRSIZE := "$(TOOLCHAIN_ROOT)/bin/avr-size$(TOOL_EXTENSION)"
