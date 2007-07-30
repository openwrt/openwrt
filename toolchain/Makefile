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
endif
$(curdir)/gcc/install:=$(curdir)/uClibc/install

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//prepare = $(STAGING_DIR)/include-host/.done
$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),toolchain,install))
$(eval $(call subdir,$(curdir)))

