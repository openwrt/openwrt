#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

LOADER		:= adm5120
LOADER_NAME	:= loader-$(LOADER)
LOADER_DATA 	:=

LOADER_BIN	:= $(KDIR)/$(LOADER_NAME).bin
LOADER_GZ	:= $(KDIR)/$(LOADER_NAME).gz
LOADER_ELF	:= $(KDIR)/$(LOADER_NAME).elf

LZMA_STARTUP_ORG:= 0
LZMA_TEXT_START	:= 0x80300000

PKG_NAME := lzma-loader
PKG_BUILD_DIR := $(KDIR)/$(PKG_NAME)

.PHONY : loader-compile

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

loader-compile: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C $(PKG_BUILD_DIR) CROSS_COMPILE="$(TARGET_CROSS)" \
		LZMA_STARTUP_ORG=$(LZMA_STARTUP_ORG) \
		LZMA_TEXT_START=$(LZMA_TEXT_START) \
		LOADER_DATA=$(LOADER_DATA) \
		clean all

$(LOADER_GZ): $(PKG_BUILD_DIR)/loader.bin
	gzip -nc9 $< > $@

$(LOADER_ELF) : $(PKG_BUILD_DIR)/loader.elf
	$(CP) $< $@

$(LOADER_BIN) : $(PKG_BUILD_DIR)/loader.bin
	$(CP) $< $@

download:
prepare: $(PKG_BUILD_DIR)/.prepared
compile: loader-compile $(LOADER_BIN) $(LOADER_GZ) $(LOADER_ELF)

install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(KDIR)/loader-*.gz $(KDIR)/loader-*.elf $(KDIR)/loader-*.bin
