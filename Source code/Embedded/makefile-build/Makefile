
all: hdk_oled hdk_lcd dsight

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

VARIANTS := \
    HDK_20 \
    HDK_OLED \
    HDK_Sharp_LCD \
    dSight_Sharp_LCD

LIBS :=

OUTPUT_STEM := HMD_MCU
OUTPUT_FILE_PATH := $(OUTPUT_STEM).elf
OUTPUT_MAP := $(OUTPUT_STEM).map
OUTPUT_HEX := $(OUTPUT_STEM).hex

MCU=atxmega256a3bu
REL_ROOT := ..
REL_SRC := $(REL_ROOT)/src


C_SRCS :=  \
src/Boot.c \
src/Console.c \
src/FPGA.c \
src/SerialStateMachine.c \
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
src/DeviceDrivers/BNO070_using_hostif.c \
src/DeviceDrivers/Solomon.c \
src/DeviceDrivers/TI-TMDS442.c \
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

# Per-variant additional includes and libs
LIBS_HDK_20 := $(REL_ROOT)/vendor/libhdk20/libhdk20.a
INCLUDE_DIRS_HDK_20 := $(REL_ROOT)/vendor/libhdk20/


# Simple lists without a variant subdirectory at the beginning
BASE_OBJS :=  $(C_SRCS:%.c=%.o) $(PREPROCESSING_SRCS:%.s=%.o)
BASE_C_DEPS := $(C_SRCS:%.c=%.d)

# The above lists, multiplied by the variants.
OBJS := $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(BASE_OBJS)))
C_DEPS := $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(BASE_C_DEPS)))

# These are the include dirs that come after the variant include dir.
COMMON_INCLUDE_DIRS = \
	$(REL_SRC)/ASF/xmega/boards/xmega_a3bu_xplained \
	$(REL_SRC) \
	$(REL_SRC)/config \
	$(REL_SRC)/ASF/xmega/boards \
	$(REL_SRC)/ASF/common/boards \
	$(REL_SRC)/ASF/xmega/drivers/cpu \
	$(REL_SRC)/ASF/xmega/drivers/nvm \
	$(REL_SRC)/ASF/xmega/drivers/pmic \
	$(REL_SRC)/ASF/xmega/drivers/sleep \
	$(REL_SRC)/ASF/xmega/drivers/spi \
	$(REL_SRC)/ASF/common/services/clock \
	$(REL_SRC)/ASF/common/services/delay \
	$(REL_SRC)/ASF/common/services/gpio \
	$(REL_SRC)/ASF/common/services/ioport \
	$(REL_SRC)/ASF/common/services/sleepmgr \
	$(REL_SRC)/ASF/common/services/spi/xmega_spi \
	$(REL_SRC)/ASF/common/services/spi \
	$(REL_SRC)/ASF/common/services/twi \
	$(REL_SRC)/ASF/common/services/usb \
	$(REL_SRC)/ASF/common/services/usb/class/cdc \
	$(REL_SRC)/ASF/common/services/usb/class/cdc/device \
	$(REL_SRC)/ASF/common/services/usb/udc \
	$(REL_SRC)/ASF/common/utils \
	$(REL_SRC)/ASF/xmega/drivers/twi \
	$(REL_SRC)/ASF/xmega/drivers/usart \
	$(REL_SRC)/ASF/xmega/drivers/usb \
	$(REL_SRC)/ASF/xmega/utils/preprocessor \
	$(REL_SRC)/ASF/xmega/utils \
	$(REL_SRC)/ASF/common/services/serial/xmega_usart \
	$(REL_SRC)/ASF/common/services/serial \
	$(REL_SRC)/ASF/xmega/drivers/rtc \
	$(REL_SRC)/ASF/xmega/drivers/tc \
	$(REL_SRC)/ASF/xmega/services/pwm \
	$(REL_SRC)/ASF/xmega/drivers/rtc32 \
	$(REL_SRC)/ASF/xmega/services/timeout \
	$(REL_SRC)/ASF/common/services/usb/class/composite/device \
	$(REL_SRC)/ASF/common/services/usb/class/hid \
	$(REL_SRC)/ASF/common/services/usb/class/hid/device \
	$(REL_SRC)/ASF/common/services/usb/class/hid/device/generic \
	$(REL_SRC)/DeviceDrivers/bno-hostif/src

