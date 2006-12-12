# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x81000000		# RAM start + 16M 
KERNEL_ENTRY = 0x80100000
RAMSIZE = 0x00100000		# 1MB

FLASH_KERNEL := 0xBFD00000
FLASH_FS     := 0xBE000000

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE)

define Build/Clean
	$(MAKE) -C ../../generic-2.6/image/lzma-loader $(LOADER_MAKEOPTS) clean
endef

DROP_SECTIONS := .reginfo .mdebug .comment .note .pdr .options .MIPS.options
OBJCOPY_SREC := $(TARGET_CROSS)objcopy -S -O srec $(addprefix --remove-section=,$(DROP_SECTIONS))
OBJCOPY_BIN := $(TARGET_CROSS)objcopy -S -O binary $(addprefix --remove-section=,$(DROP_SECTIONS))

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	
	# Build RAM image
	$(MAKE) -C ../../generic-2.6/image/lzma-loader \
		$(LOADER_MAKEOPTS) \
		clean compile
	$(OBJCOPY_SREC) $(KDIR)/loader.elf $(KDIR)/kernel.ram.srec
	
	# Build Flash image
	$(MAKE) -C ../../generic-2.6/image/lzma-loader \
		$(LOADER_MAKEOPTS) \
		IMAGE_COPY=1 \
		LOADER_ENTRY=$(FLASH_KERNEL) \
		LOADER_TYPE=_flash \
		clean compile
	$(OBJCOPY_SREC) $(KDIR)/loader_flash.elf $(KDIR)/kernel.flash.srec
	$(OBJCOPY_BIN) $(KDIR)/loader_flash.elf $(KDIR)/kernel.flash.bin
endef

define Image/Build
	$(TARGET_CROSS)objcopy -O srec -I binary --adjust-vma $(FLASH_FS) $(KDIR)/root.$(1) $(KDIR)/root.$(1).srec
	grep -v S7 $(KDIR)/root.$(1).srec > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
	grep -v S0 $(KDIR)/kernel.flash.srec >> $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
	$(CP) $(KDIR)/kernel.flash.bin $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux.bin
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).fs
endef

$(eval $(call BuildImage))
