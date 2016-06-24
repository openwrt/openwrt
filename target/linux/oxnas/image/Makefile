#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

Default_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
STG212_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
KD20_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
POGOPLUG_PRO_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
POGOPLUG_V3_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096

DEVICE_VARS += DTS KERNEL_SIZE PAGESIZE BLOCKSIZE SUBPAGESIZE
DEVICE_VARS += KERNEL_IN_UBI UBOOTENV_IN_UBI UBIFS_OPTS

KERNEL_LOADADDR := 0x60008000

define Build/ubootable
	(dd if="$(KDIR)/u-boot.bin" bs=128k conv=sync; \
	 dd if="$@" bs=128k conv=sync ) >> $@.new
	@mv "$@.new" "$@"
endef

define Device/Default
  KERNEL_DEPENDS = $$(wildcard $$(DTS_DIR)/ox820-$$(DTS).dts)
  KERNEL = kernel-bin | lzma | fit lzma $$(DTS_DIR)/ox820-$$(DTS).dtb
  KERNEL_NAME := zImage
  KERNEL_SUFFIX := -uImage
  KERNEL_INSTALL := 1
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma $$(DTS_DIR)/ox820-$$(DTS).dtb | ubootable
  KERNEL_INITRAMFS_PREFIX = $$(IMAGE_PREFIX)-u-boot-initramfs
  KERNEL_INITRAMFS_SUFFIX := .bin
  BLOCKSIZE := 128KiB
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  FILESYSTEMS := squashfs ubifs
  PROFILES = Default $$(DTS)
  IMAGES := ubinized.bin sysupgrade.tar
  IMAGE/ubinized.bin := append-ubi
  IMAGE/sysupgrade.tar := sysupgrade-nand
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
endef

define Device/akitio
  DTS := akitio
  DEVICE_TITLE := Akitio MyCloud mini / Silverstone DC01
  DEVICE_PACKAGES := kmod-i2c-gpio kmod-rtc-ds1307
endef
TARGET_DEVICES += akitio

define Device/kd20
  DTS := kd20
  DEVICE_TITLE := Shuttle KD20
  DEVICE_PACKAGES := kmod-usb3 kmod-i2c-gpio kmod-rtc-pcf8563 kmod-gpio-beeper \
                     kmod-hwmon-core kmod-hwmon-gpiofan
endef
TARGET_DEVICES += kd20

define Device/pogoplug-pro
  DTS := pogoplug-pro
  DEVICE_TITLE := Cloud Engines Pogoplug Pro (with mPCIe)
endef
TARGET_DEVICES += pogoplug-pro

define Device/pogoplug-v3
  DTS := pogoplug-v3
  DEVICE_TITLE := Cloud Engines Pogoplug V3 (no mPCIe)
endef
TARGET_DEVICES += pogoplug-v3

define Device/stg212
  DTS := stg212
  DEVICE_TITLE := MitraStar STG-212
endef
TARGET_DEVICES += stg212

VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux
UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage

$(eval $(call BuildImage))