BASE_DEFINES := CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA \
           IOPORT_XMEGA_COMPAT \
           DEBUG \
           BOARD=USER_BOARD

ALL_DEFINES := $(BASE_DEFINES) $(DEFINES)

COMMON_FLAGS := -mmcu=$(MCU) -mrelax $(patsubst %, -D%,$(ALL_DEFINES))

ALL_CFLAGS = -x c $(COMMON_FLAGS) -c -std=gnu99 \
              -ffunction-sections -fdata-sections \
              -fpack-struct -fshort-enums -fno-strict-aliasing \
              $(CFLAGS) $(EXTRA_CFLAGS)
ALL_ASFLAGS := -Wa,-gdwarf2 -x assembler-with-cpp -c $(COMMON_FLAGS) $(ASFLAGS) $(EXTRA_ASFLAGS)

for_each_variant = $(foreach variant,$(VARIANTS),$(variant)/$1)
get_variant=$(word 1,$(subst /, ,$1))

make_include_dirs = $(patsubst %, -I"%",$(REL_SRC)/Variants/$1 $(COMMON_INCLUDE_DIRS) $(INCLUDE_DIRS_$(1)))
DEBUG_VARIANT = @echo Variant: $(VARIANT)

ifeq ($(strip $(VERBOSE)),)
QUIETRULE := @
else
$(info Building in verbose mode - all meaningful command lines will be echoed)
endif


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
CP := "$(ATMEL_STUDIO_ROOT)/shellutils/cp.exe"
# must be recursively evaluated - it's a function
MKDIR_P = "$(ATMEL_STUDIO_ROOT)/shellutils/mkdir.exe" -p "$1"
else

# fall back to cmd...
CP := copy
RM := del /q
# must be recursively evaluated - it's a function
MKDIR_P = if not exist "$(call PATH_CONVERT,$1)" mkdir "$(call PATH_CONVERT,$1)"
endif


else
# not Windows
TOOLCHAIN_ROOT ?= /usr
TOOL_EXTENSION :=
PATH_SEPARATOR = /
PATH_CONVERT = $1
RM := rm -f
CP := cp
MKDIR_P = mkdir -p "$1"

endif

CC := "$(TOOLCHAIN_ROOT)/bin/avr-gcc$(TOOL_EXTENSION)"
OBJCOPY := "$(TOOLCHAIN_ROOT)/bin/avr-objcopy$(TOOL_EXTENSION)"
OBJDUMP := "$(TOOLCHAIN_ROOT)/bin/avr-objdump$(TOOL_EXTENSION)"
AVRSIZE := "$(TOOLCHAIN_ROOT)/bin/avr-size$(TOOL_EXTENSION)"

# No implicit rules, please.
SUFFIXES :=

#GET_PATH = $(subst /,$(call PATH_CONVERT,$1),$(dir $1))

# Pattern for building object files from c
$(call for_each_variant,%.o): $(REL_ROOT)/%.c
	$(eval VARIANT?=$(call get_variant,$@))
	@echo [$(VARIANT)$(SUFFIX)] ($(OPTIMIZATION)) $< : $@
	$(QUIETRULE)$(call MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) $(OPTIMIZATION) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"

# Pattern for building object files from .s
$(call for_each_variant,%.o): $(REL_ROOT)/%.s
	$(eval VARIANT?=$(call get_variant,$@))
	@echo [$(VARIANT)$(SUFFIX)] $< : $@
	$(QUIETRULE)$(call MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_ASFLAGS) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g -o "$@" "$<"

