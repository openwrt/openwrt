# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2021 Matthew Hagan <mnhagan88@gmail.com>

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=bcm5862x
BOARDNAME:=Broadcom BCM5862x (Northstar Plus)
FEATURES:=squashfs nand usb pci pcie gpio ramdisk
CPU_TYPE:=cortex-a9
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.10

define Target/Description
	Build firmware images for Broadcom Northstar Plus (NSP) devices.
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += kmod-usb-ehci kmod-usb-ohci kmod-usb2 \
	kmod-leds-gpio kmod-ledtrig-default-on kmod-ledtrig-netdev \
	kmod-gpio-button-hotplug

$(eval $(call BuildTarget))
