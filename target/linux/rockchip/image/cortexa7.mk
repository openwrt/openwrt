# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

define Device/Default-emmc
  $(Device/Default-arm32)
  FILESYSTEMS += squashfs
  IMAGES := boot.img rootfs.img
  IMAGE/rootfs.img := append-rootfs | pad-extra 128k
  IMAGE/boot.img := resource-img | boot-arm-bin
endef

define Device/Default-sdcard
  $(Device/Default-arm32)
  FILESYSTEMS = squashfs
  IMAGES := boot.img rootfs.img
  IMAGE/rootfs.img := append-rootfs | pad-extra 128k
  IMAGE/boot.img := resource-img | boot-arm-bin
endef

define Device/Default-spiflash
  $(Device/Default-arm32)
  FILESYSTEMS = squashfs
  IMAGES := boot.img rootfs.img
  IMAGE/rootfs.img := append-rootfs | pad-extra 128k
  IMAGE/boot.img := resource-img | boot-arm-bin
endef

define Device/Default-nandflash
  $(Device/Default-arm32)
  $(Device/Default-sfc-128k)
  FILESYSTEMS := squashfs
  IMAGES := boot.img rootfs.img env.img idblock.img uboot.img
  IMAGE/rootfs.img := append-ubi | pad-to $$$$(PAGESIZE) | check-size $$$$(IMAGE_SIZE)
  IMAGE/boot.img := resource-img | boot-arm-bin
endef

define Device/luckfox_pico-max
  PAGESIZE := 2048
  BLOCKSIZE := 128k
  $(Device/Default-nandflash)
  DEVICE_TITLE := Luckfox Pico Max
  SUPPORTED_DEVICES := luckfox,pico-max
  SOC := rv1106
  MKUBIFS_OPTS := -m 2048 -e 124KiB -c 2114
  DEVICE_DTS := rv1106g-luckfox-pico-pro-max
  UBOOT_DEVICE_NAME := rv1106-sfc
  IMAGES += sysupgrade.img.gz
  IMAGE/sysupgrade.img.gz := env-sfc-img | rockchip32-legacy-bin | append-rootfs | pad-extra 128k | gzip | append-metadata
endef
TARGET_DEVICES += luckfox_pico-max

define Device/luckfox_pico
  $(Device/Default-emmc)
  DEVICE_TITLE := Luckfox Pico
  SUPPORTED_DEVICES := luckfox,pico
  SOC := rv1103
  DEVICE_DTS := rv1103g-luckfox-pico
  UBOOT_DEVICE_NAME := rv1106-emmc
  DEFAULT_PACKAGES += kmod-rknpu-rockchip
  IMAGES += sysupgrade.img.gz
  IMAGE/sysupgrade.img.gz := env-sd-img | rockchip32-legacy-bin | append-rootfs | pad-extra 128k | gzip | append-metadata
endef
TARGET_DEVICES += luckfox_pico