# These files needs optimization turned off (set to -O0) - hopefully this will do it.
$(call for_each_variant,src/DeviceDrivers/bno-hostif/src/sensorhub.o): override OPTIMIZATION := -O0
$(call for_each_variant,src/DeviceDrivers/bno-hostif/BNO070_using_hostif.o): override OPTIMIZATION := -O0

# These files are terribly full of warnings. We really just want to disable the shift overflow ones, but the flag
# below seems insufficient, thus -w disables all warnings...
$(call for_each_variant,src/NXP/tmbslTDA1997X_functions.o): override CFLAGS := -w $(filter-out -Wall,$(CFLAGS)) -Wno-shift-count-overflow
$(call for_each_variant,src/NXP/tmdlHdmiRx.o): override CFLAGS := -w $(filter-out -Wall,$(CFLAGS)) -Wno-shift-count-overflow


# Makes the elf file
$(call for_each_variant,$(OUTPUT_FILE_PATH)): %/$(OUTPUT_FILE_PATH) : $(addprefix %/,$(BASE_OBJS))
	$(eval VARIANT?=$(call get_variant,$@))
	@echo [$(VARIANT)$(SUFFIX)] Linking $@
	$(QUIETRULE)$(CC) -o"$@" $(addprefix $(VARIANT)/,$(BASE_OBJS)) $(LIBS) $(LIBS_$(VARIANT)) -Wl,-Map="$(VARIANT)/$(OUTPUT_MAP)" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=$(MCU) -Wl,--relax -Wl,--section-start=.BOOT=0x40000
	$(QUIETRULE)$(AVRSIZE) --mcu=$(MCU) --format=avr "$@"


# Makes the hex file
$(call for_each_variant,$(OUTPUT_HEX)): %/$(OUTPUT_HEX) : %/$(OUTPUT_FILE_PATH)
	@echo [$(VARIANT)$(SUFFIX)] Generating $@
	$(QUIETRULE)$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "$<" "$@"

# Includes the deps files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

define VARIANT_TOPLEVEL
	$(QUIETRULE)$(CP) "$(call PATH_CONVERT,$<)" "$@$(FN_SUFFIX).hex"
	@echo Generated "$@$(FN_SUFFIX).hex"
$(eval FINAL_GENERATED += $1$(FN_SUFFIX).hex)
endef

#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "HMD-MCU.elf" "HMD-MCU.hex"
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "HMD-MCU.elf" "HMD-MCU.eep" || exit 0
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "HMD-MCU.elf" > "HMD-MCU.lss"
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .user_signatures --set-section-flags=.user_signatures=alloc,load --change-section-lma .user_signatures=0 --no-change-warnings -O ihex "HMD-MCU.elf" "HMD-MCU.usersignatures" || exit 0
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "HMD-MCU.elf"

FINAL_GENERATED :=

hdk20: HDK_20/$(OUTPUT_HEX)
$(call VARIANT_TOPLEVEL,hdk20,HDK_20)
hdk_oled: HDK_OLED/$(OUTPUT_HEX)
$(call VARIANT_TOPLEVEL,hdk_oled,HDK_OLED)
hdk_lcd: HDK_Sharp_LCD/$(OUTPUT_HEX)
$(call VARIANT_TOPLEVEL,hdk_lcd,HDK_Sharp_LCD)
dsight: dSight_Sharp_LCD/$(OUTPUT_HEX)
$(call VARIANT_TOPLEVEL,dsight,dSight_Sharp_LCD)


#	-$(foreach variant,$(VARIANTS),$(RM) $(addprefix $(variant)/,$(BASE_OBJS));)
clean:
	-$(RM) $(OBJS)
	-$(RM) $(C_DEPS)
	-$(RM) $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(OUTPUT_FILE_PATH) $(OUTPUT_MAP) $(OUTPUT_HEX)))

distclean: clean
	-$(RM) $(FINAL_GENERATED)

.PHONY: all hdk_oled hdk_lcd dsight clean distclean
