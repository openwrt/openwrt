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
FEATURES:=audio display fpu gpio pcie rtc usb usbgadget squashfs targz nand ubifs
SUBTARGETS:=generic
MAINTAINER:=Luka Perkov <luka@openwrt.org>

KERNEL_PATCHVER:=4.9

define Target/Description
	Build firmware image for Freescale i.MX 6 SoC devices.
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += uboot-envtools

$(eval $(call BuildTarget))
