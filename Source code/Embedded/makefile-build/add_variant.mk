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

# "Map"
FULL_VARIANT_NAME_$(SHORT_NAME) := $(VARIANT_NAME)

FINAL_GENERATED += $(COPIED_OUTPUT_$(VARIANT_NAME))
#$(info Available variant: $(VARIANT_NAME) aka $(SHORT_NAME), copied output of $(COPIED_OUTPUT_$(VARIANT_NAME)))

# For this target, we pass $< instead of $@ to set_variant_variable because that's the one that still lives in the variant directory.
$(COPIED_OUTPUT_$(VARIANT_NAME)): $(VARIANT_NAME)/$(OUTPUT_HEX) $(VARIANT_NAME)/$(OUTPUT_LSS)
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

# Clean up input variables
# undefine SHORT_NAME
# undefine VARIANT_NAME
# undefine SKIP_ALL_TARGET
SHORT_NAME :=
VARIANT_NAME :=
SKIP_ALL_TARGET :=
