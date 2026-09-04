# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

KERNEL_LOADADDR:=0x01080000

define Device/hardkernel_odroid-c2
  DEVICE_VENDOR := Hardkernel
  DEVICE_MODEL := ODROID-C2
  DEVICE_DTS := meson-gxbb-odroidc2
  DEVICE_PACKAGES := kmod-fs-vfat kmod-fs-f2fs kmod-ledtrig-heartbeat kmod-usb-dwc2 kmod-usb-storage kmod-usb-gadget-eth \
	kmod-drm-meson kmod-sound-core kmod-sound-soc-meson-gx
endef
TARGET_DEVICES += hardkernel_odroid-c2
