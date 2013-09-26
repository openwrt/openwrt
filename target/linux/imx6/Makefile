#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=imx6
BOARDNAME:=Freescale i.MX 6 Series
FEATURES:=audio display gpio pcie usb usbgadget squashfs targz
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=vfp3
MAINTAINER:=Luka Perkov <luka@openwrt.org>

LINUX_VERSION:=3.10.12

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage dtbs"

DEFAULT_PACKAGES += \
	kmod-usb-chipidea kmod-usb-mxs-phy uboot-envtools

$(eval $(call BuildTarget))
