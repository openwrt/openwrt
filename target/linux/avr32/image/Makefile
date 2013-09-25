#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KDIR_TMP:=$(KDIR)/tmp

ifneq ($(CONFIG_AVR32_UBOOT),)
  define Build/Clean
	$(MAKE) -C u-boot clean
  endef

  define Build/Compile
	$(MAKE) -C u-boot compile
  endef
endif

define Image/BuildKernel
	-mkdir -p $(KDIR_TMP)
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage

	$(STAGING_DIR_HOST)/bin/lzma e $(LINUX_DIR)/arch/avr32/boot/images/vmlinux.bin $(KDIR)/vmlinux.lzma
	mkimage -A avr32 -O linux -T kernel -a 0x10000000 -C lzma \
		-e 0x90000000 \
		-n 'OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux.lzma $(KDIR)/uImage-lzma

	cp $(KDIR)/uImage-lzma $(BIN_DIR)/$(IMG_PREFIX)-uImage-lzma
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/squashfs
	( \
		cat $(KDIR)/uImage-lzma; \
		cat $(KDIR)/root.squashfs; \
	) > $(KDIR_TMP)/avr32-squahsfs.img
	padjffs2 $(KDIR_TMP)/avr32-squahsfs.img 64
	dd if=$(KDIR_TMP)/avr32-squahsfs.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(1).img bs=64k conv=sync
endef

define Image/Build/jffs2-64k
	( \
		dd if=$(KDIR)/uImage-lzma bs=64k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
endef

$(eval $(call BuildImage))
