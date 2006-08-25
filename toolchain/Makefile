# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the toolchain
#
include $(TOPDIR)/rules.mk
TARGETS-y:=sed kernel-headers sstrip
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  TARGETS-y+=binutils gcc uClibc
endif
TARGETS-y+=ipkg-utils libnotimpl ext2fs squashfs jffs2 lzma mkimage

TARGETS-$(CONFIG_GDB) += gdb

TARGETS_DOWNLOAD:=$(patsubst %,%-download,$(TARGETS-y))
TARGETS_INSTALL:=$(patsubst %,%-install,$(TARGETS-y))
TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS-y))

all: install
download: $(TARGETS_DOWNLOAD)
install: $(TARGETS_INSTALL)
clean: $(TARGETS_CLEAN)

kernel-headers-prepare: sed-install
uClibc-prepare: kernel-headers-prepare sstrip-install
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  binutils-prepare: uClibc-prepare
  gcc-prepare: binutils-install
  uClibc-compile: gcc-compile
endif
gcc-install: uClibc-install
squashfs-compile: lzma-install

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

%-download: FORCE
	$(MAKE) -C $(patsubst %-download,%,$@) download

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

