# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2016 Sensics, Inc.
# All rights reserved.

# Helper makefile:
# Set SHORT_NAME, VARIANT_NAME, and optionally SKIP_ALL_TARGET, EXTRA_C_SRCS
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
$(COPIED_OUTPUT_$(VARIANT_NAME)): SUFFIX := $(SUFFIX)

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

ALL_C_SRCS := $(C_SRCS) $(EXTRA_C_SRCS)

C_OBJS := $(addprefix $(BUILD_DIR)/,$(ALL_C_SRCS:%.c=%.o))

# Build .c -> .o
$(BUILD_DIR)/%.o: $(REL_ROOT)/%.c
	$(CANNED_RECIPE_BEGINNING_SHOW_IN_AND_OUT)
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) -O$(strip $(OPTIMIZATION)) -MD -MP -MF "$(@:o=d)" -MT"$(@:o=d)" -MT"$(@:o=o)" -MT"$(@:o=i)"  -o "$@" "$<"

S_OBJS := $(addprefix $(BUILD_DIR)/,$(PREPROCESSING_SRCS:s=o))

# Pattern for building object files from .s
$(BUILD_DIR)/%.o: $(REL_ROOT)/%.s
	$(CANNED_RECIPE_BEGINNING_SHOW_IN_AND_OUT)
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_ASFLAGS) -MD -MP -MF "$(@:o=d)" -MT"$(@:o=d)" -MT"$(@:o=o)" -Wa,-g -o "$@" "$<"

CURRENT_OBJS := $(C_OBJS) $(S_OBJS)

# Pattern for outputting preprocessed files from .c files
# Additional flags are to disable warnings, turn on preprocessor-only output, -dDI to show define directives and results of preprocessing as well as include directives, -C to include comments
$(BUILD_DIR)/%.i: $(REL_ROOT)/%.c
	$(CANNED_RECIPE_BEGINNING_SHOW_IN_AND_OUT)
	$(QUIETRULE)$(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) -w -E -dDI -C -o "$@" "$<"

# For all non-ASF files, make a shortcut phony target that's just $(BUILD_DIR)/basename.i to keep the typing shorter.
define make_preprocess_phony_target
# only if this would actually be a shortcut and not just replace the real target
ifneq ($(strip $(dir $1)),./)
.PHONY: $(BUILD_DIR)/$(basename $(notdir $1)).i
$(BUILD_DIR)/$(basename $(notdir $1)).i: $(BUILD_DIR)/$(1:c=i)
endif
endef
# We skip ASF files here because there are lots of duplicate filenames, and generally we suspect they aren't as interesting to preprocess anyway.
$(foreach src,$(filter-out src/ASF/%,$(ALL_C_SRCS)),$(eval $(call make_preprocess_phony_target,$(src))))

# Makes the elf file
$(BUILD_DIR)/$(OUTPUT_FILE_PATH): $(CURRENT_OBJS) $(LIBS) $(LIBS_$(VARIANT))
	$(CANNED_RECIPE_BEGINNING_SHOW_OUT)
	$(QUIETRULE)$(CC) -o"$@" $^ $(LIBS) $(LIBS_$(VARIANT)) -Wl,-Map="$(BUILD_DIR)/$(OUTPUT_MAP)" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=$(MCU) -Wl,--relax -Wl,--section-start=.BOOT=0x40000
	$(QUIETRULE)$(AVRSIZE) --mcu=$(MCU) --format=avr "$@"
%.elf: DISPLAY_OP := Linking

###
# Compiler database generation - optional, for clang tool usage.
# requires the "jq" tool v1.5 or newer to make the "compile_commands"

COMPILATION_DATABASE_FILENAME := compile_commands.json
COMPILATION_DATABASE_PARAMS := command_parts.json
json_stringify = jq -R "." >>"$@"

# the PARAMS get sucked in as a string array: lines as follows:
# the main part of the compile command (some must be manually kept in sync!)
# "build directory" (makefile dir) absolute path
# output build directory - the thing we append to the source paths to get the object paths
# the absolute source root
EXTRA_CLEAN_FILES += $(BUILD_DIR)/$(COMPILATION_DATABASE_PARAMS)
$(BUILD_DIR)/$(COMPILATION_DATABASE_PARAMS): Makefile add_variant.mk
	$(CANNED_RECIPE_BEGINNING_SHOW_OUT)
	$(QUIETRULE)-$(RM) "$@"
	$(QUIETRULE)echo $(CC) $(call make_include_dirs,$(VARIANT)) $(ALL_CFLAGS) -O$(strip $(OPTIMIZATION)) -MD -MP | $(json_stringify)
	$(QUIETRULE)echo $(abspath $(strip .))| $(json_stringify)
	$(QUIETRULE)echo $(abspath $(strip $(BUILD_DIR)))| $(json_stringify)
	$(QUIETRULE)echo $(abspath $(REL_ROOT))| $(json_stringify)

# This rule takes the makefile variable with all the sources, turns it into a long semicolon-delimited string,
# and feeds it into the "jq" tool, along with the PARAMS (via the command line), and a jq script (also via command line arg)
# which does the dirty work of turning it into something that looks like it might have come out of CMake.
EXTRA_CLEAN_FILES += $(BUILD_DIR)/$(COMPILATION_DATABASE_FILENAME)
$(BUILD_DIR)/$(COMPILATION_DATABASE_FILENAME): make-compiledatabase.jq $(BUILD_DIR)/$(COMPILATION_DATABASE_PARAMS) add_variant.mk
	$(CANNED_RECIPE_BEGINNING_SHOW_OUT)
	$(QUIETRULE)echo $(subst $(eval) ,;,$(strip $(ALL_C_SRCS))) | jq --raw-input --slurp --slurpfile params "$(BUILD_DIR)/$(COMPILATION_DATABASE_PARAMS)" -f "$<" > "$@"
#
# end clang/libtooling section
###

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


# set target-specific variables - have to do this here, where these are not recursively defined.
$(BUILD_DIR)/%: SUFFIX := $(SUFFIX)
$(BUILD_DIR)/%: VARIANT := $(VARIANT)

# Clean up work variables
C_OBJS :=
S_OBJS :=
CURRENT_OBJS :=
VARIANT :=

# Clean up input variables
# undefine SHORT_NAME
# undefine VARIANT_NAME
# undefine SKIP_ALL_TARGET
EXTRA_C_SRCS :=
SHORT_NAME :=
VARIANT_NAME :=
SKIP_ALL_TARGET :=
