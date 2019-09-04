
# Need git to generate a custom version
ifeq ($(strip $(GIT)),)
   GIT=git
endif

GIT_DESCRIBE_OK := $(shell $(GIT) describe --tags --dirty --match '*.*' && echo GIT_DESCRIBE_OK)
#$(info GIT_DESCRIBE_OK $(GIT_DESCRIBE_OK))
GIT_DESCRIBE_OUTPUT := $(shell $(GIT) describe --tags --dirty --match '*.*')
GIT_CFLAGS :=
GIT_DIRTY := 0
GIT_AT_TAG := 0
GIT_NOT_FOUND := 0
ifneq ($(filter GIT_DESCRIBE_OK,$(GIT_DESCRIBE_OK)),)
    # OK this is successful call of git
    GIT_CFLAGS += -DSVR_GIT_DESCRIBE="$(strip $(GIT_DESCRIBE_OUTPUT))"
    GIT_DESCRIBE_OUTPUT_SPLIT := $(subst -, ,$(GIT_DESCRIBE_OUTPUT))
    #$(info GIT_DESCRIBE_OUTPUT_SPLIT $(GIT_DESCRIBE_OUTPUT_SPLIT))
    GIT_LAST_TAG := $(firstword $(GIT_DESCRIBE_OUTPUT_SPLIT))
    #$(shell echo GIT_LAST_TAG := $(GIT_LAST_TAG) >$(OUTFILE))
    #$(info GIT_LAST_TAG $(GIT_LAST_TAG))
    GIT_CFLAGS += -DSVR_GIT_LAST_TAG=\"$(strip $(GIT_LAST_TAG))\"
    ifeq ($(lastword $(GIT_DESCRIBE_OUTPUT_SPLIT)),dirty)
        #$(file >>$(OUTFILE),#define SVR_GIT_DIRTY)
        #$(shell echo GIT_DIRTY := true >>$(OUTFILE))
        GIT_CFLAGS += -DSVR_GIT_DIRTY
        GIT_DIRTY := 1
        GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN := $(GIT_DESCRIBE_OUTPUT_SPLIT:dirty=)
        $(info NOTE: Uncommitted changes!)
    else
        GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN := $(GIT_DESCRIBE_OUTPUT_SPLIT)
    endif
    #$(info GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN $(GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN))
    ifeq ($(words $(GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN)),1)
        # This is a release tag!
        #$(file >>$(OUTFILE),#define SVR_GIT_AT_TAG)
        GIT_CFLAGS += -DSVR_GIT_AT_TAG
        GIT_AT_TAG := 1
        #$(shell echo GIT_AT_TAG := true >>$(OUTFILE))
    else
        GIT_COMMIT_COUNT := $(word 2,$(GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN))
        GIT_COMMIT_HASH := $(subst g,,$(word 3,$(GIT_DESCRIBE_OUTPUT_SPLIT_CLEAN)))
        GIT_CFLAGS += -DSVR_GIT_COMMIT_COUNT=$(GIT_COMMIT_COUNT)
        #$(shell echo GIT_COMMIT_COUNT := $(GIT_COMMIT_COUNT) >>$(OUTFILE))
        #$(file >>$(OUTFILE),#define SVR_GIT_COMMIT_COUNT $(GIT_COMMIT_COUNT))
        GIT_CFLAGS += -DSVR_GIT_COMMIT_HASH=\"$(GIT_COMMIT_HASH)\"
        #$(shell echo GIT_COMMIT_HASH := $(GIT_COMMIT_HASH) >>$(OUTFILE))
        #$(file >>$(OUTFILE),#define SVR_GIT_COMMIT_HASH "$(GIT_COMMIT_HASH)")
    endif
else
    # Failed to call git
    $(warning Failed to call git!)
    GIT_NOT_FOUND := 1
    GIT_CFLAGS := -DSVR_GIT_NOT_FOUND
    $(shell echo GIT_NOT_FOUND := true >$(OUTFILE))
    #$(file >>$(OUTFILE),#define SVR_GIT_NOT_FOUND)
endif

GITREV_OUTFILE := gitdefines.h
gitdefheader:
	@echo +define SVR_GIT_NOT_FOUND $(GIT_NOT_FOUND) > $(GITREV_OUTFILE).tmp
	@echo +define SVR_GIT_DIRTY $(GIT_DIRTY) >> $(GITREV_OUTFILE).tmp
	@echo +define SVR_GIT_AT_TAG $(GIT_AT_TAG) >> $(GITREV_OUTFILE).tmp
	@echo +define SVR_GIT_LAST_TAG =$(GIT_LAST_TAG)= >> $(GITREV_OUTFILE).tmp
	@echo +define SVR_GIT_COMMIT_COUNT =$(GIT_COMMIT_COUNT)= >> $(GITREV_OUTFILE).tmp
	@echo +define SVR_GIT_COMMIT_HASH =$(GIT_COMMIT_HASH)= >> $(GITREV_OUTFILE).tmp
ifneq (,$(strip $(BUILDINFO)))
	@echo +define SVR_HMDMCU_BUILDINFO =$(BUILDINFO)= >> $(GITREV_OUTFILE).tmp
endif
	$(CAT) $(GITREV_OUTFILE).tmp | $(TR) "+=" "\#\042" > $(GITREV_OUTFILE)
#	@$(SED) -e 's/@GIT_DIRTY@/$(GIT_DIRTY)/' -e 's/@GIT_AT_TAG@/$(GIT_AT_TAG)/' -e 's/@GIT_LAST_TAG@/$(GIT_LAST_TAG)/' -e 's/@GIT_COMMIT_COUNT@/$(GIT_COMMIT_COUNT)/' -e 's/@GIT_COMMIT_HASH@/$(GIT_COMMIT_HASH)/' -e 's/@GIT_NOT_FOUND@/$(GIT_NOT_FOUND)/' gitdefines.h.in > gitdefines.h
.PHONY: gitdefheader
#include $(OUTFILE)
#all:
#	echo $(GIT_CFLAGS) > gitdefines.h
#gitdefheader:
#    @echo #
