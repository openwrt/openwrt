# 
# Copyright (C) 2020 Tobias Maedel
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Device/pine64_rock64
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := Rock64
  SOC := rk3328
  UBOOT_DEVICE_NAME := rock64-rk3328
  IMAGE/sysupgrade.img.gz := boot-common | boot-script | sdcard-img | gzip | append-metadata
  DEVICE_PACKAGES += kmod-drm-rockchip kmod-drm-rockchip-hdmi-sound
endef
TARGET_DEVICES += pine64_rock64

define Device/pine64_rockpro64
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := RockPro64
  SOC := rk3399
  UBOOT_DEVICE_NAME := rockpro64-rk3399
  IMAGE/sysupgrade.img.gz := boot-common | boot-script | sdcard-img | gzip | append-metadata
endef
TARGET_DEVICES += pine64_rockpro64
