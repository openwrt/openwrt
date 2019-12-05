#
# Copyright (C) 2013-2016 OpenWrt.org
# Copyright (C) 2016 Yousong Zhou
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
ifeq ($(SUBTARGET),cortexa53)

define Device/friendlyarm_nanopi-neo-plus2
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi NEO Plus2
  SUPPORTED_DEVICES:=nanopi-neo-plus2
  SUNXI_DTS:=allwinner/sun50i-h5-nanopi-neo-plus2
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

TARGET_DEVICES += friendlyarm_nanopi-neo-plus2

define Device/friendlyarm_nanopi-neo2
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi NEO2
  SUPPORTED_DEVICES:=nanopi-neo2
  SUNXI_DTS:=allwinner/sun50i-h5-nanopi-neo2
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

TARGET_DEVICES += friendlyarm_nanopi-neo2

define Device/pine64_pine64-plus
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := Pine64+
  SUNXI_DTS:=allwinner/sun50i-a64-pine64-plus
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

TARGET_DEVICES += pine64_pine64-plus

define Device/pine64_sopine-baseboard
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := SoPine
  SUNXI_DTS:=allwinner/sun50i-a64-sopine-baseboard
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

TARGET_DEVICES += pine64_sopine-baseboard


define Device/xunlong_orangepi-zero-plus
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi Zero Plus
  SUNXI_DTS:=allwinner/sun50i-h5-orangepi-zero-plus
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

TARGET_DEVICES += xunlong_orangepi-zero-plus

define Device/xunlong_orangepi-pc2
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi PC 2
  SUNXI_DTS:=allwinner/sun50i-h5-orangepi-pc2
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

TARGET_DEVICES += xunlong_orangepi-pc2

endif
