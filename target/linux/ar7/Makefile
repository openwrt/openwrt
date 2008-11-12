 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ar7
BOARDNAME:=TI AR7
FEATURES:=squashfs jffs2 atm

LINUX_VERSION:=2.6.26.8

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for TI AR7 based routers
endef

$(eval $(call BuildTarget))
