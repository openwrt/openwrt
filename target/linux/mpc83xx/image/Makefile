#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk


define Image/Prepare
endef

define Image/BuildKernel
	cp $(LINUX_DIR)/arch/powerpc/boot/dtbImage.rb600.elf $(BIN_DIR)/openwrt-$(BOARD)-rb600.elf
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(LINUX_DIR)/arch/powerpc/boot/uImage bs=1920k conv=sync; \
		dd if=$(KDIR)/openwrt-canyonlands.dtb bs=128k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=256k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-canyonlands-$(1).img
endef

$(eval $(call BuildImage))
