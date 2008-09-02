include $(TOPDIR)/include/verbose.mk
TMP_DIR:=$(TOPDIR)/tmp

all: $(TMP_DIR)/.$(SCAN_TARGET)

include $(TOPDIR)/include/host.mk

SCAN_TARGET ?= packageinfo
SCAN_NAME ?= package
SCAN_DIR ?= package
TARGET_STAMP:=$(TMP_DIR)/info/.files-$(SCAN_TARGET).stamp
FILELIST:=$(TMP_DIR)/info/.files-$(SCAN_TARGET)-$(SCAN_COOKIE)

ifeq ($(IS_TTY),1)
  define progress
	printf "\033[M\r$(1)" >&2;
  endef
else
  define progress
	:;
  endef
endif

define PackageDir
  $(TMP_DIR)/.$(SCAN_TARGET): $(TMP_DIR)/info/.$(SCAN_TARGET)-$(1)
  $(TMP_DIR)/info/.$(SCAN_TARGET)-$(1): $(SCAN_DIR)/$(2)/Makefile $(SCAN_STAMP) $(foreach DEP,$(DEPS_$(SCAN_DIR)/$(1)/Makefile) $(SCAN_DEPS),$(wildcard $(if $(filter /%,$(DEP)),$(DEP),$(SCAN_DIR)/$(1)/$(DEP))))
	{ \
		$$(call progress,Collecting $(SCAN_NAME) info: $(SCAN_DIR)/$(2)) \
		echo Source-Makefile: $(SCAN_DIR)/$(2)/Makefile; \
		$(NO_TRACE_MAKE) --no-print-dir DUMP=1 -C $(SCAN_DIR)/$(2) $(SCAN_MAKEOPTS) 2>/dev/null || echo "ERROR: please fix $(SCAN_DIR)/$(2)/Makefile" >&2; \
		echo; \
	} > $$@ || true
endef

$(FILELIST):
	rm -f $(TMP_DIR)/info/.files-$(SCAN_TARGET)-*
	$(call FIND_L, $(SCAN_DIR)) $(SCAN_EXTRA) -mindepth 1 $(if $(SCAN_DEPTH),-maxdepth $(SCAN_DEPTH)) -name Makefile | xargs grep -HE 'call (Build/DefaultTargets|Build(Package|Target)|.+Package)' | sed -e 's#^$(SCAN_DIR)/##' -e 's#/Makefile:.*##' | uniq > $@

$(TMP_DIR)/info/.files-$(SCAN_TARGET).mk: $(FILELIST)
	( \
		cat $< | awk '{print "$(SCAN_DIR)/" $$0 "/Makefile" }' | xargs grep -HE '^ *SCAN_DEPS *= *' | awk -F: '{ gsub(/^.*DEPS *= */, "", $$2); print "DEPS_" $$1 "=" $$2 }'; \
		awk -v deps="$$DEPS" '{ \
			info=$$0; \
			gsub(/\//, "_", info); \
			print "$$(eval $$(call PackageDir," info "," $$0 "))"; \
		} ' < $<; \
		true; \
	) > $@

-include $(TMP_DIR)/info/.files-$(SCAN_TARGET).mk

$(TARGET_STAMP)::
	+( \
		$(NO_TRACE_MAKE) $(FILELIST); \
		MD5SUM=$$(cat $(FILELIST) | (md5sum || md5) 2>/dev/null | awk '{print $$1}'); \
		[ -f "$@.$$MD5SUM" ] || { \
			rm -f $@.*; \
			touch $@.$$MD5SUM; \
			touch $@; \
		} \
	)

$(TMP_DIR)/.$(SCAN_TARGET): $(TARGET_STAMP) $(SCAN_STAMP)
	$(call progress,Collecting $(SCAN_NAME) info: merging...)
	cat $(FILELIST) | awk '{gsub(/\//, "_", $$0);print "$(TMP_DIR)/info/.$(SCAN_TARGET)-" $$0}' | xargs cat > $@
	$(call progress,Collecting $(SCAN_NAME) info: done)
	echo

FORCE:
.PHONY: FORCE
.NOTPARALLEL:
