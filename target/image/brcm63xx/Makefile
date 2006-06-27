# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x8108c8f4		# RAM start + 16M 
KERNEL_ENTRY = 0x80100000	# Default kernel entry in arch/mips/Makefile
RAMSIZE = 0x01000000		# 64MB

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE)

define Build/Clean
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) compile
endef

define Image/Build
	cp $(KDIR)/vmlinux $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

$(eval $(call BuildImage))
