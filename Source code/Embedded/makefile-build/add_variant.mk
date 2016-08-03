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

# Needed by the recursively-evaluated BUILD_DIR
VARIANT := $(VARIANT_NAME)

# "Map"
FULL_VARIANT_NAME_$(SHORT_NAME) := $(VARIANT_NAME)

FINAL_GENERATED += $(COPIED_OUTPUT_$(VARIANT_NAME))
#$(info Available variant: $(VARIANT_NAME) aka $(SHORT_NAME), copied output of $(COPIED_OUTPUT_$(VARIANT_NAME)))

# For this target, we pass $< instead of $@ to set_variant_variable because that's the one that still lives in the variant directory.
$(COPIED_OUTPUT_$(VARIANT_NAME)): $(BUILD_DIR)/$(OUTPUT_HEX) $(BUILD_DIR)/$(OUTPUT_LSS)
	$(call set_variant_variable,$<)
	$(QUIETRULE)$(call FUNC_CP,$<,$@)
	@echo [$(VARIANT)] Copied output hex to $@

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
#$(C_OBJS): $(VARIANT_NAME)/%.o : $(REL_ROOT)/%.c

$(BUILD_DIR)/%.o: $(REL_ROOT)/%.c
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] (-O$(strip $(OPTIMIZATION))) $< : $@
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) -O$(strip $(OPTIMIZATION)) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"

S_OBJS := $(addprefix $(BUILD_DIR)/,$(PREPROCESSING_SRCS:%.s=%.o))

# Pattern for building object files from .s
#$(S_OBJS): $(VARIANT_NAME)/%.o : $(REL_ROOT)/%.s
$(BUILD_DIR)/%.o: $(REL_ROOT)/%.s
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] $< : $@
	$(QUIETRULE)$(call FUNC_MKDIR_P,$(@D))
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_ASFLAGS) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -Wa,-g -o "$@" "$<"

CURRENT_OBJS := $(C_OBJS) $(S_OBJS)

# Makes the elf file
$(BUILD_DIR)/$(OUTPUT_FILE_PATH): $(CURRENT_OBJS)
	$(call set_variant_variable,$@)
	@echo [$(VARIANT)$(SUFFIX)] Linking $@
	$(QUIETRULE)$(CC) -o"$@" $^ $(LIBS) $(LIBS_$(VARIANT)) -Wl,-Map="$(BUILD_DIR)/$(OUTPUT_MAP)" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=$(MCU) -Wl,--relax -Wl,--section-start=.BOOT=0x40000
	$(QUIETRULE)$(AVRSIZE) --mcu=$(MCU) --format=avr "$@"

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
