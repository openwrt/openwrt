# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org

KERNEL_LOADADDR:=0x40080000

define Device/sun55i
  $(call Device/FitImageLzma)
  SUNXI_DTS_DIR := allwinner/
  KERNEL_NAME := Image
endef

define Device/sun55i-a527
  SOC := sun55i-a527
  $(Device/sun55i)
endef
