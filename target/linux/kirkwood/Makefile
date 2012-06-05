#
# Copyright (C) 2009-2012 OpenWrt.org
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

LINUX_VERSION:=3.3.8

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

DEFAULT_PACKAGES +=

$(eval $(call BuildTarget))
