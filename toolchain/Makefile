# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the toolchain
#
curdir:=toolchain

# subdirectories to descend into
$(curdir)/builddirs := kernel-headers $(if $(CONFIG_GDB),gdb) $(if $(CONFIG_NATIVE_TOOLCHAIN),,binutils gcc uClibc)
$(curdir)/builddirs-compile:=. $(filter-out kernel-headers,$($(curdir)/builddirs))
$(curdir)/builddirs-install:=$($(curdir)/builddirs-compile)

# builddir dependencies
$(curdir)/uClibc/prepare:=$(curdir)/kernel-headers/prepare
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  $(curdir)/binutils/prepare:=$(curdir)/uClibc/prepare
  $(curdir)/gcc/prepare:=$(curdir)/binutils/install
  $(curdir)/uClibc/compile:=$(curdir)/gcc/compile
  $(curdir)/gcc/install:=$(curdir)/uClibc/compile
  $(curdir)/uClibc/install:=$(curdir)/gcc/install
endif

ifneq ($(ARCH),)
  $(TOOLCHAIN_DIR)/info.mk: .config
	@for dir in $(TOOLCHAIN_DIR); do ( \
		set -x; \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib include stamp; \
	); done
	@grep GCC_VERSION $@ >/dev/null 2>&1 || $(INSTALL_DATA) $(TOPDIR)/toolchain/info.mk $@
	@touch $@
endif

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//prepare = $(STAGING_DIR)/.prepared $(TOOLCHAIN_DIR)/info.mk
$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile

$(TOOLCHAIN_DIR)/stamp/.gcc-initial_installed:

$(eval $(call stampfile,$(curdir),toolchain,install,$(TOOLCHAIN_DIR)/stamp/.gcc-initial_installed))
$(eval $(call subdir,$(curdir)))

