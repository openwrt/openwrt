# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Clean
	$(MAKE) -C romboot clean
	$(MAKE) -C u-boot clean
endef

define Build/Compile
	$(MAKE) -C romboot compile
#	$(MAKE) -C u-boot configure
	$(MAKE) -C u-boot compile
endef

define Image/Prepare
  cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
	cp $(KDIR)/romboot/romboot.bin $(KDIR)/romboot.bin
	cp $(KDIR)/u-boot-1.1.4/u-boot.bin $(KDIR)/u-boot.bin
	cat $(KDIR)/romboot/romboot.block $(KDIR)/u-boot-1.1.4/u-boot.block > $(KDIR)/loaders.block
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
