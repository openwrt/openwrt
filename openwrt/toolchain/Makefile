# Main makefile for the toolchain
include $(TOPDIR)/rules.mk
TARGETS:=sed utils binutils gcc uClibc ipkg-utils gdb

TARGETS_INSTALL:=$(patsubst %,%-install,$(TARGETS))
TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS))

all: install
install: $(TARGETS_INSTALL)
clean: $(TARGETS_CLEAN)

uClibc-prepare: kernel-headers-prepare sed-install utils-install
binutils-prepare: uClibc-prepare
gcc-prepare: binutils-install
uClibc-compile: gcc-prepare
gcc-compile: uClibc-install

$(STAMP_DIR):
	mkdir -p $(STAMP_DIR)

$(STAGING_DIR):
	@mkdir -p $(STAGING_DIR)/lib
	@mkdir -p $(STAGING_DIR)/include
	@mkdir -p $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)
	@ln -sf ../lib $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib

$(TOOL_BUILD_DIR):
	@mkdir -p $(TOOL_BUILD_DIR)

%-prepare: $(STAMP_DIR) $(STAGING_DIR) $(TOOL_BUILD_DIR)
	@[ -f $(STAMP_DIR)/.toolchain_$@ ] || $(MAKE) -C $(patsubst %-prepare,%,$@) prepare
	@touch $(STAMP_DIR)/.toolchain_$@

%-compile: %-prepare 
	@[ -f $(STAMP_DIR)/.toolchain_$@ ] || $(MAKE) -C $(patsubst %-compile,%,$@) compile
	@touch $(STAMP_DIR)/.toolchain_$@

%-install: %-compile
	@[ -f $(STAMP_DIR)/.toolchain_$@ ] || $(MAKE) -C $(patsubst %-install,%,$@) install
	@touch $(STAMP_DIR)/.toolchain_$@

%-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean
	@rm -f $(STAMP_DIR)/.toolchain_$(patsubst %-clean,%,$@)-*

