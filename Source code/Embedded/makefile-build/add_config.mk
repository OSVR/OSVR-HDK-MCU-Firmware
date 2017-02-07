# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2016 Sensics, Inc.
# All rights reserved.

# Helper makefile included by add_variant.mk:
# Set CONFIG_SHORT_NAME and DEFINES_CONTENTS, and optionally SKIP_VARIANTS or USE_VARIANTS
# immediately each time before including this file.
# add_variant.mk is expected to already have defined SHORT_NAME and VARIANT.
# In a single place in the overall makefile:
# CONFIG_CLEAN_TARGET and CONFIG_REALCLEAN_TARGET should be defined as
# recursively-expanded, using $(CONFIG), and
# CONFIG_SHORT_NAMES, CONFIG_CLEAN_TARGETS, CONFIG_REALCLEAN_TARGETS should be defined as empty and simply-expanded
#
# Creates a $(SHORT_NAME)_$(CONFIG_SHORT_NAME) rule to build the $(VARIANT) with DEFINES=$(DEFINES_CONTENTS)
# as long as $(VARIANT) is not in $(SKIP_VARIANTS) (or is in $(USE_VARIANTS))
#
# If not already existing, creates a $(CONFIG_SHORT_NAME) target which will
# depend on all $(SHORT_NAME)_$(CONFIG_SHORT_NAME) for all variants with this particular config, as
# well as creates $(CONFIG_SHORT_NAME)_clean and $(CONFIG_SHORT_NAME)_realclean targets
# (name format determined by CONFIG_CLEAN_TARGET and CONFIG_REALCLEAN_TARGET) that
# invoke clean and realclean, respectively, with the config applied


ifneq (,$(filter-out $(CONFIG_SHORT_NAMES),$(CONFIG_SHORT_NAME)))

# This is the first time add_config has been invoked for this configuration.
CONFIG := $(CONFIG_SHORT_NAME)
CONFIG_SHORT_NAMES += $(CONFIG)

$(CONFIG)_TARGETS :=

# Save this for potentially generating the help in the future.
DEFINES_$(CONFIG_SHORT_NAME) := $(DEFINES_CONTENTS)

# Make a target and its clean counterparts.
# No contents to this target: each variant will add its target as a dependency instead.
$(CONFIG):
.PHONY: $(CONFIG)

# No dependency between *_clean and *_realclean because there's a dependency between the
# real targets (clean and realclean) that they end up invoking: these are
# effectively "convenience" targets.

$(CONFIG_CLEAN_TARGET):
	@echo [$(CONFIG_CLEAN_TARGET)] Recursively building the clean target with "DEFINES=$(DEFINES_CONTENTS)"
	$(QUIETRULE)$(MAKE) "DEFINES=$(DEFINES_CONTENTS)" clean
.PHONY: $(CONFIG_CLEAN_TARGET)
# Pass known variables in.
$(CONFIG_CLEAN_TARGET): CONFIG := $(CONFIG_SHORT_NAME)
$(CONFIG_CLEAN_TARGET): DEFINES_CONTENTS := $(DEFINES_CONTENTS)

CONFIG_CLEAN_TARGETS += $(CONFIG_CLEAN_TARGET)

$(CONFIG_REALCLEAN_TARGET):
	@echo [$(CONFIG_REALCLEAN_TARGET)] Recursively building the realclean target with "DEFINES=$(DEFINES_CONTENTS)"
	$(QUIETRULE)$(MAKE) "DEFINES=$(DEFINES_CONTENTS)" realclean
.PHONY: $(CONFIG_REALCLEAN_TARGET)
# Pass known variables in.
$(CONFIG_REALCLEAN_TARGET): CONFIG := $(CONFIG_SHORT_NAME)
$(CONFIG_REALCLEAN_TARGET): DEFINES_CONTENTS := $(DEFINES_CONTENTS)

CONFIG_REALCLEAN_TARGETS += $(CONFIG_REALCLEAN_TARGET)

endif


ifneq (,$(strip $(USE_VARIANTS)))
# USE_VARIANTS is defined. Check the list.
ConfigValidForVariant :=
ifeq (,$(filter-out $(USE_VARIANTS),$(VARIANT)))
# We're in the USE_VARIANTS list.
ConfigValidForVariant := 1
endif
else
# USE_VARIANTS is not usefully defined. Assume we're in unless SKIP_VARIANTS says so.
ConfigValidForVariant := 1
ifeq (,$(filter-out $(SKIP_VARIANTS),$(VARIANT)))
# We're in the SKIP_VARIANTS list.
ConfigValidForVariant :=
endif
endif

ifneq (,$(strip $(ConfigValidForVariant)))
# If this variant is not disabled or is explicitly enabled for this config:

CONFIG_TARGET_NAME := $(SHORT_NAME)_$(CONFIG_SHORT_NAME)

# here's the recursive target to build it.
$(CONFIG_TARGET_NAME):
	$(MAKE) DEFINES=$(DEFINES_CONTENTS) $(SHORT_NAME)
.PHONY: $(CONFIG_TARGET_NAME)
# Pass the applicable variables into the target.
$(CONFIG_TARGET_NAME): SHORT_NAME := $(SHORT_NAME)
$(CONFIG_TARGET_NAME): DEFINES_CONTENTS := $(DEFINES_CONTENTS)

# Have the overall config name build this target.
$(CONFIG_SHORT_NAME): $(CONFIG_TARGET_NAME)

# Add it to the lists.
$(CONFIG_SHORT_NAME)_TARGETS += $(CONFIG_TARGET_NAME)
SPECIAL_CONFIG_TARGETS += $(CONFIG_TARGET_NAME)

endif

# work vars
CONFIG_TARGET_NAME :=

# input vars specific to this makefile
CONFIG_SHORT_NAME :=
DEFINES_CONTENTS :=
SKIP_VARIANTS :=
USE_VARIANTS :=
