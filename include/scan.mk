include $(TOPDIR)/include/verbose.mk

SCAN_TARGET ?= pkginfo
SCAN_NAME ?= package
SCAN_DIR ?= package
SCAN_DEPS ?= include/package.mk

ifeq ($(IS_TTY),1)
  define progress
	printf "\033[M\r$(1)" >&2;
  endef
else
  define progress
	:
  endef
endif

SCAN = $(patsubst $(SCAN_DIR)/%/Makefile,%,$(shell ls $(SCAN_DIR)/*/Makefile))
tmp/.$(SCAN_TARGET):
	@($(call progress,Collecting $(SCAN_NAME) info: merging...))
	for file in $(SCAN); do \
		cat tmp/info/.$(SCAN_TARGET)-$$file; \
	done > $@
	@($(call progress,Collecting $(SCAN_NAME) info: done))
	@echo

define scanfiles
$(foreach FILE,$(SCAN),
  tmp/.$(SCAN_TARGET): tmp/info/.$(SCAN_TARGET)-$(FILE) FORCE
  tmp/info/.$(SCAN_TARGET)-$(FILE): $(SCAN_DEPS) $(SCAN_DIR)/$(FILE)/Makefile
	{ \
		$$(call progress,Collecting $(SCAN_NAME) info: $(SCAN_DIR)/$(FILE)) \
		echo Source-Makefile: $(SCAN_DIR)/$(FILE)/Makefile; \
		$(NO_TRACE_MAKE) --no-print-dir DUMP=1 -C $(SCAN_DIR)/$(FILE) 3>/dev/null || echo "ERROR: please fix $(SCAN_DIR)/$(FILE)/Makefile" >&2; \
		echo; \
	} > $$@
)

endef
$(eval $(call scanfiles))

FORCE:
.PHONY: FORCE
