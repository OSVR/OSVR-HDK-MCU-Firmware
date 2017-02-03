# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2017 Sensics, Inc.
# All rights reserved.

inFile := $(variant).hex

# If we actually have a control variant
ifneq (,$(strip $(CONTROL_VARIANT_PREFIX)))
controlVariant := $(CONTROL_VARIANT_PREFIX)$(BUNDLED_VERSION_DASH)$(CONTROL_VARIANT_SUFFIX).hex
$(OSVR_CONTROL_OUTDIR)$(controlVariant): $(inFile)
	$(call FUNC_CP,$<,$@)
bundled:: $(OSVR_CONTROL_OUTDIR)$(controlVariant)

CONTROL_VARIANT_PREFIX :=
CONTROL_VARIANT_SUFFIX :=
controlVariant :=
endif

# GitHub release variant
githubVariant := $(variant)$(VARIANT_NAME_SUFFIX)-v$(subst v,,$(BUNDLED_VERSION)).hex
$(GITHUB_OUTDIR)$(githubVariant): $(inFile)
	$(call FUNC_CP,$<,$@)
bundled:: $(GITHUB_OUTDIR)$(githubVariant)

VARIANT_NAME_SUFFIX :=
githubVariant :=
inFile :=
