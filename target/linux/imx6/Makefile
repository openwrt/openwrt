#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=imx6
BOARDNAME:=Freescale i.MX 6
# FEATURES:=audio display fpu gpio pcie rtc usb usbgadget squashfs targz nand ubifs
# CPU_TYPE:=cortex-a9
# CPU_SUBTYPE:=neon
FEATURES:=fpu gpio usb usbgadget targz nand ubifs
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
MAINTAINER:=Luka Perkov <luka@openwrt.org>

# KERNEL_PATCHVER:=4.4
LINUX_VERSION:=4.1

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += uboot-envtools

$(eval $(call BuildTarget))
