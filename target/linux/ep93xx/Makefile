#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ep93xx
BOARDNAME:=Cirrus Logic EP93xx SoC
FEATURES:=squashfs jffs2 ext2 tgz usb
CFLAGS:=-Os -pipe -march=armv4t -funit-at-a-time

LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

$(eval $(call BuildTarget))
