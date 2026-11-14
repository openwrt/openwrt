# SPDX-License-Identifier: GPL-2.0-only
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR := 0x00008000

define Device/Default
  PROFILES := Default
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
  DEVICE_DTS_DIR := $(DTS_DIR)/cortina
  DEVICE_DTS = $$(SOC)-$(subst _,-,$(1))
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/securifi_almondplus
  DEVICE_VENDOR := Securifi
  DEVICE_MODEL := Almond+
  SOC := cs7542
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  UBINIZE_OPTS := -E 5
  DEVICE_PACKAGES := kmod-ath9k kmod-ath10k ath10k-firmware-qca988x
endef
TARGET_DEVICES += securifi_almondplus

$(eval $(call BuildImage))
