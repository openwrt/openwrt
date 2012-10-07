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

# Build sysupgrade image
define BuildFirmware/Generic
	dd if=$(BIN_DIR)/openwrt-$(BOARD)-uImage of=$(KDIR)/uImage.pad bs=64k conv=sync; \
	dd if=$(KDIR)/root.$(1) of=$(KDIR)/root.$(1).pad bs=128k conv=sync; \
	sh $(TOPDIR)/scripts/combined-image.sh \
		$(KDIR)/uImage.pad \
		$(KDIR)/root.$(1).pad \
		$(BIN_DIR)/$(IMG_PREFIX)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))-sysupgrade.bin
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call BuildFirmware/Generic,$(1))
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=64k conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=128k conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=128k conv=sync
endef

$(eval $(call BuildImage))
