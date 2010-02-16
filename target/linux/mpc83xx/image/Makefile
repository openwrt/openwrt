#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk


define Image/Prepare
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -R 4 -S 0x20000 $(LINUX_DIR)/arch/powerpc/boot/dts/mpc8377_wlan.dts > $(BIN_DIR)/openwrt-mpc83xx-mpc8377_wlan.dtb
endef

define Image/BuildKernel
	cp $(LINUX_DIR)/arch/powerpc/boot/dtbImage.rb600.elf $(BIN_DIR)/openwrt-$(BOARD)-rb600.elf
	cp $(LINUX_DIR)/arch/powerpc/boot/uImage $(BIN_DIR)/openwrt-$(BOARD)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(LINUX_DIR)/arch/powerpc/boot/uImage bs=3072k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=256k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-mpc8377_wlan-$(1).img
endef

$(eval $(call BuildImage))
