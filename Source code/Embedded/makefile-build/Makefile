# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2016 Sensics, Inc.
# All rights reserved.

all:

DEFINES :=

CFLAGS := -g3 -Wall \
          -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith \
          -fdiagnostics-show-option
# -Wstrict-prototypes
EXTRA_CFLAGS :=

OPTIMIZATION := 3

ASFLAGS :=
EXTRA_ASFLAGS :=

# Optional config file.
-include config.mk

# Other useful things you could define on the command line:
# ATMEL_STUDIO_ROOT
# NO_ATMEL_STUDIO (for a Windows avrgcc toolchain without atmel studio - any value is true)
# TOOLCHAIN_ROOT
# VERBOSE (any value is true and causes the commands to not be hidden)

# If the user passed additional DEFINES on the command line, we'll combine those
# in a hopefully-safe way for display and for appending to the output filename.
# Of course, if you change the value of DEFINES, you must rebuild the whole thing.
ifneq ($(strip $(DEFINES)),)
    # the bit with eval is just a dummy, to be able to search for a space.
    # This swaps spaces for dashes.
    CONFIG_DEFINES := $(subst $(eval) ,-,$(subst =,_,$(strip $(DEFINES))))
    # This swaps equals signs for underscores so the result is (hopefully) safe for filename usage.
    FN_SAFE_CONFIG_DEFINES := $(subst =,_,$(CONFIG_DEFINES))

    # This is a human-readable string for use in messages
    CONFIG_DEFINES_HUMAN := (Config: $(CONFIG_DEFINES))

    # This prepends a dash to CONFIG_DEFINES, for display usage.
    SUFFIX := -$(CONFIG_DEFINES)
    # This prepends a dash to FN_SAFE_CONFIG_DEFINES, for output filename usage.
    FN_SUFFIX := -$(FN_SAFE_CONFIG_DEFINES)
endif

LIBS :=

OUTPUT_STEM := HMD_MCU
OUTPUT_FILE_PATH := $(OUTPUT_STEM).elf
OUTPUT_MAP := $(OUTPUT_STEM).map
OUTPUT_HEX := $(OUTPUT_STEM).hex
OUTPUT_LSS := $(OUTPUT_STEM).lss

# Relative path from makefile to source root
REL_ROOT := ..

C_SRCS :=  \
src/Boot.c \
src/Console.c \
src/FPGA.c \
src/SerialStateMachine.c \
src/SvrYield.c \
src/TimingDebug.c \
src/USB.c \
src/main.c \
src/my_hardware.c \
src/SideBySide.c \
src/Revision.c \
src/uart_xmega.c \
src/NXP/AVRHDMI.c \
src/NXP/NXP_AVR_Internal.c \
src/NXP/i2c.c \
src/NXP/tmbslTDA1997X_functions.c \
src/NXP/tmdlHdmiRx.c \
src/NXP/tmdlHdmiRx_IW_RTX.c \
src/NXP/tmdlHdmiRx_cfg.c \
src/DeviceDrivers/Display.c \
src/DeviceDrivers/Display_Solomon.c \
src/DeviceDrivers/Display_DualAUO_TC358870.c \
src/DeviceDrivers/VideoInput.c \
src/DeviceDrivers/VideoInput_FPGALockPin.c \
src/DeviceDrivers/VideoInput_Single_NXP.c \
src/DeviceDrivers/VideoInput_TMDS422_NXP.c \
src/DeviceDrivers/VideoInput_Toshiba_TC358870.c \
src/DeviceDrivers/BNO070_using_hostif.c \
src/DeviceDrivers/HDK2.c \
src/DeviceDrivers/Solomon.c \
src/DeviceDrivers/Solomon_SSD2828.c \
src/DeviceDrivers/TI-TMDS442.c \
src/DeviceDrivers/Toshiba_TC358870.c \
src/DeviceDrivers/bno-hostif/bno_callbacks.c \
src/DeviceDrivers/bno-hostif/src/sensorhub.c \
src/DeviceDrivers/bno-hostif/src/sensorhub_hid.c \
_StackInstrument.c \
src/ASF/common/services/clock/xmega/sysclk.c \
src/ASF/common/services/ioport/xmega/ioport_compat.c \
src/ASF/common/services/serial/usart_serial.c \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.c \
src/ASF/common/services/spi/xmega_spi/spi_master.c \
src/ASF/common/services/spi/xmega_usart_spi/usart_spi.c \
src/ASF/common/services/usb/class/cdc/device/example/atxmega256a3bu_xmega_a3bu_xplained/ui.c \
src/ASF/common/services/usb/class/cdc/device/udi_cdc.c \
src/ASF/common/services/usb/class/composite/device/udi_composite_desc.c \
src/ASF/common/services/usb/class/hid/device/generic/udi_hid_generic.c \
src/ASF/common/services/usb/class/hid/device/udi_hid.c \
src/ASF/common/services/usb/udc/udc.c \
src/ASF/xmega/boards/xmega_a3bu_xplained/init.c \
src/ASF/xmega/drivers/nvm/nvm.c \
src/ASF/xmega/drivers/rtc32/rtc32.c \
src/ASF/xmega/drivers/spi/spi.c \
src/ASF/xmega/drivers/tc/tc.c \
src/ASF/xmega/drivers/twi/twim.c \
src/ASF/xmega/drivers/twi/twis.c \
src/ASF/xmega/drivers/usart/usart.c \
src/ASF/xmega/drivers/usb/usb_device.c \
src/ASF/xmega/services/pwm/pwm.c \
src/ASF/xmega/services/timeout/timeout.c

