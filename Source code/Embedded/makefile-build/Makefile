
RM := rm -rf
CC := "C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe"
OBJCOPY := "C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe"
OBJDUMP := "C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe"
AVRSIZE := "C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe"

PATH_CONVERT = $(subst /,\,$1)
MKDIR_P = if not exist "$(call PATH_CONVERT,$1)" mkdir "$(subst /,\,$1)"
CP = copy
#if 0
#PATH_CONVERT = $1
#MKDIR_P = mkdir -p "$1"
#endif

GET_PATH = $(subst /,\,$(dir $1))

#define GET_PATH
#$(eval OUT_PATH := $(subst /,\,$(dir $1)))
#$(eval $(info OUT_PATH = $(OUT_PATH)))
#$(OUT_PATH)
#endef

USER_OBJS :=

LIBS := 
VARIANTS := \
    HDK_OLED \
	HDK_Sharp_LCD \
	dSight_Sharp_LCD

OUTPUT_STEM := HMD_MCU
OUTPUT_FILE_PATH := $(OUTPUT_STEM).elf
OUTPUT_MAP := $(OUTPUT_STEM).map
OUTPUT_HEX := $(OUTPUT_STEM).hex
	
MCU=atxmega256a3bu
REL_ROOT := ..
REL_SRC := $(REL_ROOT)/src

