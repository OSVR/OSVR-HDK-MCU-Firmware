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

OPTIMIZATION := -O3

ASFLAGS :=
EXTRA_ASFLAGS :=

# Other useful things you could define on the command line:
# ATMEL_STUDIO_ROOT
# NO_ATMEL_STUDIO (for a Windows avrgcc toolchain without atmel studio - any value is true)
# TOOLCHAIN_ROOT
# VERBOSE (any value is true and causes the commands to not be hidden)

# If the user passed additional DEFINES on the command line, we'll combine those
# in a hopefully-safe way for display and for appending to the output filename.
# Of course, if you change the value of DEFINES, you must rebuild the whole thing.
ifneq ($(strip $(DEFINES)),)
# the bit with eval is to be able to search for a space.
SUFFIX := -$(subst $(eval) ,-,$(subst =,_,$(strip $(DEFINES))))
FN_SUFFIX := $(subst =,_,$(SUFFIX))
endif

LIBS :=

OUTPUT_STEM := HMD_MCU
OUTPUT_FILE_PATH := $(OUTPUT_STEM).elf
OUTPUT_MAP := $(OUTPUT_STEM).map
OUTPUT_HEX := $(OUTPUT_STEM).hex
OUTPUT_LSS := $(OUTPUT_STEM).lss

# Relative path from makefile to source root
REL_ROOT := ..

# Set up the variables that add_variant.mk will populate.
FINAL_GENERATED :=
CONVENIENCE_TARGETS :=
BUILT_BY_ALL :=
VARIANTS :=

SHORT_NAME := hdk2
VARIANT_NAME := HDK_20
SKIP_ALL_TARGET := true
# Per-variant libs and includes
LIBS_HDK_20 := $(REL_ROOT)/vendor/libhdk20/libhdk20.a
INCLUDE_DIRS_HDK_20 := $(REL_ROOT)/vendor/libhdk20/
include add_variant.mk

SHORT_NAME := hdk_oled
VARIANT_NAME := HDK_OLED
include add_variant.mk

SHORT_NAME := hdk_lcd
VARIANT_NAME := HDK_Sharp_LCD
include add_variant.mk

SHORT_NAME := dsight
VARIANT_NAME := dSight_Sharp_LCD
include add_variant.mk

# Done with variants.

MCU=atxmega256a3bu



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
src/DeviceDrivers/Solomon.c \
src/DeviceDrivers/TI-TMDS442.c \
src/DeviceDrivers/Toshiba_TC358870.c \
src/DeviceDrivers/bno-hostif/1000-3251_1.7.0.390_avr.c \
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

# Simple lists without a variant subdirectory at the beginning
BASE_OBJS :=  $(C_SRCS:%.c=%.o) $(PREPROCESSING_SRCS:%.s=%.o)
BASE_C_DEPS := $(C_SRCS:%.c=%.d)

# The above lists, multiplied by the variants.
OBJS := $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(BASE_OBJS)))
C_DEPS := $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(BASE_C_DEPS)))

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
           BOARD=USER_BOARD

ALL_DEFINES := $(BASE_DEFINES) $(DEFINES)

COMMON_FLAGS := -mmcu=$(MCU) -mrelax $(patsubst %, -D%,$(ALL_DEFINES))

ALL_CFLAGS = $(COMMON_FLAGS) -c -std=gnu99 \
              -ffunction-sections -fdata-sections \
              -fpack-struct -fshort-enums -fno-strict-aliasing \
              $(CFLAGS) $(EXTRA_CFLAGS)
ALL_ASFLAGS := -Wa,-gdwarf2 -x assembler-with-cpp -c $(COMMON_FLAGS) $(ASFLAGS) $(EXTRA_ASFLAGS)

for_each_variant = $(foreach variant,$(VARIANTS),$(variant)/$1)
get_variant=$(word 1,$(subst /, ,$1))
set_variant_variable = $(eval VARIANT := $(call get_variant,$1))

make_include_dirs = $(patsubst %, -I"%",$(call variant_include_dir,$1) $(COMMON_INCLUDE_DIRS) $(INCLUDE_DIRS_$(1)))
DEBUG_VARIANT = @echo Variant: $(VARIANT)

ifeq ($(strip $(VERBOSE)),)
QUIETRULE := @
else
$(info Building in verbose mode - all meaningful command lines will be echoed)
endif

# No implicit rules, please.
SUFFIXES :=

include atmel_build.mk

# Pattern for building object files from c
$(call for_each_variant,%.o): $(REL_ROOT)/%.c
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] ($(OPTIMIZATION)) $< : $@
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) $(OPTIMIZATION) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"

# Pattern for building object files from .s
$(call for_each_variant,%.o): $(REL_ROOT)/%.s
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] $< : $@
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_ASFLAGS) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g -o "$@" "$<"

# These files had optimization turned off before for some reason, turning it down now.
$(call for_each_variant,src/DeviceDrivers/bno-hostif/src/sensorhub.o): override OPTIMIZATION := -O1
$(call for_each_variant,src/DeviceDrivers/bno-hostif/BNO070_using_hostif.o): override OPTIMIZATION := -O1

# These files are terribly full of warnings. We really just want to disable the shift overflow ones, but the flag
# below seems insufficient, thus -w disables all warnings...
$(call for_each_variant,src/NXP/tmbslTDA1997X_functions.o): override CFLAGS := -w $(filter-out -Wall,$(CFLAGS)) -Wno-shift-count-overflow
$(call for_each_variant,src/NXP/tmdlHdmiRx.o): override CFLAGS := -w $(filter-out -Wall,$(CFLAGS)) -Wno-shift-count-overflow


# Makes the elf file
$(call for_each_variant,$(OUTPUT_FILE_PATH)): %/$(OUTPUT_FILE_PATH) : $(addprefix %/,$(BASE_OBJS))
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] Linking $@
	$(QUIETRULE)$(CC) -o"$@" $(addprefix $(VARIANT)/,$(BASE_OBJS)) $(LIBS) $(LIBS_$(VARIANT)) -Wl,-Map="$(VARIANT)/$(OUTPUT_MAP)" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=$(MCU) -Wl,--relax -Wl,--section-start=.BOOT=0x40000
	$(QUIETRULE)$(AVRSIZE) --mcu=$(MCU) --format=avr "$@"


# Makes the hex file
$(call for_each_variant,$(OUTPUT_HEX)): %/$(OUTPUT_HEX) : %/$(OUTPUT_FILE_PATH)
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] Generating $@
	$(QUIETRULE)$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "$<" "$@"

# Makes the lss (assembly listing) file
$(call for_each_variant,$(OUTPUT_LSS)): %/$(OUTPUT_LSS) : %/$(OUTPUT_FILE_PATH)
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] Generating $@
	$(QUIETRULE)$(OBJDUMP) -h -S "$<" > "$@"


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
	-$(RM) $(OBJS)
	-$(RM) $(C_DEPS)
	-$(RM) $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(OUTPUT_FILE_PATH) $(OUTPUT_MAP) $(OUTPUT_HEX) $(OUTPUT_LSS)))
.PHONY: clean

distclean: clean
	-$(RM) $(FINAL_GENERATED)
.PHONY: distclean

help:
	@echo make clean will delete build intermediate files.
	@echo make distclean will do make clean and also delete the final output hex files
	@echo ($(FINAL_GENERATED))
	@echo Build targets:
	@echo These targets exist to build variants: $(CONVENIENCE_TARGETS)
	@echo By default (or by make all), this subset is built: $(BUILT_BY_ALL)
.PHONY: help
