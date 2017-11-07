#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR := 0x80400000

define Build/dtb-bin
	$(INSTALL_DIR) $(dir $(DTS_DIR)/$(DEVICE_DTS))
	$(call Image/BuildDTB,$(DEVICE_DTS_DIR)/$(DEVICE_DTS).dts,\
				$(DTS_DIR)/$(DEVICE_DTS).dtb)
endef

define Device/Default
	DEVICE_DTS_DIR := ../dts
	KERNEL_DEPENDS = $$(wildcard $$(DEVICE_DTS_DIR)/$$(DEVICE_DTS).dts)
	KERNEL = dtb-bin | kernel-bin | gzip | fit gzip $(DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_PREFIX := $$(IMAGE_PREFIX)
	KERNEL_SUFFIX := -kernel.itb
	KERNEL_INSTALL := 1
	KERNEL_IN_UBI := 1
	FILESYSTEMS := squashfs ubifs
	IMAGES := factory.ubi sysupgrade.tar
	IMAGE/factory.ubi := append-ubi
	IMAGE/sysupgrade.tar := sysupgrade-nand
endef
DEVICE_VARS += DEVICE_DTS_DIR DEVICE_DTS KERNEL_IN_UBI

define Device/marduk
	PROFILES := marduk
	DEVICE_DTS := pistachio/pistachio_$(PROFILE)
	BLOCKSIZE := 256KiB
	PAGESIZE := 4KiB
endef
TARGET_DEVICES += marduk

$(eval $(call BuildImage))