PREPROCESSING_SRCS :=  \
src/ASF/xmega/drivers/cpu/ccp.s \
src/ASF/xmega/drivers/nvm/nvm_asm.s

variant_include_dir=$(REL_ROOT)/src/Variants/$1

# These are the include dirs that come after the variant include dir.
COMMON_INCLUDE_DIRS = \
	$(REL_ROOT)/src/ASF/xmega/boards/xmega_a3bu_xplained \
	$(REL_ROOT)/src \
	$(REL_ROOT)/src/config \
	$(REL_ROOT)/src/ASF/xmega/boards \
	$(REL_ROOT)/src/ASF/common/boards \
	$(REL_ROOT)/src/ASF/xmega/drivers/cpu \
	$(REL_ROOT)/src/ASF/xmega/drivers/nvm \
	$(REL_ROOT)/src/ASF/xmega/drivers/pmic \
	$(REL_ROOT)/src/ASF/xmega/drivers/sleep \
	$(REL_ROOT)/src/ASF/xmega/drivers/spi \
	$(REL_ROOT)/src/ASF/common/services/clock \
	$(REL_ROOT)/src/ASF/common/services/delay \
	$(REL_ROOT)/src/ASF/common/services/gpio \
	$(REL_ROOT)/src/ASF/common/services/ioport \
	$(REL_ROOT)/src/ASF/common/services/sleepmgr \
	$(REL_ROOT)/src/ASF/common/services/spi/xmega_spi \
	$(REL_ROOT)/src/ASF/common/services/spi \
	$(REL_ROOT)/src/ASF/common/services/twi \
	$(REL_ROOT)/src/ASF/common/services/usb \
	$(REL_ROOT)/src/ASF/common/services/usb/class/cdc \
	$(REL_ROOT)/src/ASF/common/services/usb/class/cdc/device \
	$(REL_ROOT)/src/ASF/common/services/usb/udc \
	$(REL_ROOT)/src/ASF/common/utils \
	$(REL_ROOT)/src/ASF/xmega/drivers/twi \
	$(REL_ROOT)/src/ASF/xmega/drivers/usart \
	$(REL_ROOT)/src/ASF/xmega/drivers/usb \
	$(REL_ROOT)/src/ASF/xmega/utils/preprocessor \
	$(REL_ROOT)/src/ASF/xmega/utils \
	$(REL_ROOT)/src/ASF/common/services/serial/xmega_usart \
	$(REL_ROOT)/src/ASF/common/services/serial \
	$(REL_ROOT)/src/ASF/xmega/drivers/rtc \
	$(REL_ROOT)/src/ASF/xmega/drivers/tc \
	$(REL_ROOT)/src/ASF/xmega/services/pwm \
	$(REL_ROOT)/src/ASF/xmega/drivers/rtc32 \
	$(REL_ROOT)/src/ASF/xmega/services/timeout \
	$(REL_ROOT)/src/ASF/common/services/usb/class/composite/device \
	$(REL_ROOT)/src/ASF/common/services/usb/class/hid \
	$(REL_ROOT)/src/ASF/common/services/usb/class/hid/device \
	$(REL_ROOT)/src/ASF/common/services/usb/class/hid/device/generic \
	$(REL_ROOT)/src/DeviceDrivers/bno-hostif/src

