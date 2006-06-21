# Main makefile for the toolchain
include $(TOPDIR)/rules.mk
TARGETS-y:=sed kernel-headers utils binutils gcc uClibc ipkg-utils libnotimpl squashfs jffs2 lzma
TARGETS-$(CONFIG_GDB) += gdb

TARGETS_DOWNLOAD:=$(patsubst %,%-source,$(TARGETS-y))
TARGETS_INSTALL:=$(patsubst %,%-install,$(TARGETS-y))
TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS-y))

all: install
download: $(TARGETS_DOWNLOAD)
install: $(TARGETS_INSTALL)
clean: $(TARGETS_CLEAN)

uClibc-prepare: kernel-headers-prepare sed-install utils-install
binutils-prepare: uClibc-prepare
gcc-prepare: binutils-install
uClibc-compile: gcc-prepare
gcc-compile: uClibc-install
squashfs-compile: lzma-compile

TOOLCHAIN_STAMP_DIR:=$(STAGING_DIR)/stampfiles

$(TOOLCHAIN_STAMP_DIR):
	mkdir -p $@

$(STAGING_DIR):
	@mkdir -p $@/lib
	@mkdir -p $@/include
	@mkdir -p $@/$(REAL_GNU_TARGET_NAME)
	@ln -sf ../lib $@/$(REAL_GNU_TARGET_NAME)/lib

$(TOOL_BUILD_DIR):
	@mkdir -p $@

%-source: FORCE
	$(MAKE) -C $(patsubst %-source,%,$@) source

%-prepare: $(TOOLCHAIN_STAMP_DIR) $(STAGING_DIR) $(TOOL_BUILD_DIR) FORCE
	@[ -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@ ] || { \
		$(MAKE) -C $(patsubst %-prepare,%,$@) prepare; \
	}
	@touch $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@

%-compile: %-prepare 
	@[ -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@ ] || { \
		$(MAKE) -C $(patsubst %-compile,%,$@) compile; \
	}
	@touch $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@

%-install: %-compile
	@[ -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@ ] || { \
		$(MAKE) -C $(patsubst %-install,%,$@) install; \
	}
	@touch $(TOOLCHAIN_STAMP_DIR)/.toolchain_$@

%-clean: FORCE
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean
	@rm -f $(TOOLCHAIN_STAMP_DIR)/.toolchain_$(patsubst %-clean,%,$@)-*

