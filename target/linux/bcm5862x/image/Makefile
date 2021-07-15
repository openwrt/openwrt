# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2021 Matthew Hagan <mnhagan88@gmail.com>

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/MerakiNAND-old
	-$(STAGING_DIR_HOST)/bin/mkmerakifw-old \
		-B mx64 -s \
		-i $@ \
		-o $@.new
	mv $@.new $@
endef

define Device/Default
  BLOCKSIZE := 32k
  DEVICE_DTS = $$(SOC)-$(lastword $(subst _, ,$(1)))
  KERNEL_NAME := zImage
  PAGESIZE := 2048
endef

define Device/meraki
  DEVICE_PACKAGES := kmod-eeprom-at24
  DEVICE_VENDOR := Cisco Meraki
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | sysupgrade-tar kernel=$$$$@ | append-metadata
  KERNEL := kernel-bin | append-dtb | pad-to $$(BLOCKSIZE) | MerakiNAND-old
  KERNEL_SIZE := 3145728
  SOC := bcm958625
  UBINIZE_OPTS := -E 5
endef

define Device/meraki_mx64
  $(call Device/meraki)
  DEVICE_MODEL := MX64
endef
TARGET_DEVICES += meraki_mx64

define Device/meraki_mx64a0
  $(call Device/meraki)
  DEVICE_MODEL := MX64(A0)
endef
TARGET_DEVICES += meraki_mx64a0

define Device/meraki_mx65
  $(call Device/meraki)
  DEVICE_MODEL := MX65
  DEVICE_PACKAGES += kmod-hwmon-bcm59111
endef
TARGET_DEVICES += meraki_mx65

$(eval $(call BuildImage))
