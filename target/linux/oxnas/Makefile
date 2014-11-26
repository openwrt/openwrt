#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=oxnas
BOARDNAME:=PLXTECH NAS782X
DEVICE_TYPE:=nas
FEATURES:=gpio nand pcie usb usbgadget ramdisk rtc squashfs
CPU_TYPE:=mpcore

MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

KERNEL_PATCHVER:=3.18

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-button-hotplug kmod-input-gpio-keys-polled kmod-leds-gpio		\
	kmod-i2c-gpio uboot-envtools uboot-oxnas-ox820

KERNELNAME:=zImage dtbs

$(eval $(call BuildTarget))
