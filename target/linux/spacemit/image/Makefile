# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

loadaddr-$(CONFIG_TARGET_spacemit_k1) := 0x200000

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | fit lzma
  KERNEL_SUFFIX := .itb
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma "" with-initrd
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_LOADADDR = $(loadaddr-y)
  DEVICE_VENDOR := SpacemiT
  DEVICE_DTS_DIR := $(DTS_DIR)/spacemit
endef

include $(SUBTARGET).mk

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
