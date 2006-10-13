# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 1823 2005-09-01 20:07:42Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME := loader
PKG_VERSION := 0.05

PKG_BUILD_DIR := $(KDIR)/$(PKG_NAME)-$(PKG_VERSION)

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/lzma.elf: $(PKG_BUILD_DIR)/.prepared $(PKG_BUILD_DIR)/vmlinux.lzma
	PATH="$(TARGET_PATH)" $(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CROSS_COMPILE="$(TARGET_CROSS)" \
		RAMSIZE=$(RAMSIZE) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		IMAGE_COPY=$(IMAGE_COPY)


$(PKG_BUILD_DIR)/vmlinux.lzma: $(KDIR)/vmlinux.lzma
	$(CP) $< $@

$(KDIR)/loader.elf: $(PKG_BUILD_DIR)/lzma.elf
	$(CP) $< $@

download: 
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(KDIR)/loader.elf
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(KDIR)/loader.elf
