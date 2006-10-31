# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the toolchain
#
include $(TOPDIR)/rules.mk

TARGETS-y:=kernel-headers
TARGETS-$(CONFIG_GDB) += gdb
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  TARGETS-y+=binutils gcc uClibc
endif

TARGETS_DOWNLOAD:=$(patsubst %,%-download,$(TARGETS-y))
TARGETS_INSTALL:=$(patsubst %,%-install,$(TARGETS-y))
TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS-y))

all: install
download: $(TARGETS_DOWNLOAD)
install: $(TARGETS_INSTALL)
clean: $(TARGETS_CLEAN)
$(TARGETS_INSTALL): $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)

uClibc-prepare: kernel-headers-prepare
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  binutils-prepare: uClibc-prepare
  gcc-prepare: binutils-install
  uClibc-compile: gcc-compile
endif
gcc-install: uClibc-install

TOOLCHAIN_STAMP_DIR:=$(STAGING_DIR)/stampfiles

$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME):
	@mkdir -p $@
	@ln -sf ../lib $@/lib

$(TOOLCHAIN_STAMP_DIR):
	mkdir -p $@

$(TOOLCHAIN_BUILD_DIR):
	@mkdir -p $@

%-download: FORCE
	$(MAKE) -C $(patsubst %-download,%,$@) download

%-prepare: $(TOOLCHAIN_STAMP_DIR) $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME) $(TOOLCHAIN_BUILD_DIR) FORCE
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

