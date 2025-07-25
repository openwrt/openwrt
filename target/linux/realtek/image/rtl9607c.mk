# SPDX-License-Identifier: GPL-2.0-only

define Device/realtek_rtl9607c-eng
  SOC := rtl9607c
  DEVICE_VENDOR := Realtek
  DEVICE_MODEL := RTL9607C ENG
  UIMAGE_MAGIC := 0x27051956
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += realtek_rtl9607c-eng
