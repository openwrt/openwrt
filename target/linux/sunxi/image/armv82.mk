# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org

KERNEL_LOADADDR:=0x40080000

define Device/sun55i
  $(call Device/FitImageLzma)
  SUNXI_DTS_DIR := allwinner/
  KERNEL_NAME := Image
endef

define Device/cubie-a5e
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := Cubie A5E
  SUPPORTED_DEVICES += cubie-a5e
  $(Device/sun55i)
  SOC := sun55i-a527
 endef
TARGET_DEVICES += cubie-a5e
