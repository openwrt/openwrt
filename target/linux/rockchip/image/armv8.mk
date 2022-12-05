# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

# define Device/friendlyarm_nanopi-r2s
#   DEVICE_VENDOR := FriendlyARM
#   DEVICE_MODEL := NanoPi R2S
#   SOC := rk3328
#   UBOOT_DEVICE_NAME := nanopi-r2s-rk3328
#   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r2s | pine64-img | gzip | append-metadata
#   DEVICE_PACKAGES := kmod-usb-net-rtl8152
# endef
# TARGET_DEVICES += friendlyarm_nanopi-r2s

# define Device/friendlyarm_nanopi-r4s
#   DEVICE_VENDOR := FriendlyARM
#   DEVICE_MODEL := NanoPi R4S
#   DEVICE_VARIANT := 4GB LPDDR4
#   SOC := rk3399
#   UBOOT_DEVICE_NAME := nanopi-r4s-rk3399
#   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r4s | pine64-img | gzip | append-metadata
#   DEVICE_PACKAGES := kmod-r8169
# endef
# TARGET_DEVICES += friendlyarm_nanopi-r4s

# define Device/pine64_rockpro64
#   DEVICE_VENDOR := Pine64
#   DEVICE_MODEL := RockPro64
#   SOC := rk3399
#   UBOOT_DEVICE_NAME := rockpro64-rk3399
#   IMAGE/sysupgrade.img.gz := boot-common | boot-script | pine64-img | gzip | append-metadata
# endef
# TARGET_DEVICES += pine64_rockpro64

# define Device/radxa_rock-pi-4a
#   DEVICE_VENDOR := Radxa
#   DEVICE_MODEL := ROCK Pi 4A
#   SOC := rk3399
#   SUPPORTED_DEVICES := radxa,rockpi4a radxa,rockpi4
#   UBOOT_DEVICE_NAME := rock-pi-4-rk3399
#   IMAGE/sysupgrade.img.gz := boot-common | boot-script | pine64-img | gzip | append-metadata
# endef
# TARGET_DEVICES += radxa_rock-pi-4a

define Device/radxa_rock-3a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3A
  SOC := rk3568
  SUPPORTED_DEVICES := radxa,rock3a
  UBOOT_DEVICE_NAME := rock-3a-rk3568
  KERNEL = kernel-bin
  IMAGE/sysupgrade.img.gz := boot-common-legacy | boot-script rk3568-mmc | pine64-img | gzip | append-metadata
endef
TARGET_DEVICES += radxa_rock-3a

define Device/evb_evb1-ddr4-v10
  DEVICE_VENDOR := Rockchip
  DEVICE_MODEL := ROCKCHIP EVB1
  SOC := rk3568
  SUPPORTED_DEVICES := rockchip,rk3568
  UBOOT_DEVICE_NAME := rock-3a-rk3568
  KERNEL = kernel-bin
  IMAGE/sysupgrade.img.gz := boot-common-legacy | boot-script rk3568-mmc | pine64-img | gzip | append-metadata
endef
TARGET_DEVICES += evb_evb1-ddr4-v10
