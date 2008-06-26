#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
# WRT350N v2: mach id 1633 (0x661)
	echo -en "\x06\x1c\xa0\xe3\x61\x10\x81\xe3" > $(KDIR)/wrt350nv2-zImage
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/wrt350nv2-zImage
	$(STAGING_DIR_HOST)/bin/mkimage -A arm -O linux -T kernel \
	-C none -a 0x00008000 -e 0x00008000 -n 'Linux-$(LINUX_VERSION)' \
	-d $(KDIR)/wrt350nv2-zImage $(KDIR)/wrt350nv2-uImage
	cp $(KDIR)/wrt350nv2-uImage $(BIN_DIR)/openwrt-wrt350nv2-uImage
# WNR854T: mach id 1801 (0x709)
	echo -en "\x07\x1c\xa0\xe3\x09\x10\x81\xe3" > $(KDIR)/wnr854t-zImage
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/wnr854t-zImage
	$(STAGING_DIR_HOST)/bin/mkimage -A arm -O linux -T kernel \
	-C none -a 0x00008000 -e 0x00008000 -n 'Linux-$(LINUX_VERSION)' \
	-d $(KDIR)/wnr854t-zImage $(KDIR)/wnr854t-uImage
	cp $(KDIR)/wnr854t-uImage $(BIN_DIR)/openwrt-wnr854t-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).img
endef

$(eval $(call BuildImage))