BASE_DEFINES := CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA \
           IOPORT_XMEGA_COMPAT \
           DEBUG \
           BOARD=USER_BOARD \
           SVR_HMDMCU_MAKEFILE

ALL_DEFINES := $(BASE_DEFINES) $(DEFINES)

COMMON_FLAGS = -mmcu=$(MCU) -mrelax $(patsubst %, -D%,$(ALL_DEFINES))

BASE_LDFLAGS = $(COMMON_FLAGS) -Wl,--relax -Wl,--gc-sections

#LDFLAGS := -Wl,--section-start=.BOOT=0x40000

# Not defining a location for .BOOT because we shouldn't need to keep the sections of ASF that assign
# themselves to it (the flash writing routines) and if we put stuff at that offset, we risk destroying the
# bootloader.
LDFLAGS :=

ALL_LDFLAGS = $(BASE_LDFLAGS) $(LDFLAGS)

ALL_CFLAGS = $(COMMON_FLAGS) -c -std=gnu99 \
              -ffunction-sections -fdata-sections \
              -fpack-struct -fshort-enums -fno-strict-aliasing \
              $(CFLAGS) $(EXTRA_CFLAGS)
ALL_ASFLAGS = -Wa,-gdwarf2 -x assembler-with-cpp -c $(COMMON_FLAGS) $(ASFLAGS) $(EXTRA_ASFLAGS)

ifeq (,$(strip $(DEFINES)))
    # no separator if no special defines
    VARIANT_SUFFIX_SEP :=
    get_variant = $(firstword $(subst /, ,$1))
else
    # Special defines produce a special build dir so we don't mix object files between configs.
    # Has to be parseable, though, so we can still extract the variant name from it.
    VARIANT_SUFFIX_SEP := --
    get_variant = $(firstword $(subst $(VARIANT_SUFFIX_SEP), ,$(firstword $(subst /, ,$1))))
endif

# For when you can't set VARIANT but can use call.
make_build_dir = $1$(VARIANT_SUFFIX_SEP)$(FN_SAFE_CONFIG_DEFINES)

# Recursively evaluated: as long as you have VARIANT defined correctly, this will always be the right build directory.
#BUILD_DIR = $(VARIANT)$(VARIANT_SUFFIX_SEP)$(FN_SAFE_CONFIG_DEFINES)
BUILD_DIR = $(call make_build_dir,$(VARIANT))

isCFamilyFile = $(filter .c .cpp .h,$(suffix $1))

# Commands common to all canned recipe beginnings.
# Ensure that the target's directory exists.
define CANNED_RECIPE_COMMON
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
endef

# Show the variant and suffix, and if the input is a c-family file, show
# optimization level, all followed by showing the input and output.
# Then common portion.
define CANNED_RECIPE_BEGINNING_SHOW_IN_AND_OUT
	@echo [$(VARIANT)$(SUFFIX)]$(if $(call isCFamilyFile,$<),[-O$(strip $(OPTIMIZATION))]) $< : $@
	$(CANNED_RECIPE_COMMON)
endef

# Show the variant and suffix, then just show the output, with either
# "Generating", or, if you have target/pattern specific variable
# "DISPLAY_OP" set, its contents, preceding it
# Then common portion.
define CANNED_RECIPE_BEGINNING_SHOW_OUT
	@echo [$(VARIANT)$(SUFFIX)] $(if $(DISPLAY_OP),$(strip $(DISPLAY_OP)),Generating) $@
	$(CANNED_RECIPE_COMMON)
endef


# Prefixes the build dir on everything that's passed.
make_build_dirs = $(foreach VARIANT,$(VARIANTS),$(addprefix $(BUILD_DIR)/,$1))
set_variant_variable = $(eval VARIANT := $(call get_variant,$1))

make_include_dirs = $(patsubst %, -I"%",$(call variant_include_dir,$1) $(COMMON_INCLUDE_DIRS) $(INCLUDE_DIRS_$(1)))
DEBUG_VARIANT = @echo Variant: $(VARIANT)

