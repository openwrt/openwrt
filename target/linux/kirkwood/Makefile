#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=kirkwood
BOARDNAME:=Marvell Kirkwood
FEATURES:=targz usb jffs2
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.35.11

include $(INCLUDE_DIR)/target.mk

CFLAGS:=-Os -pipe -march=armv5te -mtune=marvell-f -funit-at-a-time

KERNELNAME:="uImage"

DEFAULT_PACKAGES +=

$(eval $(call BuildTarget))
