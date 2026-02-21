# SPDX-License-Identifier: GPL-2.0-only

define Device/bt-pon_bt-g711ax
  SOC := rtl9607
  DEVICE_VENDOR := BT-PON
  DEVICE_MODEL := BT-G711AX
  UIMAGE_MAGIC := 0x27051956
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += bt-pon_bt-g711ax
