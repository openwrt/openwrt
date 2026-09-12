# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2009-2010 OpenWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/append-dtb-to-elf
	$(TARGET_CROSS)objcopy --update-section .appended_dtb=$(KDIR)/image-$(DEVICE_DTS).dtb $@
endef

define Build/strip-kernel
	# Workaround pre-SDK-1.9.0 u-boot versions not handling the .notes section
	$(TARGET_CROSS)strip -R .notes $@ -o $@.stripped && mv $@.stripped $@
endef

DTS_DIR := $(DTS_DIR)/cavium-octeon

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := vmlinux.elf
  KERNEL_INITRAMFS_NAME := vmlinux-initramfs.elf
  KERNEL := kernel-bin | strip-kernel | patch-cmdline
  IMAGES := sysupgrade.tar
  IMAGE/sysupgrade.tar/squashfs := append-rootfs | pad-extra 128k | sysupgrade-tar rootfs=$$$$@
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
