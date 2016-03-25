#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/patch-dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb)
	$(STAGING_DIR_HOST)/bin/patch-dtb $@ $@.dtb
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DEVICE_DTS).dts)
  KERNEL_SUFFIX := .elf
  KERNEL_INITRAMFS := kernel-bin | patch-dtb
  KERNEL_INITRAMFS_NAME = $$(KERNEL_NAME)-initramfs.elf
  DEVICE_PROFILE :=
  DEVICE_DTS :=
endef
DEVICE_VARS += DEVICE_PROFILE DEVICE_DTS

define add_arc770
  define Device/$(1)
    DEVICE_PROFILE := $(1)
    DEVICE_DTS := $(1)
  endef
  TARGET_DEVICES += $(1)
endef

# DesignWare AXS101
$(eval $(call add_arc770,axs101))

# nSIM with ARC770
$(eval $(call add_arc770,nsim_700))

$(eval $(call BuildImage))