ifeq ($(strip $(VERBOSE)),)
    QUIETRULE := @
else
    $(info Building in verbose mode - all meaningful command lines will be echoed)
endif

# No implicit rules, please. We'll define them all and we'd like our performance back.
.SUFFIXES:
SUFFIXES :=
ifeq ($(filter -r,$(MAKEFLAGS)),)
    MAKEFLAGS += -r
endif

include atmel_build.mk

# Lists to be populated
FINAL_GENERATED :=
CONVENIENCE_TARGETS :=
BUILT_BY_ALL :=
VARIANTS :=
CONFIG_SHORT_NAMES :=
CONFIG_CLEAN_TARGETS :=
CONFIG_REALCLEAN_TARGETS :=
SPECIAL_CONFIG_TARGETS :=
EXTRA_CLEAN_FILES :=
# recursively-expanded variables required by add_config.mk
CONFIG_CLEAN_TARGET = $(CONFIG)_clean
CONFIG_REALCLEAN_TARGET = $(CONFIG)_realclean

###
# The Variants
###
MCU:=atxmega256a3bu
# Used for compilation databases, for code analysis tools
MIXED_CASE_MCU:=ATxmega256A3BU

SHORT_NAME := hdk2
VARIANT_NAME := HDK_20
include add_variant.mk

SHORT_NAME := hdk2svr
VARIANT_NAME := HDK_20_SVR
include add_variant.mk

SHORT_NAME := hdk_oled
VARIANT_NAME := HDK_OLED
include add_variant.mk

SHORT_NAME := hdk_lcd
VARIANT_NAME := HDK_Sharp_LCD
include add_variant.mk

SHORT_NAME := dsight
VARIANT_NAME := dSight_Sharp_LCD
MCU:=atxmega256a3u
MIXED_CASE_MCU:=ATxmega256A3U
include add_variant.mk

# Done with variants.
# Simple lists without a variant subdirectory at the beginning
BASE_C_OBJS := $(C_SRCS:%.c=%.o)
BASE_S_OBJS := $(PREPROCESSING_SRCS:%.s=%.o)
BASE_OBJS := $(BASE_C_OBJS) $(BASE_S_OBJS)
BASE_C_DEPS := $(C_SRCS:%.c=%.d)
BASE_C_PREPROCS := $(C_SRCS:c=i)

# The above lists, multiplied by the variants.
OBJS := $(call make_build_dirs,$(BASE_OBJS)))
C_OBJS := $(call make_build_dirs,$(BASE_C_OBJS)))
C_DEPS := $(call make_build_dirs,$(BASE_C_DEPS)))
C_PREPROCS := $(call make_build_dirs,$(BASE_C_PREPROCS)))

# These files had optimization turned off before for some reason, turning it down now.
# TODO why does optimization need to be reduced for this BNO code?
$(call make_build_dirs,src/DeviceDrivers/bno-hostif/src/sensorhub.o): OPTIMIZATION := 1
$(call make_build_dirs,src/DeviceDrivers/bno-hostif/BNO070_using_hostif.o): OPTIMIZATION := 1

include git-revision.mk
#$(info GIT_CFLAGS $(GIT_CFLAGS))
# Force rebuild of recursive targets if git changes.
$(SPECIAL_CONFIG_TARGETS): gitdefheader
$(call make_build_dirs,src/Revision.o): gitdefheader
$(call make_build_dirs,src/Revision.o): override CFLAGS += -I.

# Makes the hex file
$(call make_build_dirs,$(OUTPUT_HEX)): %/$(OUTPUT_HEX) : %/$(OUTPUT_FILE_PATH)
	$(CANNED_RECIPE_BEGINNING_SHOW_OUT)
	$(QUIETRULE)$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "$<" "$@"
$(call make_build_dirs,$(OUTPUT_HEX)) : DISPLAY_OP := Reading full .elf file to extract

# Makes the lss (assembly listing) file
# 	$(call set_variant_variable,$@)
$(call make_build_dirs,$(OUTPUT_LSS)): %/$(OUTPUT_LSS) : %/$(OUTPUT_FILE_PATH)
	$(CANNED_RECIPE_BEGINNING_SHOW_OUT)
	$(QUIETRULE)$(OBJDUMP) -h -S "$<" > "$@"
