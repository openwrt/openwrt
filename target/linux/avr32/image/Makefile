#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

ifneq ($(CONFIG_AVR32_UBOOT),)
  define Build/Clean
	$(MAKE) -C u-boot clean
  endef

  define Build/Compile
	$(MAKE) -C u-boot compile
  endef
endif

define Image/Prepare
	cp $(LINUX_DIR)/arch/avr32/boot/images/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-uImage

	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.lzma
	mkimage -A avr32 -O linux -T kernel -a 0x10000000 -C lzma \
		-e 0x90000000 \
		-n 'OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux.lzma $(KDIR)/uImage-lzma

	cp $(KDIR)/uImage-lzma $(BIN_DIR)/openwrt-$(BOARD)-uImage-lzma
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).img
endef

define Image/Build/jffs2-64k
	( \
		dd if=$(KDIR)/uImage bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).img
endef

$(eval $(call BuildImage))
