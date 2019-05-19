#
# Auxiliary kernel loader for Qualcom IPQ-4XXX/806X based boards
#
# Copyright (C) 2019 Sergey Sergeev <adron@mstnt.com>
#
# Some parts of this code was based on the OpenWrt specific lzma-loader
# for the Atheros AR7XXX/AR9XXX based boards:
#	Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#

include $(TOPDIR)/rules.mk

LOADER_NAME	:= $(basename $(notdir $(LOADER)))

ifeq ($(TARGET_DIR),)
TARGET_DIR	:= $(KDIR)
endif

LOADER_ELF	:= $(TARGET_DIR)/$(LOADER_NAME).elf
LOADER_UBI	:= $(TARGET_DIR)/$(LOADER_NAME).ubi

PKG_NAME := ipq-aux-loader
PKG_BUILD_DIR := $(KDIR)/$(PKG_NAME)

.PHONY : loader-compile loader.elf

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	touch $@

CPU_TYPE := UNKNOWN

ifeq ($(PLATFORM),mikrotik-ipq4xxx)
  CPU_TYPE := IPQ4XXX
  TEXT_BASE := 0x84800000
  TEXT_BASE2 := 0x01100000
  FAT_SIZE_START := 5000000
  TEXT_BASE2_FAT := 0x00000000
endif

loader-compile: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C $(PKG_BUILD_DIR) CROSS_COMPILE="$(TARGET_CROSS)" \
		TEXT_BASE=$(TEXT_BASE) TEXT_BASE2=$(TEXT_BASE2) 					\
		FAT_SIZE_START=$(FAT_SIZE_START)													\
		TEXT_BASE2_FAT=$(TEXT_BASE2_FAT) 													\
		CPU_TYPE=$(CPU_TYPE) KERNEL_IMAGE=$(LOADER_DATA)					\
		BLOCKSIZE=$(BLOCKSIZE) PAGESIZE=$(PAGESIZE) 							\
		clean $(LOADER_TYPE)

loader.elf: $(PKG_BUILD_DIR)/bin/loader.elf
	$(CP) $< $(LOADER_ELF)

loader.ubi: $(PKG_BUILD_DIR)/bin/loader.ubi
	$(CP) $< $(LOADER_UBI)

download:
prepare: $(PKG_BUILD_DIR)/.prepared
compile: loader-compile

install:

clean:
	rm -rf $(PKG_BUILD_DIR)
