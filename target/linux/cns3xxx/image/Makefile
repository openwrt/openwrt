# 
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define mkimage
	mkimage -A arm -O linux -T kernel -C none -a $(2) -e $(2) -n 'OpenWrt Linux-$(LINUX_VERSION)' -d $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/uImage-$(1)
endef

define Image/Prepare
	$(call mkimage,old,0x00008000)
	$(call mkimage,new,0x20008000)
endef

define Image/BuildKernel
	cp $(KDIR)/uImage-old $(BIN_DIR)/openwrt-$(BOARD)-old-uboot-uImage
	cp $(KDIR)/uImage-new $(BIN_DIR)/openwrt-$(BOARD)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=65536 conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=131072 conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=131072 conv=sync
endef

$(eval $(call BuildImage))
