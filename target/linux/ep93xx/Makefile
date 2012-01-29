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
FEATURES:=squashfs jffs2 ext4 targz usb
CFLAGS:=-Os -pipe -march=armv4t -fno-caller-saves
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

LINUX_VERSION:=3.2.1

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

$(eval $(call BuildTarget))
