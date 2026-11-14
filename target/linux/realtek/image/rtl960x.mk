# SPDX-License-Identifier: GPL-2.0-only

define Device/alphion_asee-1447
  SOC := rtl9607
  DEVICE_VENDOR := Alphion
  DEVICE_MODEL := ASEE-1447
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 10m
  IMAGE_SIZE := 25m
  $(Device/kernel-lzma)
  KERNEL := $$(KERNEL) | check-size $$(KERNEL_SIZE)
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += alphion_asee-1447

define Device/bt-pon_bt-g711ax
  SOC := rtl9607
  DEVICE_VENDOR := BT-PON
  DEVICE_MODEL := BT-G711AX
  KERNEL_SIZE := 10m
  IMAGE_SIZE := 25m
  $(Device/kernel-lzma)
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += bt-pon_bt-g711ax

define Device/buffalo_wsr-1500ax2s
  SOC := rtl8198d
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WSR-1500AX2S
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  $(Device/kernel-lzma)
  KERNEL := $$(KERNEL) | check-size $$(KERNEL_SIZE)
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += buffalo_wsr-1500ax2s
