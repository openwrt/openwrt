# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the toolchain
#

# subdirectories to descend into
toolchain/builddirs := kernel-headers $(if $(CONFIG_GDB),gdb) $(if $(CONFIG_NATIVE_TOOLCHAIN),,binutils gcc uClibc)

# builddir dependencies
toolchain/uClibc/prepare:=toolchain/kernel-headers/prepare
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  toolchain/binutils/prepare:=toolchain/uClibc/prepare
  toolchain/gcc/prepare:=toolchain/binutils/install
  toolchain/uClibc/compile:=toolchain/gcc/compile
endif
toolchain/gcc/install:=toolchain/uClibc/install

# prerequisites for the individual targets
toolchain/ := .config $(tools/stamp)
toolchain//prepare = $(STAGING_DIR)/include-host/.done
toolchain//compile = $(1)/prepare
toolchain//install = $(1)/compile

$(eval $(call stampfile,toolchain,toolchain))
$(eval $(call subdir,toolchain))

