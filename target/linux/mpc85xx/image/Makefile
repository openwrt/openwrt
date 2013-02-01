#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DTS_TARGETS = mpc8548cds pb1010rdb

define Image/Prepare
	cp $(LINUX_DIR)/arch/powerpc/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
	cp $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
	$(foreach dts,$(DTS_TARGETS),
		$(LINUX_DIR)/scripts/dtc/dtc -I dts -O dtb $(LINUX_DIR)/arch/powerpc/boot/dts/$(dts).dts > $(BIN_DIR)/$(IMG_PREFIX)-$(dts).fdt
	)
endef

define Image/Build/ext2
	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-ext2.img
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
