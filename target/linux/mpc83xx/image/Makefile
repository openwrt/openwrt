#
# Copyright (C) 2009-2010 OpenWrt.org
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
		dd if=$(LINUX_DIR)/arch/powerpc/boot/uImage.fit.mpc8377_wlan bs=3072k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=256k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-mpc8377_wlan-$(1).img
endef

$(eval $(call BuildImage))
