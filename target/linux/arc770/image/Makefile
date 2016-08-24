#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# On ARC initramfs is put before entry point and so entry point moves
# in memory from build to built. Thus we need to extract EP from vmlinux
# every time before generation of uImage.
define Build/calculate-ep
	$(eval KERNEL_ENTRY=$(shell $(KERNEL_CROSS)readelf -h $(LINUX_DIR)/vmlinux | grep "Entry point address" | grep -o 0x.*))
endef

define Build/patch-dtb
	$(call Image/BuildDTB,$(DTS_DIR)/$(DEVICE_DTS).dts,$@.dtb)
	$(STAGING_DIR_HOST)/bin/patch-dtb $@ $@.dtb
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DEVICE_DTS).dts)
  DEVICE_PROFILE :=
endef
DEVICE_VARS += DEVICE_PROFILE

define Device/vmlinux
  KERNEL_SUFFIX := .elf
  KERNEL_INITRAMFS := kernel-bin | patch-dtb
  KERNEL_INITRAMFS_NAME = vmlinux-initramfs.elf
endef

define Device/uImage
  KERNEL_SUFFIX := -uImage
  KERNEL_INITRAMFS := kernel-bin | patch-dtb | calculate-ep | uImage none
  KERNEL_LOADADDR := 0x80000000
endef

define Device/axs101
	$(call Device/uImage)
	DEVICE_PROFILE := axs101
	DEVICE_DTS := axs101
endef
TARGET_DEVICES += axs101

define Device/nsim
	$(call Device/vmlinux)
	DEVICE_PROFILE := nsim
	DEVICE_DTS := nsim_700
endef
TARGET_DEVICES += nsim

$(eval $(call BuildImage))
