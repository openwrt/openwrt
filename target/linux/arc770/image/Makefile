#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# On ARC initramfs is put before entry point and so entry point moves
# in memory from build to built. Thus we need to extract EP from vmlinux
# every time late in building process.
define Build/calculate-ep
	$(eval KERNEL_ENTRY=$(shell $(KERNEL_CROSS)readelf -h $(LINUX_DIR)/vmlinux | grep "Entry point address" | grep -o 0x.*))
endef

define Build/patch-dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb)
	$(STAGING_DIR_HOST)/bin/patch-dtb $@ $@.dtb
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DEVICE_DTS).dts)
  DEVICE_PROFILE :=
  DEVICE_DTS :=
endef
DEVICE_VARS += DEVICE_PROFILE DEVICE_DTS

define Device/vmlinux
  KERNEL_SUFFIX := .elf
  KERNEL_INITRAMFS := kernel-bin | patch-dtb
  KERNEL_INITRAMFS_NAME = vmlinux-initramfs.elf
endef

define Device/uImage
  KERNEL_SUFFIX := .bin
  KERNEL_INITRAMFS := kernel-bin | patch-dtb | calculate-ep | uImage none
  KERNEL_LOADADDR := 0x80000000
endef

define add_arc770_uImage
  define Device/$(1)-uImage
    $(call Device/uImage)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(1)
  endef
  TARGET_DEVICES += $(1)-uImage
endef

define add_arc770_vmlinux
  define Device/$(1)-vmlinux
    $(call Device/vmlinux)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(1)
  endef
  TARGET_DEVICES += $(1)-vmlinux
endef

# DesignWare AXS101
$(eval $(call add_arc770_vmlinux,axs101))
$(eval $(call add_arc770_uImage,axs101))

# nSIM with ARC770
$(eval $(call add_arc770_vmlinux,nsim_700))
$(eval $(call add_arc770_uImage,nsim_700))

$(eval $(call BuildImage))
