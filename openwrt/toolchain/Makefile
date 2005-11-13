# Main makefile for the toolchain
include $(TOPDIR)/rules.mk
TARGETS:=sed utils binutils gcc uClibc ipkg-utils gdb libnotimpl

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

TOOLCHAIN_STAMP_DIR:=$(STAGING_DIR)/stampfiles

$(TOOLCHAIN_STAMP_DIR):
	mkdir -p $(TOOLCHAIN_STAMP_DIR)

$(STAGING_DIR):
	@mkdir -p $(STAGING_DIR)/lib
	@mkdir -p $(STAGING_DIR)/include
	@mkdir -p $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)
	@ln -sf ../lib $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib

$(TOOL_BUILD_DIR):
	@mkdir -p $(TOOL_BUILD_DIR)

%-prepare: $(TOOLCHAIN_STAMP_DIR) $(STAGING_DIR) $(TOOL_BUILD_DIR)
	@[ -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@ ] || { \
		$(TRACE) toolchain/$(patsubst %-prepare,%,$@)/prepare; \
		$(MAKE) -C $(patsubst %-prepare,%,$@) prepare $(MAKE_TRACE); \
	}
	@touch $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@

%-compile: %-prepare 
	@[ -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@ ] || { \
		$(TRACE) toolchain/$(patsubst %-compile,%,$@)/compile; \
		$(MAKE) -C $(patsubst %-compile,%,$@) compile $(MAKE_TRACE); \
	}
	@touch $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@

%-install: %-compile
	@[ -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@ ] || { \
		$(TRACE) toolchain/$(patsubst %-install,%,$@)/install; \
		$(MAKE) -C $(patsubst %-install,%,$@) install $(MAKE_TRACE); \
	}
	@touch $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@

%-clean:
	$(TRACE) toolchain/$(patsubst %-clean,%,$@)/clean
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean $(MAKE_TRACE)
	@rm -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$(patsubst %-clean,%,$@)-*

