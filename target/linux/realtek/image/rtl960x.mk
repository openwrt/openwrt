# SPDX-License-Identifier: GPL-2.0-only

define Device/bt-pon_bt-g711ax
  SOC := rtl9607
  DEVICE_VENDOR := BT-PON
  DEVICE_MODEL := BT-G711AX
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += bt-pon_bt-g711ax

define Device/eltex_rg-5520
  SOC := rtl9607
  DEVICE_VENDOR := Eltex
  DEVICE_MODEL := RG-5520
  KERNEL_SIZE := 10m
  IMAGE_SIZE := 30m
  $(Device/kernel-lzma)
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += eltex_rg-5520

