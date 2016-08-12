# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2016 Sensics, Inc.
# All rights reserved.

# Helper makefile:
# Set SHORT_NAME, VARIANT_NAME, and optionally SKIP_ALL_TARGET
# before including this file.
#
# Creates a rule to copy the final hex to a renamed file in the current directory
# Creates a phony rule with the short name (and adds it to a variable CONVENIENCE_TARGETS)
# If SKIP_ALL_TARGET is not defined, the short name rule will be added to "all" (and to a variable BUILT_BY_ALL)
# Adds the VARIANT_NAME to the list of VARIANTS

# Convenience variable
COPIED_OUTPUT_$(VARIANT_NAME) := $(SHORT_NAME)$(FN_SUFFIX).hex

# Needed by the recursively-expanded BUILD_DIR
VARIANT := $(VARIANT_NAME)

# "Map"
FULL_VARIANT_NAME_$(SHORT_NAME) := $(VARIANT_NAME)

FINAL_GENERATED += $(COPIED_OUTPUT_$(VARIANT_NAME))

$(COPIED_OUTPUT_$(VARIANT_NAME)): $(BUILD_DIR)/$(OUTPUT_HEX) $(BUILD_DIR)/$(OUTPUT_LSS)
	$(QUIETRULE)$(call FUNC_CP,$<,$@)
	@echo [$(VARIANT)] Copied output hex to $@
# set target-specific variable
$(COPIED_OUTPUT_$(VARIANT_NAME)): VARIANT := $(VARIANT)

$(SHORT_NAME): $(COPIED_OUTPUT_$(VARIANT_NAME))
	@echo [$(FULL_VARIANT_NAME_$(@)) - $@] Build finished.
.PHONY: $(SHORT_NAME)

ifeq ($(strip $(SKIP_ALL_TARGET)),)
all: $(SHORT_NAME)
BUILT_BY_ALL += $(SHORT_NAME)
endif

CONVENIENCE_TARGETS += $(SHORT_NAME)

VARIANTS += $(VARIANT_NAME)

CURRENT_OBJS :=

C_OBJS := $(addprefix $(BUILD_DIR)/,$(C_SRCS:%.c=%.o))

# Build .c -> .o
$(BUILD_DIR)/%.o: $(REL_ROOT)/%.c
	@echo [$(VARIANT)$(SUFFIX)] (-O$(strip $(OPTIMIZATION))) $< : $@
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) -O$(strip $(OPTIMIZATION)) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
# set pattern-specific variable for the variant
$(BUILD_DIR)/%.o: VARIANT := $(VARIANT)

S_OBJS := $(addprefix $(BUILD_DIR)/,$(PREPROCESSING_SRCS:%.s=%.o))

# Pattern for building object files from .s
$(BUILD_DIR)/%.o: $(REL_ROOT)/%.s
	@echo [$(VARIANT)$(SUFFIX)] $< : $@
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_ASFLAGS) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g -o "$@" "$<"
# set pattern-specific variable for the variant
$(BUILD_DIR)/%.o: VARIANT := $(VARIANT)

CURRENT_OBJS := $(C_OBJS) $(S_OBJS)

# Makes the elf file
$(BUILD_DIR)/$(OUTPUT_FILE_PATH): $(CURRENT_OBJS) $(LIBS) $(LIBS_$(VARIANT))
	@echo [$(VARIANT)$(SUFFIX)] Linking $@
	$(QUIETRULE)$(CC) -o"$@" $^ $(LIBS) $(LIBS_$(VARIANT)) -Wl,-Map="$(BUILD_DIR)/$(OUTPUT_MAP)" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=$(MCU) -Wl,--relax -Wl,--section-start=.BOOT=0x40000
	$(QUIETRULE)$(AVRSIZE) --mcu=$(MCU) --format=avr "$@"
# set target-specific variables
$(BUILD_DIR)/$(OUTPUT_FILE_PATH): SUFFIX := $(SUFFIX)
$(BUILD_DIR)/$(OUTPUT_FILE_PATH): VARIANT := $(VARIANT)

# Add targets for different configurations of the build.
CONFIG_SHORT_NAME := hdmi_verbose
DEFINES_CONTENTS := HDMI_VERBOSE
include add_config.mk

CONFIG_SHORT_NAME := disable_video
DEFINES_CONTENTS := SVR_DISABLE_VIDEO_INPUT
# TODO fix this - should be able to build dsight without video too!
SKIP_VARIANTS := dSight_Sharp_LCD
include add_config.mk

CONFIG_SHORT_NAME := bno_dfu
DEFINES_CONTENTS := PERFORM_BNO_DFU
SKIP_VARIANTS := dSight_Sharp_LCD HDK_Sharp_LCD
include add_config.mk

# Clean up work variables
C_OBJS :=
S_OBJS :=
CURRENT_OBJS :=
VARIANT :=

# Clean up input variables
# undefine SHORT_NAME
# undefine VARIANT_NAME
# undefine SKIP_ALL_TARGET
SHORT_NAME :=
VARIANT_NAME :=
SKIP_ALL_TARGET :=
