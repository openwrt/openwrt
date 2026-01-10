#
# Copyright (C) 2011 OpenWrt.org
# Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

LZMA_TEXT_START	:= 0x80002000
LOADER		:= loader.bin
LOADER_NAME	:= $(basename $(notdir $(LOADER)))
LOADER_DATA 	:=
TARGET_DIR	:=
FLASH_START	:=
FLASH_OFFS	:=
FLASH_MAX	:=
BOARD		:=
PLATFORM	:=
SUBTARGET	:=
CACHE_FLAGS    := -DCONFIG_CACHELINE_SIZE=32

# NAND boot configuration (optional, defaults in nand_boot.h)
NAND_BOOT		:=
MAX_BBT_SIZE		:=
NAND_OS1_OFFSET		:=
NAND_OS2_OFFSET		:=
NAND_BOOT_FLAG_OFFSET	:=
NAND_BOOT_FLAG_OS1	:=
NAND_BOOT_FLAG_OS2	:=
NAND_BOOT_FLAG_MASK	:=
NAND_CMDLINE_OS1	:=
NAND_CMDLINE_OS2	:=

ifeq ($(TARGET_DIR),)
TARGET_DIR	:= $(KDIR)
endif

LOADER_BIN	:= $(TARGET_DIR)/$(LOADER_NAME).bin
LOADER_GZ	:= $(TARGET_DIR)/$(LOADER_NAME).gz
LOADER_ELF	:= $(TARGET_DIR)/$(LOADER_NAME).elf

PKG_NAME := lzma-loader
PKG_BUILD_DIR := $(KDIR)/$(PKG_NAME)

.PHONY : loader-compile loader.bin loader.elf loader.gz

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

loader-compile: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C $(PKG_BUILD_DIR) CROSS_COMPILE="$(TARGET_CROSS)" \
		LZMA_TEXT_START=$(LZMA_TEXT_START) \
		LOADER_DATA=$(LOADER_DATA) \
		FLASH_START=$(FLASH_START) \
		FLASH_OFFS=$(FLASH_OFFS) \
		FLASH_MAX=$(FLASH_MAX) \
		BOARD="$(BOARD)" \
		PLATFORM="$(PLATFORM)" \
		SUBTARGET="$(SUBTARGET)" \
		CACHE_FLAGS="$(CACHE_FLAGS)" \
		NAND_BOOT="$(NAND_BOOT)" \
		MAX_BBT_SIZE="$(MAX_BBT_SIZE)" \
		NAND_OS1_OFFSET="$(NAND_OS1_OFFSET)" \
		NAND_OS2_OFFSET="$(NAND_OS2_OFFSET)" \
		NAND_BOOT_FLAG_OFFSET="$(NAND_BOOT_FLAG_OFFSET)" \
		NAND_BOOT_FLAG_OS1="$(NAND_BOOT_FLAG_OS1)" \
		NAND_BOOT_FLAG_OS2="$(NAND_BOOT_FLAG_OS2)" \
		NAND_BOOT_FLAG_MASK="$(NAND_BOOT_FLAG_MASK)" \
		NAND_CMDLINE_OS1="$(NAND_CMDLINE_OS1)" \
		NAND_CMDLINE_OS2="$(NAND_CMDLINE_OS2)" \
		clean all

loader.gz: $(PKG_BUILD_DIR)/loader.bin
	gzip -nc9 $< > $(LOADER_GZ)

loader.elf: $(PKG_BUILD_DIR)/loader.elf
	$(CP) $< $(LOADER_ELF)

loader.bin: $(PKG_BUILD_DIR)/loader.bin
	$(CP) $< $(LOADER_BIN)

download:
prepare: $(PKG_BUILD_DIR)/.prepared
compile: loader-compile

install:

clean:
	rm -rf $(PKG_BUILD_DIR)

