#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=kirkwood
BOARDNAME:=Marvell Kirkwood
FEATURES:=targz usb jffs2_nand nand ubifs
CPU_TYPE:=xscale
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.10.44

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage dtbs"

$(eval $(call BuildTarget))
