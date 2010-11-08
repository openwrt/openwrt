#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=versatile
BOARDNAME:=ARM Versatile board (qemu)
FEATURES:=squashfs jffs2
CFLAGS:=-Os -pipe -march=armv5te -mtune=arm926ej-s -funit-at-a-time

LINUX_VERSION:=2.6.36

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage"

$(eval $(call BuildTarget))
