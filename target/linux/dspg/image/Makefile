# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR := 0x40008000

define Device/Default
  PROFILES := Default
  DEVICE_DTS_DIR := ../dts
  KERNEL_NAME := zImage
  KERNEL = kernel-bin | fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS = kernel-bin | fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS_SUFFIX := .itb
  IMAGES :=
endef

$(eval $(call BuildImage))
