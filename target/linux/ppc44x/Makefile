# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc44x
BOARDNAME:=AMCC Taishan
FEATURES:=jffs2

LINUX_VERSION:=2.6.23.14
LINUX_KARCH:=ppc

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for the AMCC Taishan evaluation board
endef

$(eval $(call BuildTarget))
