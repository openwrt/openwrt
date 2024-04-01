# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

define Device/Default-emmc
  $(Device/Default)
  FILESYSTEMS += squashfs ext4
  DEVICE_DTS := $(1)
  IMAGES := boot.img rootfs.img
  IMAGE/rootfs.img := append-rootfs | pad-extra 128k
  IMAGE/boot.img := resource-img $(1)
endef

define Device/Default-spiflash
  $(Device/Default)
  FILESYSTEMS += squashfs jffs2
  DEVICE_DTS := $(1)
  IMAGES := boot.img rootfs.img
  IMAGE/rootfs.img := append-rootfs | pad-extra 128k
  IMAGE/boot.img := resource-img $(1)
endef

define Device/Default-nandflash
  $(Device/Default)
  FILESYSTEMS += squashfs ubifs
  DEVICE_DTS := $(1)
  IMAGES := boot.img rootfs.img
  IMAGE/rootfs.img := append-rootfs | pad-extra 128k
  IMAGE/boot.img := resource-img $(1)
endef


define Device/luckfox_pico-max
  $(Device/Default-emmc)
  DEVICE_TITLE := Luckfox Pico Max
  SUPPORTED_DEVICES := luckfox,pico-max
  SOC := rv1106g
  DEVICE_DTS := rv1106g-luckfox-pico-pro-max
  UBOOT_DEVICE_NAME := luckfox-pico-max-rv1106
endef
TARGET_DEVICES += luckfox_pico-max
