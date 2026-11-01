include $(TOPDIR)/include/verbose.mk
include $(TOPDIR)/rules.mk
TMP_DIR:=$(TOPDIR)/tmp

all: $(TMP_DIR)/.$(SCAN_TARGET)

SCAN_TARGET ?= packageinfo
SCAN_NAME ?= package
SCAN_DIR ?= package
TARGET_STAMP:=$(TMP_DIR)/info/.files-$(SCAN_TARGET).stamp
FILELIST:=$(TMP_DIR)/info/.files-$(SCAN_TARGET)-$(SCAN_COOKIE)
OVERRIDELIST:=$(TMP_DIR)/info/.overrides-$(SCAN_TARGET)-$(SCAN_COOKIE)

export ORIG_PATH:=$(if $(ORIG_PATH),$(ORIG_PATH),$(PATH))
export PATH:=$(STAGING_DIR_HOST)/bin:$(PATH)

define feedname
$(if $(patsubst feeds/%,,$(1)),,$(word 2,$(subst /, ,$(1))))
endef

ifeq ($(SCAN_NAME),target)
  SCAN_DEPS=image/Makefile profiles/*.mk $(TOPDIR)/include/kernel*.mk $(TOPDIR)/include/target.mk image/*.mk
else
  SCAN_DEPS=$(TOPDIR)/include/package*.mk $(TOPDIR)/include/rust-*.mk
ifneq ($(call feedname,$(SCAN_DIR)),)
  SCAN_DEPS += $(TOPDIR)/feeds/$(call feedname,$(SCAN_DIR))/*.mk
endif
endif

ifeq ($(IS_TTY),1)
  ifneq ($(strip $(NO_COLOR)),1)
    define progress
	printf "\033[M\r$(1)" >&2;
    endef
  else
    define progress
	printf "\r$(1)" >&2;
    endef
  endif
else
  define progress
	:;
  endef
endif

define PackageDir
  $(TMP_DIR)/.$(SCAN_TARGET): $(TMP_DIR)/info/.$(SCAN_TARGET)-$(1)
  $(TMP_DIR)/info/.$(SCAN_TARGET)-$(1): $(SCAN_DIR)/$(2)/Makefile $(foreach DEP,$(DEPS_$(SCAN_DIR)/$(2)/Makefile) $(SCAN_DEPS),$(wildcard $(if $(filter /%,$(DEP)),$(DEP),$(SCAN_DIR)/$(2)/$(DEP))))
	{ \
		$$(call progress,Collecting $(SCAN_NAME) info: $(SCAN_DIR)/$(2)) \
		echo Source-Makefile: $(SCAN_DIR)/$(2)/Makefile; \
		$(if $(3),echo Override: $(3),true); \
		$(if $(findstring c,$(OPENWRT_VERBOSE)),$(MAKE),$(NO_TRACE_MAKE) --no-print-dir) -r DUMP=1 FEED="$(call feedname,$(2))" -C $(SCAN_DIR)/$(2) $(SCAN_MAKEOPTS) \
			$(if $(findstring c,$(OPENWRT_VERBOSE)),,2>/dev/null) || { \
			mkdir -p "$(TOPDIR)/logs/$(SCAN_DIR)/$(2)"; \
			$(NO_TRACE_MAKE) --no-print-dir -r DUMP=1 FEED="$(call feedname,$(2))" -C $(SCAN_DIR)/$(2) $(SCAN_MAKEOPTS) > $(TOPDIR)/logs/$(SCAN_DIR)/$(2)/dump.txt 2>&1; \
			printf '$(if $(filter 1,$(IS_TTY)),\n)%s\n' "ERROR: please fix $(SCAN_DIR)/$(2)/Makefile - see logs/$(SCAN_DIR)/$(2)/dump.txt for details" >&2; \
			rm -f $$@ $$@.tmp; \
			exit 1; \
		}; \
		echo; \
	} > $$@.tmp
	mv $$@.tmp $$@
endef

$(OVERRIDELIST):
	rm -f $(TMP_DIR)/info/.overrides-$(SCAN_TARGET)-*
	touch $@

ifeq ($(SCAN_NAME),target)
  GREP_STRING=BuildTarget
else
  GREP_STRING=(Build/DefaultTargets|BuildPackage|BuildContainer|BuildCompose|KernelPackage)
endif

$(FILELIST): $(OVERRIDELIST)
	rm -f $(TMP_DIR)/info/.files-$(SCAN_TARGET)-*
	set -o pipefail; \
	trap 'rm -f $@.tmp $(OVERRIDELIST).tmp' EXIT; \
	: > $(OVERRIDELIST).tmp || exit 1; \
	find -L $(SCAN_DIR) -mindepth 1 $(if $(SCAN_DEPTH),-maxdepth $(SCAN_DEPTH)) $(SCAN_EXTRA) -name Makefile | \
		xargs -r sh -c 'grep -aHE "$$@" || [ $$? -eq 1 ]' -- 'call $(GREP_STRING)' | \
		sed -e 's#^$(SCAN_DIR)/##' -e 's#/Makefile:.*##' | uniq | \
		awk -v of=$(OVERRIDELIST).tmp -f include/scan.awk > $@.tmp || exit 1; \
	mv $(OVERRIDELIST).tmp $(OVERRIDELIST) && mv $@.tmp $@

$(TMP_DIR)/info/.files-$(SCAN_TARGET).mk: $(FILELIST)
	( \
		set -o pipefail; \
		awk '{print "$(SCAN_DIR)/" $$0 "/Makefile" }' $< | \
			xargs -r sh -c 'grep -HE "$$@" || [ $$? -eq 1 ]' -- '^ *SCAN_DEPS *= *' | \
			awk -F: '{ gsub(/^.*DEPS *= */, "", $$2); print "DEPS_" $$1 "=" $$2 }' || exit 1; \
		awk -F/ -v deps="$$DEPS" -v of="$(OVERRIDELIST)" ' \
		BEGIN { \
			while (getline < (of)) \
				override[$$NF]=$$0; \
			close(of) \
		} \
		{ \
			info=$$0; \
			gsub(/\//, "_", info); \
			dir=$$0; \
			pkg=""; \
			if($$NF in override) \
				pkg=override[$$NF]; \
			print "$$(eval $$(call PackageDir," info "," dir "," pkg "))"; \
		} ' < $<; \
	) > $@.tmp || { rm -f $@.tmp; exit 1; }
	mv $@.tmp $@

include $(TMP_DIR)/info/.files-$(SCAN_TARGET).mk

$(TARGET_STAMP)::
	+( \
		set -o pipefail; \
		$(NO_TRACE_MAKE) $(FILELIST) || exit 1; \
		MD5SUM=$$(cat $(FILELIST) $(OVERRIDELIST) | $(MKHASH) md5 | awk '{print $$1}') || exit 1; \
		[ -f "$@.$$MD5SUM" ] || { \
			rm -f $@.*; \
			touch $@.$$MD5SUM; \
			touch $@; \
		} \
	)

$(TMP_DIR)/.$(SCAN_TARGET): $(TARGET_STAMP)
	$(call progress,Collecting $(SCAN_NAME) info: merging...)
	set -o pipefail; \
	trap 'rm -f $@.tmp' EXIT; \
	cat $(FILELIST) | awk '{gsub(/\//, "_", $$0);print "$(TMP_DIR)/info/.$(SCAN_TARGET)-" $$0}' | \
		xargs -r cat > $@.tmp && mv $@.tmp $@
	$(call progress,Collecting $(SCAN_NAME) info: done)
	echo

FORCE:
.PHONY: FORCE
.NOTPARALLEL:
