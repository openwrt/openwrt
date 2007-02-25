# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/ppc/boot/images/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-64k
	( \
		dd if=$(KDIR)/uImage bs=65536 conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=65536 conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

#define Image/Build/jffs2-128k
#	( \
#		dd if=$(LINUX_DIR)/arch/ppc/boot/images/uImage bs=131072 conv=sync; \
#		dd if=$(KDIR)/root.$(1) bs=131072 conv=sync; \
#	) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
#endef

$(eval $(call BuildImage))