for_each_variant = $(foreach variant,$(VARIANTS),$(variant)/$1)

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../src/ \
../src/ASF/ \
../src/ASF/common/ \
../src/ASF/common/boards/ \
../src/ASF/common/services/ \
../src/ASF/common/services/clock/ \
../src/ASF/common/services/clock/xmega/ \
../src/ASF/common/services/delay/ \
../src/ASF/common/services/delay/xmega/ \
../src/ASF/common/services/gpio/ \
../src/ASF/common/services/gpio/xmega_gpio/ \
../src/ASF/common/services/ioport/ \
../src/ASF/common/services/ioport/xmega/ \
../src/ASF/common/services/serial/ \
../src/ASF/common/services/serial/xmega_usart/ \
../src/ASF/common/services/sleepmgr/ \
../src/ASF/common/services/sleepmgr/xmega/ \
../src/ASF/common/services/spi/ \
../src/ASF/common/services/spi/xmega_spi/ \
../src/ASF/common/services/spi/xmega_usart_spi/ \
../src/ASF/common/services/twi/ \
../src/ASF/common/services/twi/xmega_twi/ \
../src/ASF/common/services/usb/ \
../src/ASF/common/services/usb/class/ \
../src/ASF/common/services/usb/class/cdc/ \
../src/ASF/common/services/usb/class/cdc/device/ \
../src/ASF/common/services/usb/class/cdc/device/example/ \
../src/ASF/common/services/usb/class/cdc/device/example/atxmega256a3bu_xmega_a3bu_xplained/ \
../src/ASF/common/services/usb/class/composite/ \
../src/ASF/common/services/usb/class/composite/device/ \
../src/ASF/common/services/usb/class/hid \
../src/ASF/common/services/usb/class/hid/device \
../src/ASF/common/services/usb/class/hid/device/generic \
../src/ASF/common/services/usb/udc/ \
../src/ASF/common/utils/ \
../src/ASF/common/utils/interrupt/ \
../src/ASF/common/utils/make/ \
../src/ASF/xmega/ \
../src/ASF/xmega/boards/ \
../src/ASF/xmega/boards/xmega_a3bu_xplained/ \
../src/ASF/xmega/drivers/ \
../src/ASF/xmega/drivers/cpu/ \
../src/ASF/xmega/drivers/nvm/ \
../src/ASF/xmega/drivers/pmic/ \
../src/ASF/xmega/drivers/rtc32/ \
../src/ASF/xmega/drivers/sleep/ \
../src/ASF/xmega/drivers/spi/ \
../src/ASF/xmega/drivers/tc/ \
../src/ASF/xmega/drivers/twi/ \
../src/ASF/xmega/drivers/usart/ \
../src/ASF/xmega/drivers/usb/ \
../src/ASF/xmega/services/ \
../src/ASF/xmega/services/pwm/ \
../src/ASF/xmega/services/timeout/ \
../src/ASF/xmega/utils/ \
../src/ASF/xmega/utils/assembler/ \
../src/ASF/xmega/utils/bit_handling/ \
../src/ASF/xmega/utils/preprocessor/ \
../src/config/ \
../src/DeviceDrivers \
../src/DeviceDrivers/bno-hostif \
../src/DeviceDrivers/bno-hostif/src \
../src/Variants \
../src/NXP \
../src/Variants/HDK_OLED \
../src/Variants/HDK_Sharp_LCD \
../src/Variants/dSight_Sharp_LCD


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS :=  \
src/ASF/common/services/usb/class/composite/device/udi_composite_desc.c \
src/ASF/common/services/usb/class/hid/device/generic/udi_hid_generic.c \
src/ASF/common/services/usb/class/hid/device/udi_hid.c \
src/ASF/xmega/drivers/rtc32/rtc32.c \
src/ASF/xmega/drivers/tc/tc.c \
src/ASF/xmega/services/pwm/pwm.c \
src/ASF/xmega/services/timeout/timeout.c \
src/DeviceDrivers/bno-hostif/1000-3251_1.7.0.390_avr.c \
src/DeviceDrivers/bno-hostif/bno_callbacks.c \
src/DeviceDrivers/bno-hostif/src/sensorhub.c \
src/DeviceDrivers/bno-hostif/src/sensorhub_hid.c \
src/DeviceDrivers/BNO070_using_hostif.c \
src/NXP/NXP_AVR_Internal.c \
src/TimingDebug.c \
src/USB.c \
_StackInstrument.c \
src/ASF/common/services/clock/xmega/sysclk.c \
src/ASF/common/services/ioport/xmega/ioport_compat.c \
src/ASF/common/services/serial/usart_serial.c \
src/ASF/common/services/sleepmgr/xmega/sleepmgr.c \
src/ASF/common/services/spi/xmega_spi/spi_master.c \
src/ASF/common/services/spi/xmega_usart_spi/usart_spi.c \
src/ASF/common/services/usb/class/cdc/device/example/atxmega256a3bu_xmega_a3bu_xplained/ui.c \
src/ASF/common/services/usb/class/cdc/device/udi_cdc.c \
src/ASF/common/services/usb/udc/udc.c \
src/ASF/xmega/boards/xmega_a3bu_xplained/init.c \
src/ASF/xmega/drivers/nvm/nvm.c \
src/ASF/xmega/drivers/spi/spi.c \
src/ASF/xmega/drivers/twi/twim.c \
src/ASF/xmega/drivers/twi/twis.c \
src/ASF/xmega/drivers/usart/usart.c \
src/ASF/xmega/drivers/usb/usb_device.c \
src/Boot.c \
src/Console.c \
src/DeviceDrivers/Solomon.c \
src/DeviceDrivers/TI-TMDS442.c \
src/FPGA.c \
src/main.c \
src/my_hardware.c \
src/NXP/AVRHDMI.c \
src/NXP/i2c.c \
src/NXP/tmbslTDA1997X_functions.c \
src/NXP/tmdlHdmiRx.c \
src/NXP/tmdlHdmiRx_cfg.c \
src/NXP/tmdlHdmiRx_IW_RTX.c \
src/SerialStateMachine.c \
src/uart_xmega.c


PREPROCESSING_SRCS :=  \
src/ASF/xmega/drivers/cpu/ccp.s \
src/ASF/xmega/drivers/nvm/nvm_asm.s

BASE_OBJS :=  $(C_SRCS:%.c=%.o) $(PREPROCESSING_SRCS:%.s=%.o)

BASE_C_DEPS := $(C_SRCS:%.c=%.d)

OBJS := $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(BASE_OBJS)))
C_DEPS := $(foreach variant,$(VARIANTS),$(addprefix $(variant)/,$(BASE_C_DEPS)))

OBJ_DIRS := $(sort $(dir $(OBJS)))
OBJ_DIRS := $(subst /,\,$(OBJ_DIRS))
#$(info $(OBJ_DIRS))


ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 

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

get_variant=$(word 1,$(subst /, ,$1))
get_variant_include_dir=$(REL_ROOT)/src/Variants/$(word 1,$(subst /, ,$1))
make_include_flags=$(patsubst %, -I"%",$1)

DEFINES := CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA \
           IOPORT_XMEGA_COMPAT \
		   DEBUG \
		   BOARD=USER_BOARD

