# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Clean
	$(MAKE) -C dfboot clean
	$(MAKE) -C u-boot clean
endef

define Build/Compile
	$(MAKE) -C dfboot compile
	$(MAKE) -C u-boot compile
	$(KDIR)/u-boot-1.1.4/tools/ubparams
	cp params $(KDIR)
endef

define Image/Prepare
  cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
	cp $(KDIR)/dfboot/dfboot.bin $(KDIR)/dfboot.bin
	cp $(KDIR)/dfboot/dfbptest.bin $(KDIR)/dfbptest.bin
	cp $(KDIR)/u-boot-1.1.4/u-boot.bin $(KDIR)/u-boot.bin
	dd if=$(KDIR)/u-boot.bin of=$(KDIR)/u-boot.block bs=100k count=1 conv=sync
	cat $(KDIR)/u-boot.block $(KDIR)/params > $(KDIR)/u-boot.full
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build
	dd if=$(KDIR)/uImage of=$(KDIR)/uImage.block bs=8448 conv=sync
	dd if=$(KDIR)/root.squashfs of=$(KDIR)/root.block bs=8448 conv=sync
	cat $(KDIR)/uImage.block $(KDIR)/root.block > $(KDIR)/knlroot.bin
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/$(IMG_PREFIX).trx -f $(KDIR)/dfboot.bin -f$(KDIR)/u-boot.full -f$(KDIR)/knlroot.bin
	cp $(KDIR)/dfbptest.bin $(BIN_DIR)
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
