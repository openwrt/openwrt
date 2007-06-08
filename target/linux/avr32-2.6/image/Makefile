# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/avr32/boot/images/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

define Image/Build/jffs2-64k
	( \
		dd if=$(KDIR)/uImage bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

$(eval $(call BuildImage))