%.lss : DISPLAY_OP := Extracting assembly listing

# Helper target to build all the compilation databases.
.PHONY: compiledb
compiledb: $(call make_build_dirs,$(COMPILATION_DATABASE_FILENAME))
all: compiledb

# Includes the deps files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "HMD-MCU.elf" "HMD-MCU.hex"
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "HMD-MCU.elf" "HMD-MCU.eep" || exit 0
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "HMD-MCU.elf" > "HMD-MCU.lss"
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .user_signatures --set-section-flags=.user_signatures=alloc,load --change-section-lma .user_signatures=0 --no-change-warnings -O ihex "HMD-MCU.elf" "HMD-MCU.usersignatures" || exit 0
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "HMD-MCU.elf"

clean:
	@echo [clean$(SUFFIX)] Removing object files
	$(QUIETRULE)-$(RM) $(OBJS)
	@echo [clean$(SUFFIX)] Removing preprocessor files
	$(QUIETRULE)-$(RM) $(C_PREPROCS)
	@echo [clean$(SUFFIX)] Removing generated dependency makefiles
	$(QUIETRULE)-$(RM) $(C_DEPS)
	@echo [clean$(SUFFIX)] Removing linker and related outputs.
	$(QUIETRULE)-$(RM) $(call make_build_dirs,$(OUTPUT_FILE_PATH) $(OUTPUT_MAP) $(OUTPUT_HEX) $(OUTPUT_LSS))
	@echo [clean$(SUFFIX)] Removing other files.
	$(QUIETRULE)-$(RM) $(EXTRA_CLEAN_FILES)
.PHONY: clean

realclean: clean
	@echo [realclean$(SUFFIX)] Removing renamed copies of final .hex files.
	$(QUIETRULE)-$(RM) $(FINAL_GENERATED)
.PHONY: realclean

# A special target that's typically only called with no DEFINES contents.
ifeq (,$(strip $(DEFINES)))
distclean: realclean
	@echo [distclean$(SUFFIX)] Recursing to realclean known configs using these targets: $(CONFIG_REALCLEAN_TARGETS)
	$(QUIETRULE)-$(MAKE) $(CONFIG_REALCLEAN_TARGETS)
.PHONY: distclean
endif

# A target for use at tagged versions
ifeq (,$(strip $(DEFINES)))
release:
	$(QUIETRULE)-$(MAKE) distclean
	$(QUIETRULE)-$(MAKE) -f bundled.mk
.PHONY: release
endif

# A target for use at commits that aren't tagged versions
snapshot:
	$(QUIETRULE)-$(MAKE) -f bundled.mk SNAPSHOT=1
.PHONY: snapshot

# like all, except also including all of the known configs as well.
complete: all $(SPECIAL_CONFIG_TARGETS)

help:
	@echo make clean will delete build intermediate files.
	@echo make realclean includes make clean, and also deletes the final output hex files
	@echo ($(strip $(FINAL_GENERATED)))
	@echo make distclean is make realclean for both "standard" configuration and the special configurations listed below.
	@echo Build targets:
	@echo These targets exist to build variants:
	$(call FUNC_PRINT_LIST,CONVENIENCE_TARGETS, - )
	@echo By default (or by make all), this subset is built:
	$(call FUNC_PRINT_LIST,BUILT_BY_ALL, - )
	@echo make hdmi_verbose will build targets with DEFINES=HDMI_VERBOSE - make hdmi_verbose_clean is its clean counterpart.
	@echo make disable_video will build targets with DEFINES=SVR_DISABLE_VIDEO_INPUT - make disable_video_clean is its clean counterpart.
	@echo Special config targets:
	$(call FUNC_PRINT_LIST,SPECIAL_CONFIG_TARGETS, - )
	@echo The extreme target "complete" will build a normal make all, as well as all those special config targets.
	@echo make compiledb will generate compile_commands.json in each variant directory for use with Clang-related tools (libtooling-based).
	@echo make release when at a release tag in a clean souce tree will distclean then build a subdirectory containing all the files, named appropriately, needed for release.
	@echo make snapshot is like make release, except that it functions at any commit in a clean source tree and does not distclean first.
.PHONY: help
