#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ep93xx
BOARDNAME:=Cirrus Logic EP93xx
FEATURES:=squashfs ext4 targz usb display sound
CPU_TYPE:=arm920t
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

KERNEL_PATCHVER:=3.8

DEVICE_TYPE:=developerboard

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage

$(eval $(call BuildTarget))
