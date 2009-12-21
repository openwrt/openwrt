#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=m68k
BOARD:=coldfire
BOARDNAME:=Freescale Coldfire v4e
FEATURES:=squashfs broken

LINUX_VERSION:=2.6.25.20

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for ColdFire v4e boards
endef

$(eval $(call BuildTarget))
