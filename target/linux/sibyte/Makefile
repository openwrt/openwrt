#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=sibyte
BOARDNAME:=Broadcom/SiByte SB-1
FEATURES:=fpu ramdisk
CFLAGS:=-Os -pipe -march=sb1 -funit-at-a-time
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.37.4

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Broadcom/SiByte SB-1 boards
endef

$(eval $(call BuildTarget))