COMMON_FLAGS := -mmcu=$(MCU) -mrelax $(patsubst %, -D%,$(DEFINES))
CFLAGS := -x c $(COMMON_FLAGS)\
          -O3 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g3 -Wall \
		  -c -std=gnu99 -fno-strict-aliasing \
		  -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith
ASFLAGS := -Wa,-gdwarf2 -x assembler-with-cpp -c $(COMMON_FLAGS)
OPTIMIZATION = -O3

DEBUG_VARIANT = @echo Variant: $(VARIANT)

# Pattern for creating directories
#$(OBJ_DIRS):
#	-$(call MKDIR_P,$@)

# Pattern for building object files from c
#$(foreach variant,$(VARIANTS),$(variant)/%.o): $(REL_ROOT)/%.c
#$(call GET_PATH,$@)
$(call for_each_variant,%.o): $(REL_ROOT)/%.c 
	-$(call MKDIR_P,$(@D))
	$(eval VARIANT?=$(call get_variant,$@))
	$(DEBUG_VARIANT)
	$(CC)  $(patsubst %, -I"%",$(REL_SRC)/Variants/$(VARIANT) $(COMMON_INCLUDE_DIRS)) $(CFLAGS) $(OPTIMIZATION) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 

# Pattern for building object files from .s
$(call for_each_variant,%.o): $(REL_ROOT)/%.s
	-$(call MKDIR_P,$(@D))
	$(eval VARIANT?=$(call get_variant,$@))
	$(DEBUG_VARIANT)
	$(CC) $(ASFLAGS) $(patsubst %, -I"%",$(REL_SRC)/Variants/$(VARIANT) $(COMMON_INCLUDE_DIRS)) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g -o "$@" "$<"
	
# This file needs optimization turned off (set to -O0) - hopefully this will do it.
$(call for_each_variant,src/DeviceDrivers/bno-hostif/src/sensorhub.o): OPTIMIZATION = -O0

#OUTPUT_FILE_PATHS := $(call for_each_variant,$(OUTPUT_FILE_PATH))

# Makes the elf file
$(call for_each_variant,$(OUTPUT_FILE_PATH)): %/$(OUTPUT_FILE_PATH) : $(addprefix %/,$(BASE_OBJS))
	-$(call MKDIR_P,$(@D))
	$(eval VARIANT?=$(call get_variant,$@))
	$(DEBUG_VARIANT)
	@echo output: $@
	@echo deps: $<
	$(CC) -o"$@" $(patsubst %,$(VARIANT)/%,$(BASE_OBJS)) $(USER_OBJS) $(LIBS) -Wl,-Map="$(VARIANT)/$(OUTPUT_MAP)" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=$(MCU) -Wl,--relax -Wl,--section-start=.BOOT=0x40000
	$(AVR_SIZE) "$@"


$(call for_each_variant,$(OUTPUT_HEX)): %/$(OUTPUT_HEX) : %/$(OUTPUT_FILE_PATH)
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "$<" "$@"
#$(patsubst %,HDK_OLED/,$(BASE_OBJS))
#%/$(OUTPUT_FILE_PATH): $(foreach obj,$(BASE_OBJS),%/$(obj))
#$(foreach obj,$(BASE_OBJS),%/$(obj)) 
#	@echo output: $@
#	@echo deps: $<
#$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="HMD-MCU.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=atxmega256a3bu -Wl,--relax -Wl,--section-start=.BOOT=0x40000

define VARIANT_TOPLEVEL
	$(CP) $< $@.hex 
endef

#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "HMD-MCU.elf" "HMD-MCU.hex"
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "HMD-MCU.elf" "HMD-MCU.eep" || exit 0
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "HMD-MCU.elf" > "HMD-MCU.lss"
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .user_signatures --set-section-flags=.user_signatures=alloc,load --change-section-lma .user_signatures=0 --no-change-warnings -O ihex "HMD-MCU.elf" "HMD-MCU.usersignatures" || exit 0
#	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "HMD-MCU.elf"

all: hdk_oled hdk_lcd dsight


hdk_oled: HDK_OLED/$(OUTPUT_HEX)
	$(call VARIANT_TOPLEVEL)
hdk_lcd: HDK_Sharp_LCD/$(OUTPUT_HEX)
	$(call VARIANT_TOPLEVEL)
dsight: dSight_Sharp_LCD/$(OUTPUT_HEX)
	$(call VARIANT_TOPLEVEL)

.PHONY: all hdk_oled hdk_lcd dsight
	