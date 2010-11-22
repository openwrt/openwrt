#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=m68k
BOARD:=coldfire
BOARDNAME:=Freescale Coldfire v4e
FEATURES:=squashfs broken
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.31.14

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for ColdFire v4e boards
endef

$(eval $(call BuildTarget))
