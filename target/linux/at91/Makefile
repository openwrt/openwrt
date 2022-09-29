# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2013 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=at91
BOARDNAME:=Microchip (Atmel AT91)
FEATURES:=ext4 squashfs targz usbgadget ubifs
SUBTARGETS:=sama7 sama5 sam9x

KERNEL_PATCHVER:=5.10
KERNEL_TESTING_PATCHVER:=5.15

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += kmod-usb-ohci kmod-at91-udc kmod-usb-gadget-eth

$(eval $(call BuildTarget))
