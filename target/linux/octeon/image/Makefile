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

# A generic image per subtarget, for boards of its OCTEON generation
define Device/generic
  DEVICE_VENDOR := Generic
  DEVICE_MODEL := Octeon
  FILESYSTEMS := ext4
endef
TARGET_DEVICES += generic

ifeq ($(SUBTARGET),generic)
ITUSROUTER_CMDLINE:=console=ttyS0,115200 root=/dev/mmcblk1p2 rootfstype=squashfs,ext4,f2fs rootwait
define Device/itus_shield-router
  DEVICE_VENDOR := Itus Networks
  DEVICE_MODEL := Shield Router
  CMDLINE := $(ITUSROUTER_CMDLINE)
  IMAGE/sysupgrade.tar/squashfs += | append-metadata
endef
TARGET_DEVICES += itus_shield-router

define Device/ubnt_edgerouter-e300
  DEVICE_VENDOR := Ubiquiti
  DEVICE_PACKAGES += kmod-gpio-button-hotplug kmod-leds-gpio kmod-of-mdio kmod-sfp kmod-usb3 kmod-usb-dwc3 kmod-usb-storage-uas
  KERNEL := kernel-bin | patch-cmdline | append-dtb-to-elf
  KERNEL_DEPENDS := $$(wildcard $(DTS_DIR)/$(DEVICE_DTS).dts)
  CMDLINE := root=/dev/mmcblk0p2 rootfstype=squashfs,ext4 rootwait
endef

define Device/ubnt_edgerouter-4
  $(Device/ubnt_edgerouter-e300)
  DEVICE_MODEL := EdgeRouter 4
  DEVICE_DTS := cn7130_ubnt_edgerouter-4
endef
TARGET_DEVICES += ubnt_edgerouter-4

define Device/ubnt_edgerouter-6p
  $(Device/ubnt_edgerouter-e300)
  DEVICE_MODEL := EdgeRouter 6P
  DEVICE_DTS := cn7130_ubnt_edgerouter-6p
endef
TARGET_DEVICES += ubnt_edgerouter-6p

else
include $(SUBTARGET).mk
endif

$(eval $(call BuildImage))
