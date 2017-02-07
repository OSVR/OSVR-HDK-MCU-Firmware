# Part of the HMD MCU build system by Sensics, Inc.
# Copyright 2017 Sensics, Inc.
# All rights reserved.

# Invoke as-is to build a release at a tag. (make -f bundled.mk)
# Invoke with non-empty SNAPSHOT to build a snapshot at any commit. (make -f bundled.mk SNAPSHOT=1)

# Default target for this makefile.
bundled::
.PHONY: bundled

include Makefile

ifneq (,$(SNAPSHOT))
# Snapshot build
BUNDLED_VERSION := $(GIT_DESCRIBE_OUTPUT)
BASE_OUTDIR := snapshot-$(BUNDLED_VERSION)/

else
# Release build
BUNDLED_VERSION := $(GIT_LAST_TAG)
BASE_OUTDIR := release-$(BUNDLED_VERSION)/

# Must be at tag for release build
ifeq ($(GIT_AT_TAG),0)
BUNDLED_VERSION :=
bundled::
	$(error Not at a tag, cannot release. Try a snapshot build?)
endif # end of not-at-tag
endif #end of not-snapshot

# For release or snapshot build, must be clean source tree.
ifneq ($(GIT_DIRTY),0)
BUNDLED_VERSION :=
bundled::
	$(error Tree dirty, cannot release. Commit changes and try a snapshot build?)
endif

# If we ended up with a BUNDLED_VERSION variable with contents, then really build.
ifneq (,$(strip $(BUNDLED_VERSION)))
$(info Building a bundle to $(BASE_OUTDIR))
# Strips any "v" and changes . in to -
BUNDLED_VERSION_DASH := $(subst .,-,$(BUNDLED_VERSION:v%=%))

OSVR_CONTROL_OUTDIR := $(BASE_OUTDIR)OSVR-Control/
GITHUB_OUTDIR := $(BASE_OUTDIR)GitHub/
bundledirs:
	-$(call FUNC_MKDIR_P,$(BASE_OUTDIR))
	-$(call FUNC_MKDIR_P,$(OSVR_CONTROL_OUTDIR))
	-$(call FUNC_MKDIR_P,$(GITHUB_OUTDIR))
.PHONY: bundledirs
bundled:: bundledirs

# $(BASE_OUTDIR):
# 	-mkdir "$@"
# $(OSVR_CONTROL_OUTDIR): $(BASE_OUTDIR)
# 	-mkdir "$@"
# $(GITHUB_OUTDIR): $(BASE_OUTDIR)
# 	-mkdir "$@"


# VARIANT_NAME_SUFFIX - Suffix as used on GitHub releases page
# CONTROL_VARIANT_PREFIX - if non-empty, a copy is made for OSVR-Control as $(CONTROL_VARIANT_PREFIX)$(RELEASE_VERSION_DASH)$(CONTROL_VARIANT_SUFFIX).hex
# CONTROL_VARIANT_SUFFIX - coresponding suffix (optional - .hex added automatically)
variant := dsight
VARIANT_NAME_SUFFIX :=
CONTROL_VARIANT_PREFIX := dsight-
CONTROL_VARIANT_SUFFIX :=
include bundled-include.mk

variant := hdk_lcd
VARIANT_NAME_SUFFIX := -HDK-1.1
CONTROL_VARIANT_PREFIX := HDK11-
CONTROL_VARIANT_SUFFIX := -LCD
include bundled-include.mk

variant := hdk_oled
VARIANT_NAME_SUFFIX := -HDK-1.3-1.4
CONTROL_VARIANT_PREFIX := HDK1314-
CONTROL_VARIANT_SUFFIX := -OLED
include bundled-include.mk

variant := hdk_oled-WirelessOnly
# Need a dep to force this one to (re-)build.
bundled:: hdk_oled_WirelessOnly

VARIANT_NAME_SUFFIX := -HDK-1.3-1.4
# Not made available in OSVR-Control
CONTROL_VARIANT_PREFIX :=
CONTROL_VARIANT_SUFFIX :=
include bundled-include.mk

variant := hdk2svr
VARIANT_NAME_SUFFIX :=
CONTROL_VARIANT_PREFIX := HDK2-svr-
CONTROL_VARIANT_SUFFIX :=
include bundled-include.mk

endif
